// Copyright (C) 2026 WarcraftXL. GPL-3.0-or-later.
// Direct native component integration; portable recipes never inspect host offsets.
#include "AppearanceCompat.hpp"
#include "NativeStorage.hpp"
#include "FdidCompat.hpp"
#include "NativeFeatures.hpp"
#include "WXL/RetailChoices.hpp"
#include "vendor/extensions/wxl-db2/src/schemas/AppearanceStore.hpp"
#include "platform/waystone/CharacterAtlas.hpp"
#include "platform/waystone/AppearanceTexturePolicy.hpp"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "WoW/Common/TextureCache.h"
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Shared.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Services/Texture.h"
#include "common/Log.hpp"
#include <algorithm>
#include <array>
#include <cstring>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace wxl::native {
namespace {
namespace recipe = wxl::runtime::db2::appearance;
namespace atlas = wxl::waystone::appearance;
constexpr uint32_t MaxDimension = 4096, MaxChoices = 8192, MaxSections = 128;

uint32_t RetailRace(uint32_t race) {
    return RetailRaceId(race);
}

std::string PathKey(const char* path) {
    std::string value = path ? path : "";
    for (char& c : value) {
        if (c == '/') c = '\\';
        if (c >= 'A' && c <= 'Z') c = char(c - 'A' + 'a');
    }
    if (value.ends_with(".mdx")) value.replace(value.size()-4,4,".m2");
    return value;
}

struct Sheet {
    uint32_t width = 0, height = 0;
    std::vector<std::vector<uint32_t>> levels;
    CTexture* texture = nullptr;
    ~Sheet() {
        if (texture) {
            if (auto* gx = TextureGetGxTex(texture, 0, nullptr)) GxTexSetCannotUpdate(gx);
            HandleClose(reinterpret_cast<HOBJECT>(texture));
        }
    }
};

void UpdateSheet(EGxTexCommand command, uint32_t width, uint32_t, uint32_t,
                 uint32_t level, void* userArg, uint32_t& stride, const void*& texels) {
    const auto* sheet = static_cast<const Sheet*>(userArg);
    if (command == GxTex_Latch && sheet && level < sheet->levels.size()) {
        stride = 4 * width;
        texels = sheet->levels[level].data();
    }
}

bool FinishSheet(Sheet& sheet) {
    if (!g_theGxDevicePtr || !sheet.width || !sheet.height || sheet.levels.empty()) return false;
    const auto caps = g_theGxDevicePtr->Caps();
    if (sheet.width > caps.m_texMaxSize[0] || sheet.height > caps.m_texMaxSize[0]) return false;
    atlas::Mips(sheet.width, sheet.height, sheet.levels);
    CGxTexFlags flags(GxTex_LinearMipLinear, 0, 0, 0, 0, 0, 1);
    sheet.texture = TextureCreate(sheet.width, sheet.height, GxTex_Argb8888, GxTex_Argb8888,
        flags, &sheet, &UpdateSheet, "WXLNativeAppearance", 1);
    return sheet.texture != nullptr;
}

bool ReadImage(const char* path, atlas::Image& out, bool blended) {
    std::vector<uint8_t> bytes;
    return path && ReadAsset(path, bytes) && atlas::DecodeLayerImage(bytes, out, blended)
        && out.width && out.height && out.width <= MaxDimension && out.height <= MaxDimension;
}

std::unique_ptr<Sheet> WholeSheet(const atlas::Image& image) {
    auto sheet = std::make_unique<Sheet>();
    sheet->width = image.width;
    sheet->height = image.height;
    sheet->levels.emplace_back(size_t(image.width) * image.height);
    auto& pixels = sheet->levels[0];
    for (size_t i = 0; i < pixels.size(); ++i) {
        const auto* p = image.rgba.data() + i * 4;
        pixels[i] = uint32_t(p[0]) << 16 | uint32_t(p[1]) << 8 | p[2] | uint32_t(p[3]) << 24;
    }
    return FinishSheet(*sheet) ? std::move(sheet) : nullptr;
}

atlas::Rect Span(const std::vector<WXL_TextureSection>& sections, uint32_t mask,
                 uint32_t width, uint32_t height) {
    uint32_t left = width, top = height, right = 0, bottom = 0;
    for (const auto& r : sections) if (r.sectionType < 32 && (mask & (1u << r.sectionType))) {
        left = (std::min)(left, r.x); top = (std::min)(top, r.y);
        right = (std::max)(right, r.x + r.width); bottom = (std::max)(bottom, r.y + r.height);
    }
    return right > left && bottom > top ? atlas::Rect{left, top, right-left, bottom-top} : atlas::Rect{};
}

std::vector<uint32_t> Signature(const CCharacterComponent& component) {
    const auto& p = component.m_data.m_preferences;
    std::vector<uint32_t> values{p.m_raceID,p.m_sexID,p.m_skinID,p.m_faceId,
        p.m_hairStyleId,p.m_hairColorId,p.m_facialHairId,p.m_classId,component.m_flags & 0x20};
    for (auto id : component.m_itemDisplayID) values.push_back(id);
    for (const auto& section : component.m_section) {
        values.push_back(section.m_layerMask);
        for (auto id : section.m_layerItemDisplayId) values.push_back(id);
    }
    values.push_back(RetailChoiceRevision(component));
    return values;
}

struct SkinnedChild {
    CM2Model* model = nullptr;
    ~SkinnedChild() {
        if (model) {
            model->SetSkinningSource(nullptr);
            model->Release();
        }
    }
};
}

