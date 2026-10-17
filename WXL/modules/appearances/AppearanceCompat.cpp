#include "WXL/EquipmentAttachments.hpp"
#include "WXL/ItemGraph.hpp"
#include "WXL/PrivateAppearances.hpp"
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
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Shared.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Os/TimeManager.h"
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
    bool generated = false;
    ~Sheet() {
        if (texture) {
            if (generated)if (auto* gx = TextureGetGxTex(texture, 0, nullptr)) GxTexSetCannotUpdate(gx);
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
    sheet.generated = true;
    return sheet.texture != nullptr;
}

bool ReadImage(const char* path, atlas::Image& out, bool blended) {
    std::vector<uint8_t> bytes;
    const bool ok=path && ReadAsset(path, bytes) && atlas::DecodeLayerImage(bytes, out, blended)
        && out.width && out.height && out.width <= MaxDimension && out.height <= MaxDimension;
    static uint32_t failures=0;
    if(!ok&&failures++<24)WLOG_WARN("native-appearance: image read/decode failed path=%s bytes=%zu",path?path:"<unresolved>",bytes.size());
    return ok;
}

std::unique_ptr<Sheet> WholeTexture(const char* path) {
    if(!path||!*path)return nullptr;
    auto sheet = std::make_unique<Sheet>();
    CStatus status;
    sheet->texture=TextureCreate(path,CGxTexFlags(),&status,0);
    // Match donor LoadResource: retain the native file/alpha/mip/cache owner,
    // rather than decode a separate replacement with different texture flags.
    if(!sheet->texture||!TextureGetGxTex(sheet->texture,1,&status)||status.m_highestSeverity>=STATUS_ERROR)return nullptr;
    return sheet;
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
        // Display IDs can stay identical when the actor's component variant
        // changes. Preserve exact loaded cache identity in this immutable atlas key.
        for (uint32_t layer=0;layer<7;++layer) {
            const auto* entry=static_cast<const CACHEENTRY*>(section.m_layerTex[layer]);
            // Native cleanup retires item handles after composition. Item IDs,
            // actor identity and metadata generation retain their recipe identity.
            const char* path=entry&&!section.m_layerItemDisplayId[layer]?entry->m_fileName:"";
            const auto length=std::strlen(path);values.push_back(uint32_t(length));
            for (size_t offset=0;offset<length;offset+=4) {
                uint32_t word=0;for(size_t byte=0;byte<4&&offset+byte<length;++byte)word|=uint32_t(uint8_t(path[offset+byte]))<<(byte*8);
                values.push_back(word);
            }
        }
    }
    values.push_back(uint32_t(component.m_wxlItemGeneration));
    values.push_back(uint32_t(component.m_wxlItemGeneration>>32));
    values.push_back(RetailChoiceRevision(component));
    return values;
}

struct SkinnedChild {
    CM2Model* model = nullptr;
    ~SkinnedChild() {
        if (model) {
            model->SetEmittersEnabled(0);
            model->SetRibbonsEnabled(0);
            model->SetSkinningSource(nullptr);
            model->Release();
        }
    }
};
struct ChildGroup {
    uint32_t fdid=0,attempts=0,retryAt=0,startedAt=0;
    std::vector<uint16_t> geosets;
    std::unique_ptr<SkinnedChild> child;
    bool ready=false,exhausted=false;
};
struct WholeGroup {
    uint32_t type=0,material=0,attempts=0,retryAt=0;
    bool ready=false,exhausted=false;
};
}

struct AppearanceAssets {
    CM2Model* model = nullptr; // identity only; component owns the model
    uint32_t chrModel = 0;
    std::vector<uint32_t> signature;
    std::map<uint32_t,std::unique_ptr<Sheet>> sheets;
    std::map<uint32_t,CTexture*> original;
    std::vector<uint16_t> ownedGeosets, selectedGeosets;
    std::vector<ChildGroup> children;
    std::vector<WholeGroup> whole;
    uint32_t buildAttempts=0,retryAt=0;
    bool ready = false;
    ~AppearanceAssets() {
        for (const auto& [type, texture] : original)
            if (texture) HandleClose(reinterpret_cast<HOBJECT>(texture));
    }
};

