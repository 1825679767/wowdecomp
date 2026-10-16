// Copyright (C) 2026 WarcraftXL. GPL-3.0-or-later.
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include <algorithm>
#include <cstring>

bool CM2Model::SetSkinningSource(CM2Model* source) {
    if (source == this->m_skinningSource) return true;
    if (source) {
        // Index-copy is safe only for an identical complete bind topology.
        // Differently ordered/subset skeletons require explicit remapping.
        if (source == this || source->m_skinningSource || !this->m_skinnedDependents.empty()
            || this->m_attachParent || this->m_attachList || source->m_scene != this->m_scene
            || !(source->m_fFlags & 1) || !(this->m_fFlags & 1)
            || !source->m_shared || !this->m_shared || !source->m_boneMatrices || !this->m_boneMatrices) return false;
        const auto& from = *source->m_shared->m_data;
        const auto& to = *this->m_shared->m_data;
        if (!to.m_bones.m_count || to.m_bones.m_count > 4096 || to.m_bones.m_count != from.m_bones.m_count
            || to.m_particles.m_count || to.m_ribbons.m_count || to.m_lights.m_count || to.m_attachments.m_count) return false;
        for (uint32_t i = 0; i < to.m_bones.m_count; ++i) {
            const auto& a = from.m_bones[i];
            const auto& b = to.m_bones[i];
            if (a.m_boneId != b.m_boneId || a.m_boneNameCRC != b.m_boneNameCRC || a.m_flags != b.m_flags
                || a.m_parentIndex != b.m_parentIndex || std::memcmp(&a.m_pivot,&b.m_pivot,sizeof(a.m_pivot))) return false;
        }
        // Mutation occurs on the same owning/main thread as native model
        // attach/detach; native scene workers must already be joined there.
        source->m_skinnedDependents.push_back(this);
        ++source->m_refCount;
    }
    CM2Model* previous = this->m_skinningSource;
    this->m_skinningSource = source;
    if (previous) {
        auto& children = previous->m_skinnedDependents;
        children.erase(std::remove(children.begin(),children.end(),this),children.end());
        previous->Release();
    }
    this->SetAnimating(0);
    if (source) source->SetAnimating(1);
    return true;
}

void CM2Model::SynchronizeSkinnedDependentsMT(const C44Matrix* view) {
    for (CM2Model* child : this->m_skinnedDependents) {
        // Source completed its pose on this worker. Dependents are excluded
        // from the independent scene animation list, so no worker races this.
        child->m_worldTransform = this->m_worldTransform;
        child->m_frameOfReference = this->m_frameOfReference;
        child->m_frameOfReferenceMatrix = this->m_frameOfReferenceMatrix;
        if (child->m_fFlags & 0x1000)
            child->AnimateMTSimple(view,this->m_diffuseColor,this->m_emissiveColor,this->m_effectiveAlpha,this->m_alpha);
        else
            child->AnimateMT(view,this->m_diffuseColor,this->m_emissiveColor,this->m_effectiveAlpha,this->m_alpha);
        std::memcpy(child->m_boneMatrices,this->m_boneMatrices,
            this->m_shared->m_data->m_bones.m_count * sizeof(C44Matrix));
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