struct AppearanceAssets {
    CM2Model* model = nullptr; // identity only; component owns the model
    std::vector<uint32_t> signature;
    std::map<uint32_t,std::unique_ptr<Sheet>> sheets;
    std::map<uint32_t,CTexture*> original;
    std::vector<uint16_t> ownedGeosets, selectedGeosets;
    std::vector<std::unique_ptr<SkinnedChild>> children;
    bool ready = false;
    ~AppearanceAssets() {
        for (const auto& [type, texture] : original)
            if (texture) HandleClose(reinterpret_cast<HOBJECT>(texture));
    }
};

namespace {
bool Build(CCharacterComponent& component, AppearanceAssets& output) {
    const auto& prefs = component.m_data.m_preferences;
    const uint32_t race = RetailRace(prefs.m_raceID);
    const auto modelId = recipe::ChrModelForRace(race, prefs.m_sexID);
    const auto optionCount = recipe::OptionCount(modelId);
    if (!modelId || !optionCount || optionCount > 256) return false;
    const uint32_t layout = recipe::LayoutForModel(modelId);
    auto body = std::make_unique<Sheet>();
    if (!recipe::LayoutSize(layout, body->width, body->height) || !body->width || !body->height
        || body->width > MaxDimension || body->height > MaxDimension) return false;
    const uint32_t sectionCount = recipe::SectionCount(layout);
    if (!sectionCount || sectionCount > MaxSections) return false;
    std::vector<WXL_TextureSection> sections;
    std::vector<atlas::Rect> rects;
    for (uint32_t i = 0; i < sectionCount; ++i) {
        WXL_TextureSection section{};
        if (!recipe::SectionAt(layout,i,section) || !section.width || !section.height
            || section.sectionType >= 32 || uint64_t(section.x)+section.width > body->width
            || uint64_t(section.y)+section.height > body->height) return false;
        sections.push_back(section);
        rects.push_back({section.x,section.y,section.width,section.height});
    }
    std::vector<uint32_t> choices, allChoices;
    if (!CollectRetailChoices(component,choices) || choices.size() > MaxChoices) return false;
    for (uint32_t i = 0; i < optionCount; ++i) {
        WXL_ChrOption option{};
        if (!recipe::OptionAt(modelId,i,option)) return false;
        const auto count = recipe::ChoiceCount(option.id);
        if (!count || count > MaxChoices || allChoices.size()+count > MaxChoices) return false;
        for (uint32_t j = 0; j < count; ++j) {
            WXL_ChrChoice choice{};
            if (!recipe::ChoiceAt(option.id,j,choice)) return false;
            allChoices.push_back(choice.id);
        }
    }
    recipe::Recipe appearance, all;
    if (!recipe::BuildForCharacter(race,prefs.m_sexID,choices.data(),uint32_t(choices.size()),appearance)
        || appearance.textureLayoutId != layout || appearance.layers.empty() || appearance.layers.size()>512
        || !recipe::BuildForCharacter(race,prefs.m_sexID,allChoices.data(),uint32_t(allChoices.size()),all)) return false;
    const char* actualPath = ResolveModel(appearance.modelFileDataId);
    if (!actualPath || PathKey(actualPath) != PathKey(component.m_data.m_model->m_shared->m_fileName)) return false;
    if (all.geosets.size() > MaxChoices || appearance.geosets.size() > MaxChoices) return false;
    output.ownedGeosets = std::move(all.geosets);
    output.selectedGeosets = appearance.geosets;
    body->levels.emplace_back(size_t(body->width)*body->height,0xff000000u);
    uint32_t painted = 0;
    for (const auto& layer : appearance.layers) {
        if (layer.textureType != 1) continue;
        atlas::Image image;
        if (!ReadImage(ResolveTexture(ResolveMaterialTexture(layer.materialResourceId,0)),image,layer.blendMode != 1)) return false;
        const auto span = Span(sections,layer.sectionMask,body->width,body->height);
        std::vector<atlas::Rect> done;
        for (const auto& section : sections) {
            if (!(layer.sectionMask & (1u << section.sectionType))) continue;
            atlas::Rect r{section.x,section.y,section.width,section.height};
            if (std::any_of(done.begin(),done.end(),[&](const auto& p){return p.x==r.x && p.y==r.y && p.width==r.width && p.height==r.height;})) continue;
            done.push_back(r);
            if (!atlas::Paint(body->levels[0],body->width,body->height,image,span,r,layer.blendMode)) return false;
            ++painted;
        }
        if (layer.blendMode == 1 && layer.sectionMask == 0xffffffffu
            && !atlas::PaintBaseDetails(body->levels[0],body->width,body->height,image,span,rects,layout)) return false;
    }
    if (!painted) return false;
    // Native item priorities have already resolved the layers. Paint the same
    // handles into their retail rectangles instead of the global classic sheet.
    for (uint32_t region = 0; region < NUM_COMPONENT_SECTIONS; ++region) {
        const auto& nativeSection = component.m_section[region];
        for (uint32_t priority = 0; priority < 7; ++priority) {
            void* handle = nativeSection.m_layerTex[priority];
            if (!handle) continue;
            TCTEXTUREINFO info{};
            if (!TextureCacheGetInfo(handle,info,1)) return false;
            const auto* entry = static_cast<const CACHEENTRY*>(handle);
            atlas::Image image;
            if (!ReadImage(entry->m_fileName,image,true)) return false;
            bool found = false;
            for (const auto& section : sections) if (section.sectionType == region) {
                atlas::Rect rect{section.x,section.y,section.width,section.height};
                if (!atlas::Paint(body->levels[0],body->width,body->height,image,rect,rect,9)) return false;
                found = true;
            }
            if (!found) return false;
        }
    }
    if (!FinishSheet(*body)) return false;
    output.sheets.emplace(1,std::move(body));
    const auto whole = atlas::SelectWholeLayers(race,prefs.m_sexID,appearance.layers,choices);
    for (uint32_t type = 0; type < whole.size(); ++type) if (whole[type].materialResourceId) {
        atlas::Image image;
        if (!ReadImage(ResolveTexture(ResolveMaterialTexture(whole[type].materialResourceId,0)),image,false)) return false;
        auto sheet = WholeSheet(image);
        if (!sheet) return false;
        output.sheets.emplace(type,std::move(sheet));
    }
    if (appearance.attached.size() > 64) return false;
    std::vector<uint32_t> childFiles;
    for (const auto& attached : appearance.attached) {
        if (!attached.fileDataId || std::find(childFiles.begin(),childFiles.end(),attached.fileDataId) != childFiles.end()) continue;
        childFiles.push_back(attached.fileDataId);
        const char* path = ResolveModel(attached.fileDataId);
        if (!path) return false;
        auto child = std::make_unique<SkinnedChild>();
        child->model = component.m_data.m_model->m_scene->CreateModel(path,0);
        if (!child->model || !child->model->IsLoaded(1,0) || !child->model->m_shared
            || PathKey(child->model->m_shared->m_fileName) != PathKey(path)) return false;
        const auto* skin = child->model->m_shared->m_skinData;
        // Authored no-geometry choices deliberately suppress an appendage.
        if (!skin || !skin->m_indices.m_count || !skin->m_batches.m_count) continue;
        if (!child->model->SetSkinningSource(component.m_data.m_model)) {
            WLOG_WARN("native-appearance: incompatible skinned child fdid=%u geoset=%u path=%s",attached.fileDataId,attached.geoset,path);
            return false;
        }
        child->model->SetGeometryVisible(0,65535,1);
        child->model->OptimizeVisibleGeometry();
        for (const auto& [type,sheet] : output.sheets) child->model->ReplaceTexture(type,sheet->texture);
        output.children.push_back(std::move(child));
    }
    WLOG_INFO("native-appearance: race=%u sex=%u model=%u layout=%u size=%ux%u sheets=%zu choices=%zu skinned-children=%zu",
        race,prefs.m_sexID,modelId,layout,output.sheets.at(1)->width,output.sheets.at(1)->height,output.sheets.size(),choices.size(),output.children.size());
    return true;
}

void Bind(CCharacterComponent& component, AppearanceAssets& state) {
    if (component.m_flags & 4) component.GeosRenderPrep();
    for (uint16_t geoset : state.ownedGeosets) component.m_data.m_model->SetGeometryVisible(geoset,geoset,0);
    for (uint16_t geoset : state.selectedGeosets) component.m_data.m_model->SetGeometryVisible(geoset,geoset,1);
    component.m_data.m_model->OptimizeVisibleGeometry();
    for (const auto& [type,sheet] : state.sheets) component.m_data.m_model->ReplaceTexture(type,sheet->texture);
    for (const auto& child : state.children)
        for (const auto& [type,sheet] : state.sheets) child->model->ReplaceTexture(type,sheet->texture);
    component.m_dirtySections = 0;
    component.m_flags &= ~1u;
}

void Restore(CCharacterComponent& component) {
    if (auto* state = component.m_wxlAppearance; state && state->ready && state->model == component.m_data.m_model) {
        for (const auto& [type,texture] : state->original) component.m_data.m_model->ReplaceTexture(type,texture);
        component.m_dirtySections = 0xffffffffu;
        component.m_flags |= 1u | 4u;
    }
    ReleaseAppearance(component);
}
}

