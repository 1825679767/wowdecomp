// Copyright (C) 2026 WarcraftXL. GPL-3.0-or-later.
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include <algorithm>
#include <cmath>
#include <cstring>

namespace {
constexpr uint16_t NoParent = 0xffff;
constexpr uint32_t MaxBones = 4096, MaxCandidates = 131072;
constexpr uint32_t MaxConstraintWork = 16u * 1024 * 1024;

struct BoneTopology {
    std::vector<uint16_t> depth;
    std::vector<std::vector<uint16_t>> children;
};

bool ReadTopology(const M2Array<M2CompBone>& bones, BoneTopology& topology) {
    const auto count = bones.m_count;
    if (!count || count > MaxBones) return false;
    topology.depth.assign(count,NoParent);
    topology.children.resize(count);
    std::vector<uint16_t> order;
    order.reserve(count);
    for (uint32_t i = 0; i < count; ++i) {
        const auto& bone = bones[i];
        if (!std::isfinite(bone.m_pivot.x) || !std::isfinite(bone.m_pivot.y) || !std::isfinite(bone.m_pivot.z)) return false;
        if (bone.m_parentIndex == NoParent) {
            topology.depth[i] = 0;
            order.push_back(static_cast<uint16_t>(i));
        } else {
            if (bone.m_parentIndex >= count || bone.m_parentIndex == i) return false;
            topology.children[bone.m_parentIndex].push_back(static_cast<uint16_t>(i));
        }
    }
    // Parent order in the file is irrelevant; every bone must be reachable
    // from a root. A remaining node implies a cycle, not another mapping rule.
    for (size_t at = 0; at < order.size(); ++at) {
        const auto parent = order[at];
        for (auto child : topology.children[parent]) {
            topology.depth[child] = topology.depth[parent] + 1;
            order.push_back(child);
        }
    }
    return order.size() == count;
}

bool SameBindBone(const M2CompBone& a, const M2CompBone& b) {
    // No nearest-pivot heuristic or index fallback: exact native identifiers,
    // transform flags and finite bind-space pivot.
    // Missing IDs/CRCs can be admitted only when the remaining exact geometry
    // and parent/child constraints still leave one unique correspondence.
    return a.m_boneId == b.m_boneId && a.m_boneNameCRC == b.m_boneNameCRC
        && a.m_flags == b.m_flags
        && !std::memcmp(&a.m_pivot,&b.m_pivot,sizeof(a.m_pivot));
}

bool BuildBoneMap(const M2Array<M2CompBone>& source, const M2Array<M2CompBone>& child,
                  std::vector<uint16_t>& output) {
    BoneTopology from, to;
    if (!ReadTopology(source,from) || !ReadTopology(child,to) || child.m_count > source.m_count) return false;
    std::vector<std::vector<uint16_t>> candidates(child.m_count);
    uint32_t total = 0;
    for (uint32_t i = 0; i < child.m_count; ++i) {
        for (uint32_t j = 0; j < source.m_count; ++j) {
            if (to.depth[i] == from.depth[j] && SameBindBone(child[i],source[j])
                && to.children[i].size() <= from.children[j].size()) {
                if (++total > MaxCandidates) return false;
                candidates[i].push_back(static_cast<uint16_t>(j));
            }
        }
        if (candidates[i].empty()) return false;
    }
    uint32_t work = 0;
    bool changed;
    do {
        changed = false;
        // Injective: a source bone cannot stand for two distinct child bones.
        std::vector<int32_t> owner(source.m_count,-1);
        for (uint32_t i = 0; i < child.m_count; ++i) if (candidates[i].size() == 1) {
            auto& claimed = owner[candidates[i][0]];
            if (claimed != -1) return false;
            claimed = static_cast<int32_t>(i);
        }
        for (uint32_t i = 0; i < child.m_count; ++i) {
            auto& values = candidates[i];
            std::vector<uint16_t> retained;
            retained.reserve(values.size());
            for (uint16_t candidate : values) {
                if (++work > MaxConstraintWork) return false;
                bool valid = owner[candidate] == -1 || owner[candidate] == static_cast<int32_t>(i);
                const auto childParent = child[i].m_parentIndex;
                const auto sourceParent = source[candidate].m_parentIndex;
                if (valid && childParent == NoParent) valid = sourceParent == NoParent;
                else if (valid) {
                    const auto& parents = candidates[childParent];
                    valid = sourceParent != NoParent && std::binary_search(parents.begin(),parents.end(),sourceParent);
                }
                // Descendant identity can distinguish otherwise identical
                // parent geometry. Subsets may omit siblings, never ancestors.
                for (uint16_t next : to.children[i]) {
                    if (!valid) break;
                    bool supported = false;
                    for (uint16_t target : candidates[next]) {
                        if (++work > MaxConstraintWork) return false;
                        if (source[target].m_parentIndex == candidate) {supported = true; break;}
                    }
                    valid = supported;
                }
                if (valid) retained.push_back(candidate);
            }
            changed = changed || retained.size() != values.size();
            values.swap(retained);
            if (values.empty()) return false;
        }
    } while (changed);
    std::vector<uint16_t> map;
    map.reserve(child.m_count);
    for (const auto& values : candidates) {
        // Deliberately refuse unresolved symmetric candidates instead of
        // guessing an index or searching until an arbitrary solution appears.
        if (values.size() != 1) return false;
        map.push_back(values[0]);
    }
    for (uint32_t i = 0; i < child.m_count; ++i) {
        const auto parent = child[i].m_parentIndex;
        if (source[map[i]].m_parentIndex != (parent == NoParent ? NoParent : map[parent])) return false;
    }
    output.swap(map);
    return true;
}
}