namespace {
bool Build(CCharacterComponent& component, AppearanceAssets& output) {
    const auto& prefs = component.m_data.m_preferences;
    const uint32_t race = RetailRecipeRace(component);
    const auto modelId = recipe::ChrModelForRace(race, prefs.m_sexID);
    output.chrModel = modelId;
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
    std::vector<RetailChoicePair> effectiveOptions;
    if(!CaptureRetailChoicePairs(component,effectiveOptions))return false;
    for (uint32_t i = 0; i < effectiveOptions.size(); ++i) {
        WXL_ChrOption option{};option.id=effectiveOptions[i].option;
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
    if(race==23){recipe::Recipe human,allHuman;
        if(!recipe::BuildForCharacter(1,prefs.m_sexID,choices.data(),uint32_t(choices.size()),human))return false;
        if(human.modelFileDataId==appearance.modelFileDataId){
            for(const auto& layer:human.layers)if(layer.textureType==19)appearance.layers.push_back(layer);
            if(!recipe::BuildForCharacter(1,prefs.m_sexID,allChoices.data(),uint32_t(allChoices.size()),allHuman))return false;
            for(auto geoset:human.geosets)if(geoset/100==33)appearance.geosets.push_back(geoset);
            for(auto geoset:allHuman.geosets)if(geoset/100==33)all.geosets.push_back(geoset);
        }
    }
    // Current donor SelectionFor resolves independent geosets one option at
    // a time AND merges the settled related-choice recipe. Neither pass is a
    // substitute for the other. The native port previously kept only the latter.
    const auto append=[](std::vector<uint16_t>& ids,uint16_t id){
        if(std::find(ids.begin(),ids.end(),id)==ids.end())ids.push_back(id);
    };
    const auto actor=PathKey(actualPath);
    const bool voidElfMale=actor.find("voidelf\\male")!=std::string::npos;
    bool voidElfAdvanced=false;
    bool hasEyeTexture=std::any_of(appearance.layers.begin(),appearance.layers.end(),[](const auto& layer){return layer.textureType==19;});
    for(const auto& selected:effectiveOptions){
        const auto* optionName=recipe::OptionName(selected.option);
        const auto* choiceName=recipe::ChoiceName(selected.choice);
        if(voidElfMale&&optionName&&choiceName&&std::strcmp(choiceName,"None")
           &&(!std::strcmp(optionName,"Horns")||!std::strcmp(optionName,"Blindfold")||!std::strcmp(optionName,"Tattoo")))voidElfAdvanced=true;
        if(actor.find("orc\\female")!=std::string::npos&&optionName&&!std::strcmp(optionName,"Eyesight")){
            for(uint16_t id:{5101,5102,5103})append(all.geosets,id);
            if(choiceName){
                if(!std::strcmp(choiceName,"Both"))append(appearance.geosets,5101);
                else if(!std::strcmp(choiceName,"Right"))append(appearance.geosets,5102);
                else if(!std::strcmp(choiceName,"Left"))append(appearance.geosets,5103);
            }
        }
        recipe::Recipe one;
        if(recipe::BuildForCharacter(race,prefs.m_sexID,&selected.choice,1,one)){
            for(auto id:one.geosets)append(appearance.geosets,id);
            hasEyeTexture|=std::any_of(one.layers.begin(),one.layers.end(),[](const auto& layer){return layer.textureType==19;});
        }
    }
    // Material-only Eye Color/Eye Style choices still need the authored
    // opaque iris/sclera. Preserve any explicit group33 choice over this fallback.
    if(hasEyeTexture&&!std::any_of(appearance.geosets.begin(),appearance.geosets.end(),[](uint16_t id){return id/100==33;}))append(appearance.geosets,3301);
    if(voidElfMale){
        const auto& skin=component.m_data.m_model->m_shared->m_skinData->m_skinSections;
        for(uint32_t i=0;i<skin.m_count;++i)if(skin[i].m_skinSectionId/100==24)append(all.geosets,skin[i].m_skinSectionId);
        if(!voidElfAdvanced)std::erase_if(appearance.attached,[](const auto& attached){return attached.fileDataId==7760207u;});
    }
    if(all.geosets.size()>MaxChoices||appearance.geosets.size()>MaxChoices)return false;
    output.ownedGeosets = std::move(all.geosets);
    output.selectedGeosets = appearance.geosets;
    body->levels.emplace_back(size_t(body->width)*body->height,0xff000000u);
    uint32_t painted = 0;
    for (const auto& layer : appearance.layers) {
        if (layer.textureType != 1) continue;
        atlas::Image image;
        if (!ReadImage(ResolveTexture(ResolveMaterialTexture(layer.materialResourceId,layer.textureType)),image,layer.blendMode != 1)) return false;
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
    static uint32_t summaries=0;
    if(summaries++<64){
        uint64_t rgb=0,nonblack=0;
        for(auto pixel:body->levels[0]){const auto sum=((pixel>>16)&255)+((pixel>>8)&255)+(pixel&255);rgb+=sum;nonblack+=sum!=0;}
        WLOG_INFO("native-appearance: atlas race=%u sex=%u model=%u layout=%u painted=%u pixels=%zu nonblack=%llu rgbSum=%llu",race,prefs.m_sexID,modelId,layout,painted,body->levels[0].size(),static_cast<unsigned long long>(nonblack),static_cast<unsigned long long>(rgb));
    }
    // Native item priorities have already resolved the layers. Paint the same
    // handles into their retail rectangles instead of the global classic sheet.
    for (uint32_t region = 0; region < 8; ++region) {
        const auto& nativeSection = component.m_section[region];
        // Match native CreateRequestAU/AL/HA/TU/TL/LU/LL/FO. Cached handles
        // can outlive a cleared priority bit; they are not active overlays.
        const bool hidesPants = (component.m_flags & 0x20) != 0;
        constexpr uint32_t lastLayer[] = {2,7,1,5,7,3,6,1};
        uint32_t first = 0, last = lastLayer[region];
        if ((region == 5 || region == 6) && hidesPants) {
            first = 1; last = region == 5 ? 2 : 4;
        }
        for (uint32_t priority = first; priority < last; ++priority) {
            if (!(nativeSection.m_layerMask & (1u << priority))) continue;
            void* handle = nativeSection.m_layerTex[priority];
            struct TemporaryLayer {void* handle=nullptr;~TemporaryLayer(){if(handle)TextureCacheDestroyTexture(handle);}} temporary;
            // Classic completion destroys transient item layer handles before
            // the modern pass. Reacquire this exact actor-selected layer for
            // composition instead of silently baking a naked body.
            if(!handle&&nativeSection.m_layerItemDisplayId[priority]){
                ItemDisplayInfoRec row;
                if(!g_itemDisplayInfoDB.GetRecord(nativeSection.m_layerItemDisplayId[priority],&row))return false;
                handle=temporary.handle=component.LoadItemComponentTexture(&row,static_cast<COMPONENT_SECTIONS>(region));
                if(!handle)return false;
            }
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
        // The donor binds independent whole slots individually. One unavailable
        // eye/hair/decoration file must not discard the body and clothing atlas.
        output.whole.push_back({type,whole[type].materialResourceId});
    }
    if (appearance.attached.size() > 64) return false;
    for (const auto& attached : appearance.attached) {
        if (!attached.fileDataId) continue;
        if (attached.geoset > 65535) return false;
        auto it=std::find_if(output.children.begin(),output.children.end(),[&](const auto& group){return group.fdid==attached.fileDataId;});
        if(it==output.children.end()){output.children.emplace_back();it=output.children.end()-1;it->fdid=attached.fileDataId;}
        const auto id=uint16_t(attached.geoset);
        if(std::find(it->geosets.begin(),it->geosets.end(),id)==it->geosets.end())it->geosets.push_back(id);
    }
    WLOG_INFO("native-appearance: race=%u sex=%u model=%u layout=%u size=%ux%u sheets=%zu choices=%zu skinned-children=%zu",
        race,prefs.m_sexID,modelId,layout,output.sheets.at(1)->width,output.sheets.at(1)->height,output.sheets.size(),choices.size(),output.children.size());
    return true;
}

bool PumpWhole(AppearanceAssets& state) {
    const auto now=OsGetAsyncTimeMs();uint32_t work=0;bool changed=false;
    for(auto& group:state.whole){
        if(group.ready||group.exhausted||(group.retryAt&&int32_t(now-group.retryAt)<0)||work>=2)continue;
        ++work;
        const auto* path=ResolveTexture(ResolveMaterialTexture(group.material,group.type));
        auto sheet=WholeTexture(path);
        if(!sheet){
            group.retryAt=now+1000;
            if(++group.attempts>=3){group.exhausted=true;WLOG_WARN("native-appearance: whole slot unavailable type=%u material=%u path=%s; other slots retained",group.type,group.material,path?path:"<unresolved>");}
            continue;
        }
        if(!state.original.count(group.type)){
            CTexture* original=nullptr;
            const auto& textures=state.model->m_shared->m_data->m_textures;
            for(uint32_t i=0;i<textures.m_count;++i)if(textures[i].m_type==group.type){original=state.model->m_textures[i];break;}
            state.original.emplace(group.type,original?reinterpret_cast<CTexture*>(HandleDuplicate(reinterpret_cast<HOBJECT>(original))):nullptr);
        }
        state.sheets.emplace(group.type,std::move(sheet));group.ready=true;changed=true;
    }
    return changed;
}

void PumpChildren(CCharacterComponent& component,AppearanceAssets& state){
    const auto now=OsGetAsyncTimeMs();uint32_t work=0;
    for(auto& group:state.children){
        if(group.ready){
            if(group.child&&group.child->model->m_skinningSource!=state.model){group.child.reset();group.ready=false;group.attempts=0;}
            else continue;
        }
        if(group.exhausted||(group.retryAt&&int32_t(now-group.retryAt)<0)||work>=2)continue;
        ++work;
        auto failed=[&]{
            group.child.reset();group.retryAt=now+1000;
            if(++group.attempts>=3){group.exhausted=true;WLOG_WARN("native-appearance: child refused fdid=%u; body recipe retained",group.fdid);}
        };
        const auto* path=ResolveModel(group.fdid);
        if(!path){failed();continue;}
        if(!group.child){
            group.child=std::make_unique<SkinnedChild>();group.startedAt=now;
            group.child->model=state.model->m_scene->CreateModel(path,0);
            if(!group.child->model){failed();continue;}
        }
        auto* model=group.child->model;
        if(!model->IsLoaded(0,0)){if(uint32_t(now-group.startedAt)>10000)failed();continue;}
        if(!model->m_shared||PathKey(model->m_shared->m_fileName)!=PathKey(path)){failed();continue;}
        const auto* skin=model->m_shared->m_skinData;
        if(!skin||!skin->m_indices.m_count||!skin->m_batches.m_count){group.child.reset();group.ready=true;continue;}
        if(!model->m_skinSections){failed();continue;}
        // One FDID can own many hair/crystal/quill variants. Keep authored
        // base ID0, clear every other child section, restore only recipe IDs.
        model->SetGeometryVisible(0,65535,0);model->SetGeometryVisible(0,0,1);
        for(auto id:group.geosets)if(id)model->SetGeometryVisible(id,id,1);
        model->OptimizeVisibleGeometry();
        if(!model->SetSkinningSource(state.model,CM2Model::SkinningSourceMode::Customization)){failed();continue;}
        for(const auto& [type,sheet]:state.sheets)model->ReplaceTexture(type,sheet->texture);
        group.ready=true;
    }
}

void Bind(CCharacterComponent& component, AppearanceAssets& state) {
    wxl::native::equipment_attachments::BeginGeometry(component);
    if (component.m_flags & 4) component.GeosRenderPrep();
    // The nested native geoset post-pass may have reapplied helmet visibility.
    // Restore that owned mask before the recipe publishes its own base choices.
    wxl::native::equipment_attachments::BeginGeometry(component);
    auto* model = component.m_data.m_model;
    const auto& sections = model->m_shared->m_skinData->m_skinSections;
    std::vector<uint16_t> ids;
    std::vector<uint32_t> shown;
    ids.reserve(sections.m_count);
    shown.reserve(sections.m_count);
    for (uint32_t i = 0; i < sections.m_count; ++i) {
        ids.push_back(sections[i].m_skinSectionId);
        shown.push_back(model->m_skinSections[i]);
    }
    // Retain native equipment choices below 2001, except recipe-owned IDs.
    // Modern groups without customization ownership get one authored variant.
    atlas::SelectGeosets(ids,shown,state.ownedGeosets,state.selectedGeosets);
    // The donor's dragon None row is sentinel 1200. Native tabard defaults
    // can otherwise expose unselected panels sampling an unbound clothing slot.
    if (state.chrModel == 89)
        for (uint32_t i = 0; i < sections.m_count; ++i)
            if (ids[i] >= 1200 && ids[i] <= 1299) shown[i] = 0;
    // A selected element may survive conditional recipe resolution even when
    // the all-choice union did not include it. Donor selections still win.
    for (uint32_t i = 0; i < sections.m_count; ++i)
        if (ids[i] && std::find(state.selectedGeosets.begin(),state.selectedGeosets.end(),ids[i]) != state.selectedGeosets.end()) shown[i] = 1;
    // Current WXL explicitly owns DK shell 1701 after recipe selection.
    // Legacy GeosRenderPrep instead selects 1703 for a DK. Use this actor's
    // class, including selection/world actors, rather than a global Glue value.
    for (uint32_t i = 0; i < sections.m_count; ++i)
        if (ids[i] == 1701) shown[i] = component.m_data.m_preferences.m_classId == 6 ? 1u : 0u;
    atlas::SelectGeosetGroup(ids,shown,state.selectedGeosets,1,99);
    // Donor HeadSwap policy: absent a chosen head, use the most complete
    // authored section. Vertex count is available in the native SKIN schema.
    const bool choseHead = std::any_of(state.selectedGeosets.begin(),state.selectedGeosets.end(),
        [](uint16_t id){ return id / 100 == 32; });
    if (!choseHead) {
        uint16_t head = 0;
        uint32_t mostVertices = 0;
        for (uint32_t i = 0; i < sections.m_count; ++i) {
            if (ids[i] / 100 == 32 && sections[i].m_vertexCount > mostVertices) {
                head = ids[i];
                mostVertices = sections[i].m_vertexCount;
            }
        }
        if (head) {
            const std::array<uint16_t,1> chosenHead{head};
            atlas::SelectGeosetGroup(ids,shown,chosenHead,3200,3299);
        }
    }
    for (uint32_t i = 0; i < sections.m_count; ++i)
        if (shown[i] != model->m_skinSections[i]) model->SetGeometryVisible(ids[i],ids[i],shown[i]);
    component.m_data.m_model->OptimizeVisibleGeometry();
    for (const auto& [type,sheet] : state.sheets) component.m_data.m_model->ReplaceTexture(type,sheet->texture);
    for (const auto& child : state.children)
        if(child.ready&&child.child)for (const auto& [type,sheet] : state.sheets) child.child->model->ReplaceTexture(type,sheet->texture);
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
        if(private_appearances::Identity(component.m_data.m_preferences.m_raceID,component.m_data.m_preferences.m_sexID)){if(component.m_wxlAppearance)Restore(component);return false;}
        auto* model = component.m_data.m_model;
        if (!AppearancesEnabled() || !AssetsEnabled() || !model || !(model->m_fFlags & 1)
            || !model->m_shared || !model->m_shared->m_data || !model->m_shared->m_skinData
            || !model->m_skinSections || !model->m_textures || !RetailRace(component.m_data.m_preferences.m_raceID)
            || component.m_data.m_preferences.m_sexID > 1 || (component.m_data.m_flags & 1)) {
            if (component.m_wxlAppearance) Restore(component);
            return false;
        }
        if (component.m_request) return false; // existing worker retains its ownership
        // A context outage is transient; do not memoize it as a failed recipe.
        if (!g_theGxDevicePtr || !g_theGxDevicePtr->CapsHasContext(-1)) return false;
        // Native section priority has already selected the owning item. Load
        // its exact actor-selected component paths before composing the retail
        // atlas; an async cache miss is transient, never an immutable recipe failure.
        if (!component.ItemsLoaded(0)) return false;
        const auto signature = Signature(component);
        auto* current = component.m_wxlAppearance;
        if (current && current->model == model && current->signature == signature) {
            if(current->ready){
                const bool changed=PumpWhole(*current);
                PumpChildren(component,*current);
                if(changed||component.m_dirtySections||(component.m_flags&5))Bind(component,*current);
                return true;
            }
            if(current->buildAttempts>=3||(current->retryAt&&int32_t(OsGetAsyncTimeMs()-current->retryAt)<0))return false;
        }
        auto next = std::make_unique<AppearanceAssets>();
        next->model = model;
        next->signature = signature;
        next->buildAttempts=current&&current->model==model&&current->signature==signature?current->buildAttempts:0;
        if (!Build(component,*next)) {
            ++next->buildAttempts;next->retryAt=OsGetAsyncTimeMs()+1000;
            WLOG_WARN("native-appearance: recipe unavailable wire=%u sex=%u path=%s attempt=%u",component.m_data.m_preferences.m_raceID,component.m_data.m_preferences.m_sexID,model->m_shared->m_fileName,next->buildAttempts);
            if (current) Restore(component);
            component.m_wxlAppearance = next.release(); // bounded retry, never a permanent transient miss
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
        // Restore a previous recipe's independent whole slots before capturing
        // their real baseline for this recipe (including pending late slots).
        if(current&&current->ready&&current->model==model)
            for(const auto& [type,texture]:current->original)
                if(!next->sheets.count(type))model->ReplaceTexture(type,texture);
        PumpWhole(*next);
        PumpChildren(component,*next);
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

bool AppearanceReady(const CCharacterComponent& component) noexcept {try{if(private_appearances::Identity(component.m_data.m_preferences.m_raceID,component.m_data.m_preferences.m_sexID))return private_appearances::Ready(component);const auto* state=component.m_wxlAppearance;return state&&state->ready&&state->model==component.m_data.m_model&&state->signature==Signature(component)&&std::all_of(state->whole.begin(),state->whole.end(),[](const auto& group){return group.ready;})&&std::all_of(state->children.begin(),state->children.end(),[](const auto& group){return group.ready;});}catch(...){return false;}}
bool AppearanceChildrenPending(const CCharacterComponent& component) noexcept {
    // Preview actors are not in the in-world player replay list. Their item
    // demand shares this existing queue lifetime, even before an atlas exists.
    if(ItemGraphPending(component)||equipment_attachments::Pending(component))return true;
    const auto* state=component.m_wxlAppearance;
    return state&&state->model==component.m_data.m_model&&(!state->ready?state->buildAttempts<3:
        (std::any_of(state->whole.begin(),state->whole.end(),[](const auto& group){return !group.ready&&!group.exhausted;})||
         std::any_of(state->children.begin(),state->children.end(),[](const auto& group){return !group.ready&&!group.exhausted;})));
}
void ReleaseAppearance(CCharacterComponent& component) noexcept {
    delete component.m_wxlAppearance;
    component.m_wxlAppearance = nullptr;
}
}