bool ApplyAppearance(CCharacterComponent& component) noexcept {
    try {
        auto* model = component.m_data.m_model;
        if (!AppearancesEnabled() || !AssetsEnabled() || !model || !(model->m_fFlags & 1)
            || !model->m_shared || !model->m_shared->m_wxlAssets || !RetailRace(component.m_data.m_preferences.m_raceID)
            || component.m_data.m_preferences.m_sexID > 1 || (component.m_data.m_flags & 1)) {
            if (component.m_wxlAppearance) Restore(component);
            return false;
        }
        if (component.m_request) return false; // existing worker retains its ownership
        // A context outage is transient; do not memoize it as a failed recipe.
        if (!g_theGxDevicePtr || !g_theGxDevicePtr->CapsHasContext(-1)) return false;
        const auto signature = Signature(component);
        auto* current = component.m_wxlAppearance;
        if (current && current->model == model && current->signature == signature) {
            if (current->ready && (component.m_dirtySections || (component.m_flags & 5))) Bind(component,*current);
            return current->ready;
        }
        auto next = std::make_unique<AppearanceAssets>();
        next->model = model;
        next->signature = signature;
        if (!Build(component,*next)) {
            if (current) Restore(component);
            component.m_wxlAppearance = next.release(); // cache this failed immutable recipe
            return false;
        }
        // Capture the actual source-native replaceable slot handles for rollback.
        const auto& textures = model->m_shared->m_data->m_textures;
        for (const auto& [type,sheet] : next->sheets) {
            CTexture* original = nullptr;
            if (current && current->ready && current->model == model && current->original.count(type)) original = current->original.at(type);
            else for (uint32_t i = 0; i < textures.m_count; ++i) if (textures[i].m_type == type) {original = model->m_textures[i]; break;}
            next->original.emplace(type,original ? reinterpret_cast<CTexture*>(HandleDuplicate(reinterpret_cast<HOBJECT>(original))) : nullptr);
        }
        next->ready = true;
        if (current && current->ready && current->model == model)
            for (const auto& [type,texture] : current->original)
                if (!next->sheets.count(type)) model->ReplaceTexture(type,texture);
        Bind(component,*next);
        ReleaseAppearance(component);
        component.m_wxlAppearance = next.release();
        return true;
    } catch (...) {
        // Failed allocation/metadata never destroys the classic source path.
        if (component.m_wxlAppearance) Restore(component);
        return false;
    }
}

void ReleaseAppearance(CCharacterComponent& component) noexcept {
    delete component.m_wxlAppearance;
    component.m_wxlAppearance = nullptr;
}
}