bool CM2Model::SetSkinningSource(CM2Model* source) {
    if (source == this->m_skinningSource) return true;
    std::vector<uint16_t> boneMap;
    if (source) {
        if (source == this || source->m_skinningSource || !this->m_skinnedDependents.empty()
            || this->m_attachParent || this->m_attachList || source->m_scene != this->m_scene
            || !(source->m_fFlags & 1) || !(this->m_fFlags & 1)
            || !source->m_shared || !this->m_shared || !source->m_boneMatrices || !this->m_boneMatrices) return false;
        const auto& from = *source->m_shared->m_data;
        const auto& to = *this->m_shared->m_data;
        // Effects run through the child's existing native AnimateMT/AnimateST
        // owner after its matrices are replaced by the verified source pose.
        // Attached descendants need another pose-propagation level; cameras
        // must not acquire view authority merely by being a customization mesh.
        if (to.m_attachments.m_count || to.m_cameras.m_count
            || !BuildBoneMap(from.m_bones,to.m_bones,boneMap)) return false;
        if ((to.m_particles.m_count && !this->m_particleEmitters)
            || (to.m_ribbons.m_count && !this->m_ribbonEmitters)
            || (to.m_lights.m_count && !this->m_lights)) return false;
        for (uint32_t i=0;i<to.m_particles.m_count;++i)
            if (to.m_particles[i].m_boneIndex>=boneMap.size() || !this->m_particleEmitters[i]) return false;
        for (uint32_t i=0;i<to.m_ribbons.m_count;++i)
            if (to.m_ribbons[i].m_boneIndex>=boneMap.size() || !this->m_ribbonEmitters[i]) return false;
        for (uint32_t i=0;i<to.m_lights.m_count;++i)
            if (to.m_lights[i].m_bone>=boneMap.size()) return false;
        // Mutation occurs on the same owning/main thread as native model
        // attach/detach; native scene workers must already be joined there.
        source->m_skinnedDependents.push_back(this);
        ++source->m_refCount;
    }
    CM2Model* previous = this->m_skinningSource;
    this->m_skinningSource = source;
    this->m_skinningBoneMap.swap(boneMap);
    if (previous) {
        auto& children = previous->m_skinnedDependents;
        children.erase(std::remove(children.begin(),children.end(),this),children.end());
        previous->Release();
    }
    this->SetAnimating(0);
    if (source) {
        this->SetEmittersEnabled(source->m_wxlEmittersEnabled);
        this->SetRibbonsEnabled(source->m_wxlRibbonsEnabled);
        source->SetAnimating(1);
    }
    return true;
}

void CM2Model::SynchronizeSkinnedDependentsMT(const C44Matrix* view) {
    for (CM2Model* child : this->m_skinnedDependents) {
        // Source completed its pose on this worker. Dependents are excluded
        // from the independent scene animation list, so no worker races this.
        child->m_worldTransform = this->m_worldTransform;
        // Inherit transform/visibility and particle permission exactly as an
        // ordinary attached native model, preserving each child's user bit.
        child->m_fFlags=(child->m_fFlags&~0x18008u)|(this->m_fFlags&0x8008u)
            | ((this->m_fFlags&0x10000u)&&(child->m_fFlags&0x20000u)?0x10000u:0u);
        child->m_frameOfReference = this->m_frameOfReference;
        child->m_frameOfReferenceMatrix = this->m_frameOfReferenceMatrix;
        if (child->m_fFlags & 0x1000)
            child->AnimateMTSimple(view,this->m_diffuseColor,this->m_emissiveColor,this->m_effectiveAlpha,this->m_alpha);
        else
            child->AnimateMT(view,this->m_diffuseColor,this->m_emissiveColor,this->m_effectiveAlpha,this->m_alpha);
        for (size_t i = 0; i < child->m_skinningBoneMap.size(); ++i)
            child->m_boneMatrices[i] = this->m_boneMatrices[child->m_skinningBoneMap[i]];
        child->m_matrixF4 = this->m_matrixF4;
        child->m_scaleSq = this->m_scaleSq;
        child->m_effectiveAlpha = this->m_effectiveAlpha;
        child->m_alpha = this->m_alpha;
        child->m_diffuseColor = this->m_diffuseColor;
        child->m_emissiveColor = this->m_emissiveColor;
        child->m_fFlags = (child->m_fFlags & ~8u) | (this->m_fFlags & 8u);
        child->m_frameStamp = this->m_frameStamp;
    }
}
