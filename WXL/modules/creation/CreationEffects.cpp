// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
// Authored WXL choreography; native source integration, no injected ABI.
#include "WXL/CreationEffects.hpp"
#include "WXL/NativeFeatures.hpp"
#include "WXL/AlternateAppearances.hpp"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Model2/M2Shared.h"
#include "Engine/Source/Services/Texture.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "WoW/Source/Glue/CharacterCreation.h"
#include <algorithm>
#include <chrono>
#include <cstring>
#include <cctype>
#include <string>
#include <unordered_map>
#include <vector>
#ifndef WXL_ENABLE_CREATION_EFFECTS
#define WXL_ENABLE_CREATION_EFFECTS 1
#endif
namespace wxl::native::creation_effects { namespace {
constexpr uint32_t AllBones=0xffffffffu;
template<class T, size_t N> constexpr size_t Count(const T (&)[N]) { return N; }
uint32_t Now() { return uint32_t(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count()); }
CVar* enabled=nullptr;
CM2Model* actor=nullptr;
uint64_t serial=0;
uint32_t actorHandle=0, uiTick=0;
uint32_t customizationStandAt=0;
uint32_t classId=0, race=0, sex=0, start=0;
size_t nextStep=0;
bool finished=false, hidden=false;
bool customizationOpen=false;
uint32_t g_druidMorphStart=0, g_druidTranquilityStart=0;
const char* g_druidFormPath=nullptr;
uint32_t g_druidFormAnimation=0;
float savedAlpha=1.0f;
struct Emitter { bool active=false; uint32_t nextMs=0, emitted=0; bool preparing=false; };
Emitter g_blizzardEmitter,g_priestChannelEmitter,g_warlockImpEmitter,g_warlockRainEmitter;
    // Attachment IDs recovered from CGlueMgr::ApplySpellVisualKitAttachments.
    constexpr uint32_t kAttachBase = 0x13;
    constexpr uint32_t kAttachHead = 0x14;
    constexpr uint32_t kAttachChest = 0x22;
    constexpr uint32_t kAttachLeftHand = 0x15;
    constexpr uint32_t kAttachRightHand = 0x16;
    constexpr uint32_t kAttachRightWeapon = 0x18;

    struct ExtraVisualTexture
    {
        const char* path;
        uint32_t type;
    };

    struct VisualEffect
    {
        const char* path;
        uint32_t attachment;
        float scale;
        float travel;
        float travelZ;
        uint32_t travelMs;
        float offsetX;
        float offsetY;
        float offsetZ;
        const char* texturePath;
        uint32_t textureType = 2;
        const char* secondaryTexturePath = nullptr;
        uint32_t secondaryTextureType = 0;
        uint32_t modelAnimation = UINT32_MAX;
        float modelAnimationBlend = 1.0f;
        uint32_t modelAnimationLoop = 1;
        bool useAttachmentOrigin = false;
        bool persistAcrossPhases = false;
        const ExtraVisualTexture* extraTextures = nullptr;
        uint32_t extraTextureCount = 0;
        uint32_t animationHoldFromMs = 0;
        uint32_t animationHoldAtMs = 0;
        uint32_t animationReleaseAtMs = 0;
        bool useRootRelativeTarget = false;
        bool forceVisible = false;
    };


struct OwnedVisual {
 VisualEffect effect; CM2Model* model; uint32_t launch; bool configured=false, originCaptured=false;
 C3Vector origin;
};
std::string Canonical(const char* path) {
 std::string value=path?path:"";
 for(auto& ch:value){if(ch=='/')ch='\\';ch=char(std::tolower(static_cast<unsigned char>(ch)));}
 if(value.size()>4&&value.compare(value.size()-4,4,".mdx")==0)value.replace(value.size()-4,4,".m2");
 return value;
}
bool Mounted(const char* path) {
 static std::unordered_map<std::string,bool> cache;
 auto key=Canonical(path);auto it=cache.find(key);if(it!=cache.end())return it->second;
 const bool found=!key.empty()&&SFile::FileExists(key.c_str())!=0;
 if(cache.size()<192)cache.emplace(std::move(key),found);return found;
}
std::vector<OwnedVisual> visuals;
std::vector<CM2Model*> warm;
void Dispose(OwnedVisual& v) { v.model->SetEmittersEnabled(0); v.model->SetRibbonsEnabled(0); v.model->DetachFromParent(); v.model->Release(); }
void ClearVisuals(bool all=true) {
 g_blizzardEmitter.active=g_priestChannelEmitter.active=false;
 if(all)g_warlockImpEmitter.active=g_warlockRainEmitter.active=false;
 if(all){g_druidMorphStart=g_druidTranquilityStart=0;g_druidFormPath=nullptr;g_druidFormAnimation=0;}
 if(all){for(auto* model:warm)model->Release();warm.clear();}
 for(auto it=visuals.begin();it!=visuals.end();) {
  if(!all&&it->effect.persistAcrossPhases){++it;continue;}
  Dispose(*it);it=visuals.erase(it);
 }
}
bool HasActiveVisual(const char* path) { for(const auto& v:visuals)if(!std::strcmp(path,v.effect.path))return true;return false; }
void RemoveActiveVisualsByPath(const char* path) {
 for(auto it=visuals.begin();it!=visuals.end();)if(!std::strcmp(path,it->effect.path)){Dispose(*it);it=visuals.erase(it);}else ++it;
}
bool Animate(CM2Model* model,uint32_t animation,float speed=1.0f,int blend=1) {
 if(!model->IsLoaded(0,0)||!model->HasSequence(animation))return false;
 model->SetAnimating(1); model->SetBoneSequence(AllBones,animation,0,0,speed,blend,1);return true;
}
bool EnsureActiveVisualAnimation(const char* path,uint32_t animation,float speed,uint32_t blend) {
 for(auto& v:visuals)if(!std::strcmp(path,v.effect.path)) {
  if(v.configured)return true;
  v.configured=Animate(v.model,animation,speed,int(blend));return v.configured;
 }return false;
}
void RestartActiveVisualAnimation(const char* path,uint32_t animation,float speed,uint32_t blend) {
 for(auto& v:visuals)if(!std::strcmp(path,v.effect.path)){v.effect.modelAnimation=animation;v.effect.modelAnimationBlend=speed;v.effect.modelAnimationLoop=blend;v.configured=Animate(v.model,animation,speed,int(blend));}
}
void PreloadVisual(const char* path) {
 if(!actor||!actor->m_scene||warm.size()>=8||!Mounted(path))return;
 const auto canonical=Canonical(path);auto* model=actor->m_scene->CreateModel(canonical.c_str(),0);if(!model)return;
 model->m_baseAlpha=0.0f;model->m_fadeAlpha=0.0f;
 model->SetEmittersEnabled(0);model->SetRibbonsEnabled(0);model->SetAnimating(0);warm.push_back(model);
}
void BindTexture(CM2Model* model,const char* path,uint32_t type) {
 if(!path)return;
 CStatus status; auto* texture=TextureCreate(path,CGxTexFlags(),&status,0);
 if(texture){model->ReplaceTexture(type,texture);HandleClose(reinterpret_cast<HOBJECT>(texture));}
}
bool AttachVisual(const VisualEffect& effect) {
 if(!actor||!actor->m_scene||visuals.size()>=96)return false;
 if(!Mounted(effect.path)||(effect.texturePath&&!Mounted(effect.texturePath))||(effect.secondaryTexturePath&&!Mounted(effect.secondaryTexturePath)))return false;
 for(uint32_t i=0;i<effect.extraTextureCount;++i)if(!Mounted(effect.extraTextures[i].path))return false;
 if(effect.attachment!=kAttachBase&&!actor->HasAttachment(effect.attachment))return false;
 const auto canonical=Canonical(effect.path);auto* model=actor->m_scene->CreateModel(canonical.c_str(),0);
 if(!model)return false;
 model->m_fadeAlpha=0.0f; // Pending/fallback models stay invisible until exact-name validation.
 // Child transforms use the actual native attachment IDs. Base-relative effects
 // attach to the root transform so creature-specific base pivots cannot bend flights.
 auto* parent=effect.attachment==kAttachBase?actor->m_attachParent:actor;
 if(parent)model->AttachToParent(parent,effect.attachment==kAttachBase?0:effect.attachment,nullptr,effect.attachment==kAttachBase?1:0);
 if(parent&&model->m_attachParent!=parent){model->Release();return false;}
 model->m_lightingCallback=actor->m_lightingCallback;model->m_lightingArg=actor->m_lightingArg;
 BindTexture(model,effect.texturePath,effect.textureType);
 BindTexture(model,effect.secondaryTexturePath,effect.secondaryTextureType);
 for(uint32_t i=0;i<effect.extraTextureCount;++i)BindTexture(model,effect.extraTextures[i].path,effect.extraTextures[i].type);
 visuals.push_back({effect,model,Now()});return true;
}
void UpdateVisuals() {
 const auto now=Now();
 for(auto it=visuals.begin();it!=visuals.end();) {
  auto& v=*it;const auto& e=v.effect;auto* model=v.model;
  if(!model->IsLoaded(0,0)){if(now-v.launch>5000){Dispose(v);it=visuals.erase(it);continue;}++it;continue;}
  if(!model->m_shared||Canonical(model->m_shared->m_fileName)!=Canonical(e.path)){Dispose(v);it=visuals.erase(it);continue;}
  model->m_fadeAlpha=1.0f;
  if(!v.originCaptured){
   v.originCaptured=true;v.launch=now;
   v.origin=C3Vector(e.offsetX,e.offsetY,e.offsetZ);
   if(e.useAttachmentOrigin) {
    // Capture once in actor coordinates, then travel independently of hand motion.
    v.origin=actor->GetAttachmentPosition(e.attachment)*actor->GetAnimatedWorldTransform().AffineInverse();
    v.origin.x+=e.offsetX;v.origin.y+=e.offsetY;v.origin.z+=e.offsetZ;
    model->DetachFromParent();
    if(actor->m_attachParent)model->AttachToParent(actor->m_attachParent,0,nullptr,1);
   }
  }
  if(!v.configured)v.configured=Animate(model,e.modelAnimation==UINT32_MAX?0:e.modelAnimation,e.modelAnimationBlend,int(e.modelAnimationLoop));
  const auto age=now-v.launch;
  if(e.animationHoldFromMs&&age>=e.animationHoldFromMs&&(!e.animationReleaseAtMs||age<e.animationReleaseAtMs))model->SetBoneSequenceTime(AllBones,e.animationHoldAtMs);
  if(e.modelAnimationLoop&&v.configured){M2BoneSequenceInfo info{};if(model->GetBoneSequenceInfo(AllBones,info)&&info.m_finished)Animate(model,e.modelAnimation==UINT32_MAX?0:e.modelAnimation,e.modelAnimationBlend,int(e.modelAnimationLoop));}
  const float t=e.travelMs?(std::min)(1.0f,float(age)/float(e.travelMs)):0.0f;
  C3Vector position=v.origin;
  position.x+=e.travel*t;position.z+=e.travelZ*t;
  model->SetWorldTransform(position,0.0f,e.scale);
  if(e.attachment==kAttachBase||e.useAttachmentOrigin) {
   // Sibling ownership avoids inherited actor alpha during Ghost Wolf and keeps
   // trajectories independent of animated bone zero. Convert through real matrices.
   C44Matrix transform=model->m_worldTransform*actor->GetAnimatedWorldTransform();
   if(model->m_attachParent)transform*=model->m_attachParent->GetAnimatedWorldTransform().AffineInverse();
   model->SetWorldTransform(transform);
  }
  if(e.travelMs&&age>=e.travelMs){Dispose(v);it=visuals.erase(it);continue;}
  ++it;
 }
}
    void StartWotlkBlizzardEmitter() noexcept
    {
        g_blizzardEmitter.active = true;
        g_blizzardEmitter.nextMs = Now();
        g_blizzardEmitter.emitted = 0;
        
    }

    void UpdateWotlkBlizzardEmitter() noexcept
    {
        if (!g_blizzardEmitter.active) return;
        static constexpr float kOffsets[][3] = {
            {-1.60f,  0.00f, 0.0f},
            {-0.75f,  0.35f, 0.0f},
            { 0.10f, -0.20f, 0.0f},
            { 0.90f,  0.30f, 0.0f},
            { 1.65f, -0.10f, 0.0f},
        };
        constexpr uint32_t kEmissionIntervalMs = 200; // Authored WotLK rate: 5 per second.
        constexpr uint32_t kEmissionCount = 16;       // The preview channels for 3.2 seconds.

        const uint32_t now = Now();
        while (g_blizzardEmitter.active &&
               static_cast<int32_t>(now - g_blizzardEmitter.nextMs) >= 0)
        {
            const uint32_t emitted = g_blizzardEmitter.emitted;
            const auto& offset = kOffsets[emitted % Count(kOffsets)];
            const float scale = (emitted % 3 == 1) ? 1.05f : 0.92f;
            AttachVisual(VisualEffect{
                "Spells\\Blizzard_Impact_Base.mdx", kAttachBase, scale,
                0.0f, 0.0f, 0, offset[0], offset[1], offset[2]});
            ++g_blizzardEmitter.emitted;
            g_blizzardEmitter.nextMs += kEmissionIntervalMs;
            if (g_blizzardEmitter.emitted >= kEmissionCount)
            {
                g_blizzardEmitter.active = false;
                
            }
        }
    }

    void StartWotlkPriestChannelEmitter() noexcept
    {
        g_priestChannelEmitter.active = true;
        g_priestChannelEmitter.nextMs = Now();
        g_priestChannelEmitter.emitted = 0;
        
    }

    void UpdateWotlkPriestChannelEmitter() noexcept
    {
        if (!g_priestChannelEmitter.active) return;
        constexpr uint32_t kEmissionIntervalMs = 800;
        constexpr uint32_t kEmissionCount = 3;
        const uint32_t now = Now();
        while (g_priestChannelEmitter.active &&
               static_cast<int32_t>(now - g_priestChannelEmitter.nextMs) >= 0)
        {
            AttachVisual(VisualEffect{
                "Spells\\ShadowBolt_Missile.m2", kAttachBase, 1.0f,
                -7.2f, 4.3f, 1050, 0.35f, 0.0f, 1.15f,
                nullptr, 2, nullptr, 0, 0, 1.0f, 1});
            ++g_priestChannelEmitter.emitted;
            g_priestChannelEmitter.nextMs += kEmissionIntervalMs;
            if (g_priestChannelEmitter.emitted >= kEmissionCount)
            {
                g_priestChannelEmitter.active = false;
                
            }
        }
    }

    void StartWotlkWarlockImpEmitter() noexcept
    {
        g_warlockImpEmitter.active = true;
        g_warlockImpEmitter.preparing = true;
        g_warlockImpEmitter.nextMs = Now() + 500;
        g_warlockImpEmitter.emitted = 0;
        
    }

    void UpdateWotlkWarlockImpEmitter() noexcept
    {
        if (!g_warlockImpEmitter.active) return;
        constexpr uint32_t kPrepareMs = 1000;
        constexpr uint32_t kBetweenCastsMs = 1400;
        constexpr uint32_t kEmissionCount = 3;
        const uint32_t now = Now();
        while (g_warlockImpEmitter.active &&
               static_cast<int32_t>(now - g_warlockImpEmitter.nextMs) >= 0)
        {
            if (g_warlockImpEmitter.preparing)
            {
                RestartActiveVisualAnimation("Creature\\Imp\\Imp.m2", 52, 0.22f, 1);
                RemoveActiveVisualsByPath("Spells\\Fire_Precast_Hand.mdx");
                AttachVisual(VisualEffect{
                    "Spells\\Fire_Precast_Hand.mdx", kAttachBase, 0.66f,
                    0.0f, 0.0f, 0, 1.45f, 0.15f, 0.82f,
                    nullptr, 2, nullptr, 0, UINT32_MAX, 1.0f, 1, false, true});
                g_warlockImpEmitter.preparing = false;
                g_warlockImpEmitter.nextMs += kPrepareMs;
                continue;
            }

            RemoveActiveVisualsByPath("Spells\\Fire_Precast_Hand.mdx");
            RestartActiveVisualAnimation("Creature\\Imp\\Imp.m2", 53, 0.18f, 1);
            AttachVisual(VisualEffect{
                "Spells\\FireBolt_Missile_Low.m2", kAttachBase, 0.72f,
                -8.30f, 4.55f, 1050, 1.45f, 0.15f, 0.90f});
            ++g_warlockImpEmitter.emitted;
            if (g_warlockImpEmitter.emitted >= kEmissionCount)
            {
                g_warlockImpEmitter.active = false;
                
            }
            else
            {
                g_warlockImpEmitter.preparing = true;
                g_warlockImpEmitter.nextMs += kBetweenCastsMs;
            }
        }
    }

    void StartWotlkWarlockRainEmitter() noexcept
    {
        g_warlockRainEmitter.active = true;
        g_warlockRainEmitter.nextMs = Now();
        g_warlockRainEmitter.emitted = 0;
        
    }

    void UpdateWotlkWarlockRainEmitter() noexcept
    {
        if (!g_warlockRainEmitter.active) return;
        static constexpr float kOffsets[][2] = {
            {-1.35f, -0.35f}, { 0.10f, 0.45f}, { 1.25f, -0.15f},
            {-0.65f,  0.15f}, { 0.75f, 0.35f}, {-1.10f,  0.50f},
        };
        constexpr uint32_t kEmissionIntervalMs = 260;
        constexpr uint32_t kEmissionCount = 12;
        const uint32_t now = Now();
        while (g_warlockRainEmitter.active &&
               static_cast<int32_t>(now - g_warlockRainEmitter.nextMs) >= 0)
        {
            const auto& offset = kOffsets[g_warlockRainEmitter.emitted % Count(kOffsets)];
            AttachVisual(VisualEffect{
                "Spells\\RainOfFire_Missile.m2", kAttachBase, 0.88f,
                0.0f, -5.05f, 780, offset[0], offset[1], 5.25f});
            ++g_warlockRainEmitter.emitted;
            g_warlockRainEmitter.nextMs += kEmissionIntervalMs;
            if (g_warlockRainEmitter.emitted >= kEmissionCount)
            {
                g_warlockRainEmitter.active = false;
                
            }
        }
    }

    bool PlayWotlkMageVisuals(uint32_t stage) noexcept
    {
        if (stage < 1 || stage > 9)
            return false;
        const auto playFixed = [](const auto& effects) noexcept {
            bool fixedOk = true;
            for (const VisualEffect& effect : effects)
                fixedOk &= AttachVisual(effect);
            return fixedOk;
        };

        if (stage == 1)
        {
            static constexpr VisualEffect kPyroblastPrecast[] = {
                {"Spells\\Fire_Precast_Hand.mdx", kAttachLeftHand, 1.0f},
                {"Spells\\Fire_Precast_Hand.mdx", kAttachRightHand, 1.0f},
            };
            return playFixed(kPyroblastPrecast);
        }
        if (stage == 2)
        {
            static constexpr VisualEffect kPyroblastRelease[] = {
                {"Spells\\Fire_Cast_Hand.mdx", kAttachLeftHand, 1.0f},
                {"Spells\\Fire_Cast_Hand.mdx", kAttachRightHand, 1.0f},
                // SpellVisual 2253's WotLK missile. Anchor it to the model base so the casting
                // animation cannot bend its trajectory, then move it toward the upper-left.
                {"Spells\\PyroBlast_Missile.mdx", kAttachBase, 1.0f,
                 -7.2f, 4.3f, 900, 0.35f, 0.0f, 1.15f},
            };
            return playFixed(kPyroblastRelease);
        }

        if (stage == 3)
        {
            static constexpr VisualEffect kFrostboltPrecast[] = {
                {"Spells\\Ice_Precast_Low_Hand.mdx", kAttachLeftHand, 1.0f},
                {"Spells\\Ice_Precast_Low_Hand.mdx", kAttachRightHand, 1.0f},
            };
            return playFixed(kFrostboltPrecast);
        }
        if (stage == 4)
        {
            static constexpr VisualEffect kFrostboltRelease[] = {
                {"Spells\\Ice_Precast_Low_Hand.mdx", kAttachLeftHand, 1.0f},
                {"Spells\\Ice_Precast_Low_Hand.mdx", kAttachRightHand, 1.0f},
                {"Spells\\Frostbolt.mdx", kAttachBase, 1.20f,
                 -8.5f, 4.5f, 900, 0.35f, 0.0f, 1.15f},
            };
            return playFixed(kFrostboltRelease);
        }

        // The third Mage beat is a native summon. The Elemental remains beside the character
        // through Blizzard and is cleared when Arcane Explosion begins.
        if (stage == 5)
        {
            static constexpr VisualEffect kWaterElementalPrecast[] = {
                // Frostbolt visual 13 impact kit 4991 at the missile's top-left destination.
                {"Spells\\Ice_ImpactDD_Med_Chest.mdx", kAttachBase, 1.0f,
                 0.0f, 0.0f, 0, -8.15f, 0.0f, 5.65f},
                {"Spells\\Ice_Precast_Uber_Head.mdx", kAttachHead, 1.0f},
                {"Spells\\Ice_Precast_Uber_Base.mdx", kAttachBase, 1.0f},
                {"Spells\\Ice_Precast_Uber_Hand.mdx", kAttachLeftHand, 1.0f},
                {"Spells\\Ice_Precast_Uber_Hand.mdx", kAttachRightHand, 1.0f},
            };
            return playFixed(kWaterElementalPrecast);
        }
        if (stage == 6)
        {
            static constexpr VisualEffect kWaterElementalSummon[] = {
                {"Spells\\Ice_Precast_Med_Hand.mdx", kAttachLeftHand, 1.0f},
                {"Spells\\Ice_Precast_Med_Hand.mdx", kAttachRightHand, 1.0f},
                {"Spells\\WaterElemental_Impact_Base.mdx", kAttachBase, 1.0f,
                 0.0f, 0.0f, 0, 1.85f, 0.10f, 0.0f},
                {"Creature\\WaterElemental2\\WaterElemental2.m2", kAttachBase, 0.62f,
                 0.0f, 0.0f, 0, 1.85f, 0.10f, 0.0f,
                 "Creature\\WaterElemental2\\WaterElemental2_Blue.blp", 11,
                 "Creature\\WaterElemental2\\WaterElemental2_Asset_7.blp", 12},
            };
            return playFixed(kWaterElementalSummon);
        }
        if (stage == 7)
        {
            static constexpr VisualEffect kWotlkBlizzardPrecast[] = {
                {"Spells\\Ice_Precast_Low_Hand.mdx", kAttachLeftHand, 1.0f},
                {"Spells\\Ice_Precast_Med_Hand.mdx", kAttachRightHand, 1.0f},
                {"Creature\\WaterElemental2\\WaterElemental2.m2", kAttachBase, 0.62f,
                 0.0f, 0.0f, 0, 1.85f, 0.10f, 0.0f,
                 "Creature\\WaterElemental2\\WaterElemental2_Blue.blp", 11,
                 "Creature\\WaterElemental2\\WaterElemental2_Asset_7.blp", 12},
            };
            return playFixed(kWotlkBlizzardPrecast);
        }
        if (stage == 8)
        {
            // Spell 10's WotLK channel kit uses low ice effects on both hands. Its impact-area kit
            // adds Blizzard_Spawn once; persistent kit 9350 then runs character procedure 9 at five
            // Blizzard_Impact_Base emissions per second. Mirror that controller below.
            static constexpr VisualEffect kWotlkBlizzard[] = {
                {"Spells\\Ice_Precast_Low_Hand.mdx", kAttachLeftHand, 1.0f},
                {"Spells\\Ice_Precast_Low_Hand.mdx", kAttachRightHand, 1.0f},
                {"Spells\\Blizzard_Spawn.mdx", kAttachBase, 1.0f},
                {"Creature\\WaterElemental2\\WaterElemental2.m2", kAttachBase, 0.62f,
                 0.0f, 0.0f, 0, 1.85f, 0.10f, 0.0f,
                 "Creature\\WaterElemental2\\WaterElemental2_Blue.blp", 11,
                 "Creature\\WaterElemental2\\WaterElemental2_Asset_7.blp", 12},
            };
            const bool fixedOk = playFixed(kWotlkBlizzard);
            StartWotlkBlizzardEmitter();
            return fixedOk;
        }

        static constexpr VisualEffect kArcaneExplosion[] = {
            {"Spells\\ArcaneExplosion_Base.mdx", kAttachBase, 1.0f},
        };
        return playFixed(kArcaneExplosion);
    }

    bool PlayWotlkWarriorVisuals(uint32_t stage) noexcept
    {
        if (stage < 1 || stage > 4)
            return false;

        static constexpr VisualEffect kBattleShout[] = {
            {"Spells\\BattleShout_Cast_Base.mdx", kAttachBase, 1.0f},
        };
        static constexpr VisualEffect kThunderClap[] = {
            {"Spells\\ThunderClap_Cast_Base.mdx", kAttachBase, 1.0f},
        };
        static constexpr VisualEffect kSlam[] = {
            {"Spells\\Cleave_Cast_Base.mdx", kAttachBase, 1.05f},
            {"Spells\\DustCloud_Land.mdx", kAttachBase, 1.05f,
             0.0f, 0.0f, 0, 0.0f, 0.35f, 0.0f},
            {"Spells\\Slam_Impact_Chest.mdx", kAttachChest, 1.10f},
            {"Spells\\Cleave_Impact_Chest.mdx", kAttachChest, 0.90f},
        };
        static constexpr VisualEffect kBladestorm[] = {
            {"Spells\\Warrior_BladeStorm.m2", kAttachBase, 1.0f},
        };

        const VisualEffect* effects = nullptr;
        size_t count = 0;
        if (stage == 1) { effects = kBattleShout; count = Count(kBattleShout); }
        else if (stage == 2) { effects = kThunderClap; count = Count(kThunderClap); }
        else if (stage == 3) { effects = kSlam; count = Count(kSlam); }
        else { effects = kBladestorm; count = Count(kBladestorm); }

        bool ok = true;
        for (size_t i = 0; i < count; ++i) ok &= AttachVisual(effects[i]);
        return ok;
    }

    bool PlayWotlkHunterVisuals(uint32_t stage) noexcept
    {
        if (stage < 1 || stage > 7)
            return false;

        // CreatureDisplayInfo 52598: Draenor HD wolf with its Frost display variation in monster
        // skin slot 1. Recreate the pet at every stage because transitions clear the previous
        // owned visual set before attaching the next one.
        static constexpr VisualEffect kWolf = {
            "Creature\\WolfDraenor\\WolfDraenor.m2", kAttachBase, 0.62f,
            0.0f, 0.0f, 0, 1.65f, 0.05f, 0.0f,
            "Creature\\WolfDraenor\\WolfDraenorFrost.blp", 11,
        };
        static constexpr VisualEffect kTrapClosed = {
            "World\\Goober\\G_ImmolationTrap.mdx", kAttachBase, 0.72f,
            0.0f, 0.0f, 0, -0.70f, 0.70f, 0.0f,
            nullptr, 2, nullptr, 0, 147, 0.0f, 1,
        };
        static constexpr VisualEffect kTrapOpening = {
            "World\\Goober\\G_ImmolationTrap.mdx", kAttachBase, 0.72f,
            0.0f, 0.0f, 0, -0.70f, 0.70f, 0.0f,
            nullptr, 2, nullptr, 0, 148, 0.0f, 0,
        };
        static constexpr VisualEffect kTrapOpened = {
            "World\\Goober\\G_ImmolationTrap.mdx", kAttachBase, 0.72f,
            0.0f, 0.0f, 0, -0.70f, 0.70f, 0.0f,
            nullptr, 2, nullptr, 0, 0, 0.0f, 1,
        };

        bool ok = AttachVisual(kWolf);
        if (stage == 2)
            ok &= AttachVisual(kTrapClosed);
        else if (stage == 3)
            ok &= AttachVisual(kTrapOpening);
        else if (stage >= 4)
            ok &= AttachVisual(kTrapOpened);
        if (stage == 5)
        {
            // WotLK Aimed Shot has no dedicated travelling model; use its stock arrow missile.
            ok &= AttachVisual(VisualEffect{
                "Spells\\MultiShot_Missile.mdx", kAttachLeftHand, 1.25f,
                -7.2f, 3.8f, 800, 0.0f, 0.0f, 0.0f,
                nullptr, 2, nullptr, 0, UINT32_MAX, 1.0f, 1, true});
        }
        else if (stage == 6)
        {
            ok &= AttachVisual(VisualEffect{
                "Spells\\Explosive_Shot_Missile.m2", kAttachLeftHand, 1.05f,
                -8.5f, 4.6f, 850, 0.0f, 0.0f, 0.0f,
                nullptr, 2, nullptr, 0, UINT32_MAX, 1.0f, 1, true});
        }
        else if (stage == 7)
        {
            // Spell 1543 / visual 10387 / effect 2262 from the 3.3.5a DBC chain.
            ok &= AttachVisual(VisualEffect{
                "Spells\\missile_flare.mdx", kAttachBase, 1.0f,
                0.0f, 4.0f, 1800, 0.0f, 0.0f, 0.80f});
        }
        return ok;
    }

    bool PlayWotlkRogueVisuals(uint32_t stage) noexcept
    {
        if (stage < 1 || stage > 6)
            return false;

        if (stage == 1)
        {
            return AttachVisual(VisualEffect{
                "Spells\\BackStab_Cast_Base.mdx", kAttachBase, 1.0f});
        }
        if (stage == 2)
        {
            bool ok = AttachVisual(VisualEffect{
                "Spells\\Eviscerate_Cast_Hands.mdx", kAttachLeftHand, 1.0f});
            ok &= AttachVisual(VisualEffect{
                "Spells\\Eviscerate_Cast_Hands.mdx", kAttachRightHand, 1.0f});
            return ok;
        }
        if (stage == 3)
        {
            return AttachVisual(VisualEffect{
                "Spells\\Vanish_Cast_Base.mdx", kAttachBase, 1.05f});
        }
        if (stage == 4)
        {
            // The character animation itself holds StealthStand; Vanish has cleared.
            return true;
        }

        if (stage == 5)
        {
            bool ok = AttachVisual(VisualEffect{
                "Spells\\ShadowSteps_FX.m2", kAttachBase, 1.05f});
            ok &= AttachVisual(VisualEffect{
                "Spells\\BackStab_Cast_Base.mdx", kAttachBase, 1.0f});
            // Place the impact just in front of the Rogue as the implied invisible target.
            ok &= AttachVisual(VisualEffect{
                "Spells\\BackStab_Impact_Chest.mdx", kAttachBase, 1.0f,
                0.0f, 0.0f, 0, -0.45f, 0.75f, 1.05f});
            return ok;
        }

        bool ok = AttachVisual(VisualEffect{
            "Spells\\FanOfKnives_Precast.m2", kAttachBase, 1.0f});
        ok &= AttachVisual(VisualEffect{
            "Spells\\FanOfKnives.m2", kAttachBase, 1.08f});
        ok &= AttachVisual(VisualEffect{
            "Spells\\FanOfKnives_Missile.m2", kAttachBase, 1.0f});
        return ok;
    }

    bool PlayWotlkPaladinVisuals(uint32_t stage) noexcept
    {
        if (stage < 1 || stage > 6)
            return false;

        static constexpr VisualEffect kConsecration = {
            "Spells\\Consecration_Impact_Base.mdx", kAttachBase, 1.08f,
            0.0f, 0.0f, 0, 0.0f, 0.55f, 0.0f,
        };
        static constexpr VisualEffect kWings[] = {
            {"Spells\\AvengingWrath_Impact_Base.m2", kAttachBase, 1.0f},
            {"Spells\\AvengingWrath_State_Chest.m2", kAttachChest, 1.0f},
        };

        bool ok = true;
        if (stage <= 3)
            ok &= AttachVisual(kConsecration);
        if (stage >= 2 && stage <= 5)
        {
            for (const VisualEffect& wing : kWings)
                ok &= AttachVisual(wing);
        }

        if (stage == 3)
        {
            ok &= AttachVisual(VisualEffect{
                "Spells\\SealOfCrusader_Impact.m2", kAttachRightWeapon, 1.12f});
            ok &= AttachVisual(VisualEffect{
                "Spells\\Holy_ImpactDD_Med_Chest.mdx", kAttachChest, 0.90f});
        }
        else if (stage == 4)
        {
            ok &= AttachVisual(VisualEffect{
                "Spells\\Holy_Precast_High_Base.mdx", kAttachBase, 1.0f});
            ok &= AttachVisual(VisualEffect{
                "Spells\\Holy_Precast_High_Hand.mdx", kAttachLeftHand, 1.0f});
            ok &= AttachVisual(VisualEffect{
                "Spells\\Holy_Precast_High_Hand.mdx", kAttachRightHand, 1.0f});
        }
        else if (stage == 5)
        {
            ok &= AttachVisual(VisualEffect{
                "Spells\\HolyLight_Impact_Head.mdx", kAttachHead, 1.05f});
            ok &= AttachVisual(VisualEffect{
                "Spells\\Holy_ImpactDD_High_Base.mdx", kAttachBase, 0.90f});
        }
        else if (stage == 6)
        {
            ok &= AttachVisual(VisualEffect{
                "Spells\\BlessingOfKings_Base.m2", kAttachBase, 1.0f});
            ok &= AttachVisual(VisualEffect{
                "Spells\\HolyDivineShield_State_Base.mdx", kAttachBase, 1.0f});
            ok &= AttachVisual(VisualEffect{
                "Spells\\DivineShield_Low_Chest.mdx", kAttachChest, 1.0f});
        }
        return ok;
    }

    bool PlayWotlkPriestVisuals(uint32_t stage) noexcept
    {
        if (stage < 1 || stage > 7)
            return false;

        if (stage <= 3)
        {
            if (stage == 1)
            {
                PreloadVisual("Spells\\HolyDivineShield_State_Base.mdx");
                // The stock Shadow Bolt has a 667 ms Stand sequence. Warm it before
                // the finisher and explicitly loop Stand for its 1050 ms flight.
                PreloadVisual("Spells\\ShadowBolt_Missile.m2");
            }
            bool ok = AttachVisual(VisualEffect{
                "Spells\\CFX_Priest_PenanceDamage_CastHand.mdx",
                kAttachLeftHand, 1.0f});
            ok &= AttachVisual(VisualEffect{
                "Spells\\CFX_Priest_PenanceDamage_CastHand.mdx",
                kAttachRightHand, 1.0f});
            // Capture the real hand attachment on the first rendered frame, then continue along
            // the accepted fixed top-left trajectory. Alternating hands keeps all three bolts tied
            // to the cast pose without making their destination inherit later hand motion.
            // Use the exact stable base-relative trajectory accepted for Mage Pyroblast.
            // The hand effects preserve the cast origin visually without letting race-specific
            // hand bones skew the missile's path or destination.
            ok &= AttachVisual(VisualEffect{
                "Spells\\CFX_Priest_Penance_Missile.mdx", kAttachBase, 1.0f,
                -7.2f, 4.3f, 900, 0.35f, 0.0f, 1.15f});
            return ok;
        }

        // Spell 17 -> SpellVisual 784 -> state kit 847 -> effect 674. Play sequence zero
        // once, scrub it to the fully open pose while the spell is active, then release
        // it shortly before cleanup so its native closing segment runs exactly once.
        static constexpr VisualEffect kShield = {
            "Spells\\HolyDivineShield_State_Base.mdx", kAttachBase, 1.08f,
            0.0f, 0.0f, 0, 0.0f, 0.0f, 0.0f,
            nullptr, 2, nullptr, 0, 0, 1.0f, 0, false, true,
            nullptr, 0, 450, 450, 5850,
        };

        if (stage == 4)
        {
            bool ok = AttachVisual(kShield);
            ok &= AttachVisual(VisualEffect{
                "Spells\\Holy_Precast_Low_Hand.mdx", kAttachLeftHand, 1.0f});
            ok &= AttachVisual(VisualEffect{
                "Spells\\Holy_Precast_Low_Hand.mdx", kAttachRightHand, 1.0f});
            return ok;
        }

        static constexpr VisualEffect kShadowform = {
            "Spells\\CFX_Priest_Shadowform_StateBase.mdx", kAttachBase, 1.0f,
        };
        static constexpr VisualEffect kPsyfiend = {
            "Spells\\Priest_Psyfiend.m2", kAttachBase, 0.78f,
            0.0f, 0.0f, 0, 1.45f, 0.15f, 0.0f,
        };

        // Normally stage four owns this persistent visual. If its first asynchronous load was not
        // ready, retry on the next phase without duplicating an already active bubble.
        bool ok = HasActiveVisual(kShield.path) ? true : AttachVisual(kShield);
        if (HasActiveVisual(kShield.path))
            EnsureActiveVisualAnimation(kShield.path, 0, 1.0f, 0);
        ok &= AttachVisual(kShadowform);
        if (stage == 5)
        {
            ok &= AttachVisual(VisualEffect{
                "Spells\\Shadow_Form_Precast.m2", kAttachBase, 1.0f});
            return ok;
        }

        ok &= AttachVisual(kPsyfiend);
        if (stage == 6)
        {
            ok &= AttachVisual(VisualEffect{
                "Spells\\Priest_Psyfiend_Impact.m2", kAttachBase, 0.90f,
                0.0f, 0.0f, 0, 1.45f, 0.15f, 0.0f});
            ok &= AttachVisual(VisualEffect{
                "Spells\\Priest_Psyfiend_State_Base.m2", kAttachBase, 0.90f,
                0.0f, 0.0f, 0, 1.45f, 0.15f, 0.0f});
            return ok;
        }

        ok &= AttachVisual(VisualEffect{
            "Spells\\CFX_Priest_MindFlay_ChannelHand.mdx",
            kAttachLeftHand, 1.0f});
        ok &= AttachVisual(VisualEffect{
            "Spells\\CFX_Priest_MindFlay_ChannelHand.mdx",
            kAttachRightHand, 1.0f});
        ok &= AttachVisual(VisualEffect{
            "Spells\\CFX_Priest_MindFlay_ChannelHead.mdx",
            kAttachHead, 1.0f});
        // Spell 15407's WotLK target state is ManaFunnel_Impact_Chest. Keep it at the
        // same accepted top-left target as the other class missiles. The actual beam is
        // an engine ribbon between two units; do not substitute unrelated missile geometry.
        ok &= AttachVisual(VisualEffect{
            "Spells\\ManaFunnel_Impact_Chest.mdx", kAttachBase, 1.10f,
            0.0f, 0.0f, 0, -6.85f, 0.0f, 5.45f});
        StartWotlkPriestChannelEmitter();
        return ok;
    }

    bool PlayWotlkDeathKnightVisuals(uint32_t stage) noexcept
    {
        if (stage < 1 || stage > 6)
            return false;

        // The native WotLK Northrend ghoul uses CreatureDisplayInfo's monster-skin slot.
        // Keep it alive after Raise Dead so the companion does not blink between attacks.
        static constexpr VisualEffect kGhoul = {
            "Creature\\NorthrendGhoul\\NorthrendGhoul.m2", kAttachBase, 0.66f,
            0.0f, 0.0f, 0, 1.45f, 0.12f, 0.0f,
            "Creature\\NorthrendGhoul\\NorthrendGhoul01.blp", 11,
            nullptr, 0, 0, 1.0f, 1, false, true,
        };
        static constexpr VisualEffect kDeathAndDecayBase = {
            "Spells\\DeathAndDecay_Area_Base.m2", kAttachBase, 1.08f,
            0.0f, 0.0f, 0, -0.15f, 0.45f, 0.16f,
            nullptr, 2, nullptr, 0, UINT32_MAX, 1.0f, 1, false, true,
        };
        static constexpr VisualEffect kDeathAndDecayRunes = {
            "Spells\\DeathAndDecay_Area_Runes.m2", kAttachBase, 1.08f,
            0.0f, 0.0f, 0, -0.15f, 0.45f, 0.16f,
            nullptr, 2, nullptr, 0, UINT32_MAX, 1.0f, 1, false, true,
        };

        bool ok = HasActiveVisual(kGhoul.path) ? true : AttachVisual(kGhoul);
        if (stage == 1)
        {
            ok &= AttachVisual(VisualEffect{
                "Spells\\SummonGhouls.m2", kAttachBase, 0.92f,
                0.0f, 0.0f, 0, 1.45f, 0.12f, 0.0f});
            return ok;
        }

        ok &= HasActiveVisual(kDeathAndDecayBase.path)
            ? true : AttachVisual(kDeathAndDecayBase);
        ok &= HasActiveVisual(kDeathAndDecayRunes.path)
            ? true : AttachVisual(kDeathAndDecayRunes);

        if (stage == 2)
            return ok;
        if (stage == 3)
        {
            ok &= AttachVisual(VisualEffect{
                "Spells\\DeathKnight_FrostStrike.m2",
                kAttachRightWeapon, 1.08f});
            ok &= AttachVisual(VisualEffect{
                "Spells\\DeathKnight_FrostStrike_Impact.m2", kAttachBase, 1.0f,
                0.0f, 0.0f, 0, -0.50f, 0.75f, 1.00f});
            return ok;
        }
        if (stage == 4)
        {
            ok &= AttachVisual(VisualEffect{
                "Spells\\DeathKnight_BloodStrike.m2",
                kAttachRightWeapon, 1.08f});
            ok &= AttachVisual(VisualEffect{
                "Spells\\DeathKnight_BloodStrike_Impact.m2", kAttachBase, 1.0f,
                0.0f, 0.0f, 0, -0.50f, 0.75f, 1.00f});
            return ok;
        }
        if (stage == 5)
        {
            ok &= AttachVisual(VisualEffect{
                "Spells\\DeathKnight_DeathCoil_Missile.m2",
                kAttachBase, 1.05f,
                -7.2f, 4.3f, 950, 0.35f, 0.0f, 1.15f});
            return ok;
        }

        ok &= AttachVisual(VisualEffect{
            "Spells\\DeathKnight_BloodBoil.m2", kAttachChest, 1.08f});
        ok &= AttachVisual(VisualEffect{
            "Spells\\BloodLust_State_Hand.mdx", kAttachLeftHand, 0.92f});
        ok &= AttachVisual(VisualEffect{
            "Spells\\BloodLust_State_Hand.mdx", kAttachRightHand, 0.92f});
        return ok;
    }

    bool PlayWotlkShamanVisuals(uint32_t stage) noexcept
    {
        if (stage < 1 || stage > 9)
            return false;

        static constexpr VisualEffect kGhostWolf = {
            "Creature\\Wolf\\Wolf_ghost.m2", kAttachBase, 0.82f,
            0.0f, 0.0f, 0, 0.0f, 0.05f, 0.0f,
            "Creature\\Wolf\\WolfSkin_Ghost.blp", 11,
            nullptr, 0, 0, 1.0f, 1,
        };
        static constexpr VisualEffect kTotems[] = {
            {"Creature\\Spells\\EarthElementalTotem.m2", kAttachBase, 0.58f,
             0.0f, 0.0f, 0, -1.18f, -0.55f, 0.02f,
             nullptr, 2, nullptr, 0, UINT32_MAX, 1.0f, 1, false, true},
            {"Creature\\Spells\\FireElementalTotem.m2", kAttachBase, 0.58f,
             0.0f, 0.0f, 0, 1.18f, -0.55f, 0.02f,
             nullptr, 2, nullptr, 0, UINT32_MAX, 1.0f, 1, false, true},
            {"Creature\\Spells\\WaterElementalTotem.m2", kAttachBase, 0.58f,
             0.0f, 0.0f, 0, -1.18f, 0.78f, 0.02f,
             nullptr, 2, nullptr, 0, UINT32_MAX, 1.0f, 1, false, true},
            {"Creature\\Spells\\AirElementalTotem.m2", kAttachBase, 0.58f,
             0.0f, 0.0f, 0, 1.18f, 0.78f, 0.02f,
             nullptr, 2, nullptr, 0, UINT32_MAX, 1.0f, 1, false, true},
        };

        if (stage == 1)
        {
            bool ok = AttachVisual(VisualEffect{
                "Spells\\Shaman_AstralShift.m2", kAttachBase, 1.0f});
            VisualEffect ghostWolf = kGhostWolf;
            ghostWolf.forceVisible = true;
            ok &= AttachVisual(ghostWolf);
            return ok;
        }
        if (stage == 2)
            return AttachVisual(VisualEffect{
                "Spells\\Shaman_AstralShift.m2", kAttachBase, 1.0f});

        bool ok = true;
        for (const VisualEffect& totem : kTotems)
            ok &= HasActiveVisual(totem.path) ? true : AttachVisual(totem);
        if (stage == 3)
            return ok;
        if (stage == 4)
        {
            ok &= AttachVisual(VisualEffect{
                "Spells\\New_Windfury.m2", kAttachBase, 1.05f});
            ok &= AttachVisual(VisualEffect{
                "Spells\\Enchantments\\Shaman_Wind.m2",
                kAttachRightWeapon, 1.0f});
            return ok;
        }
        if (stage == 5)
        {
            ok &= AttachVisual(VisualEffect{
                "Spells\\Enchantments\\Shaman_Fire.m2",
                kAttachRightWeapon, 1.0f});
            ok &= AttachVisual(VisualEffect{
                "Spells\\FlameShock_Impact_Chest.m2", kAttachBase, 0.95f,
                0.0f, 0.0f, 0, -0.48f, 0.72f, 1.0f});
            return ok;
        }
        if (stage == 6)
        {
            ok &= AttachVisual(VisualEffect{
                "Spells\\Lightning_PreCast_Low_Hand.mdx", kAttachLeftHand, 1.0f});
            ok &= AttachVisual(VisualEffect{
                "Spells\\Lightning_PreCast_Low_Hand.mdx", kAttachRightHand, 1.0f});
            return ok;
        }
        if (stage == 7)
        {
            ok &= AttachVisual(VisualEffect{
                "Spells\\LightningBolt_Missile.m2", kAttachBase, 1.15f,
                -7.2f, 4.3f, 900, 0.35f, 0.0f, 1.15f});
            return ok;
        }
        if (stage == 8)
        {
            ok &= AttachVisual(VisualEffect{
                "Spells\\Fire_Precast_Hand.mdx", kAttachLeftHand, 1.0f});
            ok &= AttachVisual(VisualEffect{
                "Spells\\Fire_Precast_Hand.mdx", kAttachRightHand, 1.0f});
            return ok;
        }

        ok &= AttachVisual(VisualEffect{
            "Spells\\Fire_Cast_Hand.mdx", kAttachLeftHand, 1.0f});
        ok &= AttachVisual(VisualEffect{
            "Spells\\Fire_Cast_Hand.mdx", kAttachRightHand, 1.0f});
        ok &= AttachVisual(VisualEffect{
            "Spells\\Shaman_LavaBurst_Missile.m2", kAttachBase, 1.05f,
            -7.8f, 4.5f, 950, 0.35f, 0.0f, 1.15f});
        return ok;
    }

    bool PlayWotlkWarlockVisuals(uint32_t stage) noexcept
    {
        if (stage < 1 || stage > 6)
            return false;

        // The Imp is a stock 3.3.5 creature. Keep its monster-skin binding and placement stable
        // while its own cast animation and Firebolt stream run independently of the player.
        static constexpr VisualEffect kImp = {
            "Creature\\Imp\\Imp.m2", kAttachBase, 0.68f,
            0.0f, 0.0f, 0, 1.45f, 0.15f, 0.0f,
            "Creature\\Imp\\ImpSkinRed.blp", 11,
            nullptr, 0, 53, 0.18f, 1, false, true,
        };

        bool ok = HasActiveVisual(kImp.path) ? true : AttachVisual(kImp);
        if (stage == 1)
        {
            PreloadVisual("Spells\\Fel_Fire_Precast_Hand.m2");
            PreloadVisual("Spells\\ShadowBolt_Missile.m2");
            PreloadVisual("Spells\\cfx_warlock_chaosbolt_missile.m2");
            ok &= AttachVisual(VisualEffect{
                "Spells\\Shadow_Form_Precast.m2", kAttachBase, 0.88f,
                0.0f, 0.0f, 0, 1.45f, 0.15f, 0.0f});
            ok &= AttachVisual(VisualEffect{
                "Spells\\Shadow_Precast_Low_Hand.mdx", kAttachLeftHand, 1.0f});
            ok &= AttachVisual(VisualEffect{
                "Spells\\Shadow_Precast_Low_Hand.mdx", kAttachRightHand, 1.0f});
            StartWotlkWarlockImpEmitter();
            return ok;
        }
        if (stage == 2)
        {
            ok &= AttachVisual(VisualEffect{
                "Spells\\Shadow_Precast_Low_Hand.mdx", kAttachLeftHand, 0.95f});
            ok &= AttachVisual(VisualEffect{
                "Spells\\Shadow_Precast_Low_Hand.mdx", kAttachRightHand, 0.95f});
            return ok;
        }
        if (stage == 3)
        {
            // Same proven stock missile/Stand loop as Priest, with a slower flight.
            ok &= AttachVisual(VisualEffect{
                "Spells\\ShadowBolt_Missile.m2", kAttachBase, 1.0f,
                -7.2f, 4.3f, 1400, 0.35f, 0.0f, 1.15f,
                nullptr, 2, nullptr, 0, 0, 1.0f, 1});
            return ok;
        }
        if (stage == 4)
        {
            ok &= AttachVisual(VisualEffect{
                "Spells\\Fel_Fire_Precast_Hand.m2",
                kAttachLeftHand, 1.0f});
            ok &= AttachVisual(VisualEffect{
                "Spells\\Fel_Fire_Precast_Hand.m2",
                kAttachRightHand, 1.0f});
            return ok;
        }
        if (stage == 5)
        {
            ok &= AttachVisual(VisualEffect{
                "Spells\\cfx_warlock_chaosbolt_missile.m2", kAttachBase, 1.0f,
                -7.2f, 4.3f, 1400, 0.35f, 0.0f, 1.15f,
                nullptr, 2, nullptr, 0, 144, 1.0f, 1}); // Authored InFlight, not Stand/Impact.
            return ok;
        }

        // Rain of Fire ends the Imp volley, then drops the real WotLK missiles around the actor.
        g_warlockImpEmitter.active = false;
        RemoveActiveVisualsByPath("Spells\\Fire_Precast_Hand.mdx");
        ok &= AttachVisual(VisualEffect{
            "Spells\\RainOfFire_Impact_Base.m2", kAttachBase, 1.12f,
            0.0f, 0.0f, 0, 0.0f, 0.30f, 0.08f});
        ok &= AttachVisual(VisualEffect{
            "Spells\\Fire_Precast_Hand.mdx", kAttachLeftHand, 1.0f});
        ok &= AttachVisual(VisualEffect{
            "Spells\\Fire_Precast_Hand.mdx", kAttachRightHand, 1.0f});
        ok &= AttachVisual(VisualEffect{
            "Spells\\Fire_Cast_Hand.mdx", kAttachLeftHand, 0.92f});
        ok &= AttachVisual(VisualEffect{
            "Spells\\Fire_Cast_Hand.mdx", kAttachRightHand, 0.92f});
        StartWotlkWarlockRainEmitter();
        return ok;
    }


    void UpdateDruidEffectLifecycle() noexcept
    {
        const uint32_t now = Now();
        if (g_druidMorphStart && now - g_druidMorphStart >= 2800)
        {
            RemoveActiveVisualsByPath("Spells\\DruidMorph_Impact_Base.m2");
            g_druidMorphStart = 0;
        }
        if (g_druidTranquilityStart && now - g_druidTranquilityStart >= 1867)
        {
            RestartActiveVisualAnimation("Spells\\Tranquility_Area.m2", 158, 1.0f, 1);
            RestartActiveVisualAnimation("Spells\\Tranquility_Impact_Base.m2", 158, 1.0f, 1);
            g_druidTranquilityStart = 0;
        }
    }

    bool PlayWotlkDruidVisuals(uint32_t stage) noexcept
    {
        if (stage < 1 || stage > 17) return false;
        if (stage == 16)
        {
            // Begin the authored cloud before swapping the form 450 ms later.
            RemoveActiveVisualsByPath("Spells\\DruidMorph_Impact_Base.m2");
            VisualEffect morph{"Spells\\DruidMorph_Impact_Base.m2", kAttachBase, 1.1f,
                0.0f, 0.0f, 0, 0.0f, 0.05f, 0.05f};
            morph.forceVisible = true;
            morph.persistAcrossPhases = true;
            morph.modelAnimation = 0;
            morph.modelAnimationLoop = 0;
            g_druidMorphStart = Now();
            if (g_druidFormPath)
            {
                g_druidFormAnimation = 0;
                RestartActiveVisualAnimation(g_druidFormPath, 0, 1.0f, 1);
            }
            return AttachVisual(morph);
        }
        if (stage == 17)
        {
            // End the eight-second channel using the authored Decay sequences.
            g_druidTranquilityStart = 0;
            RestartActiveVisualAnimation("Spells\\Tranquility_Area.m2", 159, 1.0f, 0);
            RestartActiveVisualAnimation("Spells\\Tranquility_Impact_Base.m2", 159, 1.0f, 0);
            g_druidFormAnimation = 0;
            if (g_druidFormPath) RestartActiveVisualAnimation(g_druidFormPath, 0, 1.0f, 1);
            return g_druidFormPath != nullptr;
        }
        static const char* const forms[] = {
            "Creature\\DruidBear\\DruidBear.m2", "Creature\\DruidCat\\DruidCat.m2",
            "Creature\\DruidOwlbear\\DruidOwlBear.m2", "Creature\\Ent\\Ent.m2"
        };
        static const char* const skins[] = {
            "Creature\\DruidBear\\DruidBearSkin.blp", "Creature\\DruidCat\\DruidCatSkinBlack.blp",
            "Creature\\DruidOwlbear\\DruidOwlBearNESkin1.blp", "Creature\\Ent\\EntSkinBrown.blp"
        };
        static constexpr float scales[] = {0.85f, 0.85f, 0.75f, 0.70f};
        const uint32_t form = stage <= 3 ? 0 : stage <= 6 ? 1 : stage <= 10 ? 2 : 3;
        for (uint32_t i = 0; i < 4; ++i)
            if (i != form) RemoveActiveVisualsByPath(forms[i]);
        if (!HasActiveVisual(forms[form]))
        {
            VisualEffect body{forms[form], kAttachBase, scales[form],
                0.0f, 0.0f, 0, 0.0f, 0.05f, 0.0f, skins[form], 11};
            if (form == 2)
            {
                body.secondaryTexturePath = "Creature\\DruidOwlbear\\DruidOwlBearNESkin2.blp";
                body.secondaryTextureType = 12;
            }
            body.modelAnimation = 0;
            body.persistAcrossPhases = true;
            body.forceVisible = true;
            if (!AttachVisual(body)) return false;
        }
        // Animate the visible form itself, never just the hidden player actor.
        uint32_t animation = 0;
        if (stage == 2) animation = 176;       // DruidBearSwipe
        else if (stage == 3) animation = 55;   // BattleRoar
        else if (stage == 5) animation = 16;   // Cat swipe
        else if (stage == 6) animation = 174;  // DruidCatClaw / Mangle
        else if (stage == 8 || stage == 12 || stage == 14) animation = 52;
        else if (stage == 9) animation = 53;
        else if (stage == 10) animation = 54;
        else if (stage == 13) animation = 54;
        else if (stage == 15) animation = 124;
        g_druidFormPath = forms[form];
        g_druidFormAnimation = animation;
        RestartActiveVisualAnimation(forms[form], animation, 1.0f,
            animation == 0 || animation == 52 || animation == 124 ? 1 : 0);
        const auto effect = [](const char* path, float scale, float z) noexcept {
            VisualEffect visual{path, kAttachBase, scale,
                0.0f, 0.0f, 0, 0.0f, 0.05f, z};
            visual.forceVisible = true;
            return AttachVisual(visual);
        };
        if (stage == 1 || stage == 4 || stage == 7 || stage == 11)
            return true; // The persistent morph cloud already covers the swap.
        if (stage == 2 || stage == 5)
            return effect("Spells\\SwipeCaster.m2", 1.0f, 0.60f);
        if (stage == 3)
            return effect("Spells\\DemoralizingShout_Cast_Base.m2", 1.0f, 0.10f);
        if (stage == 6)
            return effect("Spells\\Mangle_Impact.m2", 1.0f, 0.65f);
        if (stage == 8)
            return effect("Spells\\Wrath_PreCast_Hand.m2", 1.0f, 1.20f);
        if (stage == 9)
        {
            VisualEffect missile{"Spells\\Wrath_Missile.m2", kAttachBase, 1.0f,
                -7.2f, 4.3f, 1400, 0.35f, 0.0f, 1.15f};
            missile.forceVisible = true;
            return AttachVisual(missile);
        }
        if (stage == 10)
        {
            VisualEffect moonfire{"Spells\\Moonfire_Impact_Base.m2", kAttachBase, 1.0f,
                0.0f, 0.0f, 0, -2.4f, 0.15f, 0.05f};
            moonfire.forceVisible = true;
            return AttachVisual(moonfire);
        }
        if (stage == 12 || stage == 14)
            return effect("Spells\\Nature_PreCast_Low_Hand.m2", 1.0f, 1.0f);
        if (stage == 13)
            return effect("Spells\\Rejuvenation_Impact_Base.m2", 1.15f, 0.10f);
        bool ok = true;
        for (const char* path : {"Spells\\Tranquility_Area.m2", "Spells\\Tranquility_Impact_Base.m2"})
        {
            VisualEffect tranquility{path, kAttachBase, 1.1f,
                0.0f, 0.0f, 0, 0.0f, 0.05f, 0.10f};
            tranquility.forceVisible = true;
            tranquility.modelAnimation = 0;
            tranquility.modelAnimationLoop = 0;
            ok &= AttachVisual(tranquility);
        }
        g_druidTranquilityStart = Now();
        return ok;
    }


struct Step {uint32_t at, animation, phase; int visible=0;};
constexpr uint32_t NoAnimation=UINT32_MAX;
// Times are relative to the donor's animationBase, after its 3s landing window.
constexpr Step warrior[]={{0,55,1},{1250,58,2},{2450,18,3},{3700,126,4},{5900,27,0}};
constexpr Step paladin[]={{0,33,1},{1350,54,2},{2750,18,3},{3900,51,4},{5100,53,5},{6200,54,6},{8200,27,0}};
constexpr Step hunter[]={{0,114,1},{50,114,1},{1000,115,2},{1500,NoAnimation,3},{2000,116,4},{2550,105,255},{3050,109,255},{3850,47,5},{4450,105,255},{4950,47,6},{5750,107,7},{7000,29,0}};
constexpr Step rogue[]={{0,85,1},{1300,57,2},{2600,120,3},{3000,120,4,-1},{4100,85,5,1},{5300,33,6},{6800,26,0,1}};
constexpr Step priest[]={{0,124,1},{850,NoAnimation,2},{1700,NoAnimation,3},{2650,54,4},{3850,54,5},{5000,54,6},{6200,124,7},{9400,28,0}};
constexpr Step deathKnight[]={{0,54,1},{1350,33,2},{2750,18,3},{3900,58,4},{5050,53,5},{6350,54,6},{8050,27,0}};
constexpr Step shaman[]={{0,54,1,-1},{2000,54,2,1},{2500,33,3},{4050,58,4},{5450,18,5},{6900,52,6},{7750,53,7},{8950,52,8},{9800,53,9},{11500,26,0,1}};
constexpr Step mage[]={{0,51,1},{1450,53,2},{2450,51,3},{3750,53,4},{4750,52,5},{6200,54,6},{8000,52,7},{9300,125,8},{12500,54,9},{13650,26,0}};
constexpr Step warlock[]={{0,54,1},{1250,52,2},{2350,53,3},{2800,28,255},{3850,52,4},{5350,53,5},{5800,52,255},{6850,124,6},{10400,28,0}};
constexpr Step druid[]={{0,NoAnimation,16},{450,NoAnimation,1},{1600,NoAnimation,2},{2900,NoAnimation,3},{4300,NoAnimation,16},{4750,NoAnimation,4},{5800,NoAnimation,5},{7000,NoAnimation,6},{8400,NoAnimation,16},{8850,NoAnimation,7},{9900,NoAnimation,8},{11000,NoAnimation,9},{12600,NoAnimation,10},{14800,NoAnimation,16},{15250,NoAnimation,11},{16300,NoAnimation,12},{17300,NoAnimation,13},{19000,NoAnimation,14},{20000,NoAnimation,15},{28000,NoAnimation,17},{29900,0,0,1}};
struct Timeline {const Step* data=nullptr;size_t count=0;};
template<size_t N> Timeline Line(const Step (&steps)[N]) {return {steps,N};}
Timeline GetTimeline(uint32_t id) {
 switch(id){case 1:return Line(warrior);case 2:return Line(paladin);case 3:return Line(hunter);case 4:return Line(rogue);case 5:return Line(priest);case 6:return Line(deathKnight);case 7:return Line(shaman);case 8:return Line(mage);case 9:return Line(warlock);case 11:return Line(druid);default:return {};}
}
void Visible(bool show) {
 auto* owner=CCharacterCreation::m_character;
 if(!actor||!owner||owner->m_wxlSerial!=serial||owner->m_data.m_model!=actor||actor->m_handle!=actorHandle){hidden=false;return;}
 if(!show&&!hidden){savedAlpha=actor->m_baseAlpha;actor->m_baseAlpha=0.0f;hidden=true;}
 if(show&&hidden){actor->m_baseAlpha=savedAlpha;hidden=false;}
}
int LuaCustomizationOpen(lua_State* L) {SetCustomizationOpen(lua_toboolean(L,1)!=0);return 0;}
const char CustomizationBridge[]=R"lua(
local frame = _G.CharacterCreate and _G.CharacterCreate.CustomizationFrame
if frame then
 if type(frame.PlayToggleAnim)=="function" and frame.WXLNativeEffectsWrapped~=frame.PlayToggleAnim then
  local original=frame.PlayToggleAnim
  local wrapped=function(self,...)
   -- Stop before the native/UI animation can retire or replace an actor.
   WXL_SetCreationEffectsCustomizationOpen(true)
   local result=original(self,...)
   WXL_SetCreationEffectsCustomizationOpen(self.show and true or false)
   return result
  end
  frame.PlayToggleAnim=wrapped
  frame.WXLNativeEffectsWrapped=wrapped
 end
 WXL_SetCreationEffectsCustomizationOpen(frame.show and frame:IsShown() and true or false)
else
 WXL_SetCreationEffectsCustomizationOpen(false)
end
)lua";
void PlayPhase(uint32_t phase) {
 // Tranquility Decay retains both running channel models through its exit.
 if(classId==11&&phase==17){PlayWotlkDruidVisuals(phase);return;}
 ClearVisuals(false);
 switch(classId){case 1:PlayWotlkWarriorVisuals(phase);break;case 2:PlayWotlkPaladinVisuals(phase);break;case 3:PlayWotlkHunterVisuals(phase);break;case 4:PlayWotlkRogueVisuals(phase);break;case 5:PlayWotlkPriestVisuals(phase);break;case 6:PlayWotlkDeathKnightVisuals(phase);break;case 7:PlayWotlkShamanVisuals(phase);break;case 8:PlayWotlkMageVisuals(phase);break;case 9:PlayWotlkWarlockVisuals(phase);break;case 11:PlayWotlkDruidVisuals(phase);break;}
}
}
void Register() {enabled=CVar::Register("wxlCreationEffects","WXL native character creation class choreography",2,WXL_ENABLE_CREATION_EFFECTS?"1":"0",nullptr,0,false,nullptr,false);}
bool Enabled() noexcept {return WXL_ENABLE_CREATION_EFFECTS&&NativeEnabled()&&enabled&&enabled->m_intValue;}
void InitializeLua() {StopCreation();customizationOpen=false;uiTick=0;FrameScript_RegisterFunction("WXL_SetCreationEffectsCustomizationOpen",&LuaCustomizationOpen);}
void SetCustomizationOpen(bool value) noexcept {
 if(value&&!customizationOpen){StopCreation();customizationStandAt=Now()+200;}
 if(!value)customizationStandAt=0;
 customizationOpen=value;
}
void StopCreation() noexcept {
 ClearVisuals();Visible(true);
 actor=nullptr;serial=0;actorHandle=0;start=0;nextStep=0;finished=false;classId=race=sex=0;
}
void UpdateCreation() {
 if(Now()-uiTick>=200){uiTick=Now();FrameScript_Execute(CustomizationBridge,"native-creation-effects-customization",nullptr);}
 auto* component=CCharacterCreation::m_character;
 if(Enabled()&&customizationOpen&&customizationStandAt&&int32_t(Now()-customizationStandAt)>=0){
  customizationStandAt=0;if(component&&component->m_data.m_model)Animate(component->m_data.m_model,0);
 }
 if(!Enabled()||customizationOpen||alternate_appearances::IsAlternate()||CCharacterCreation::m_existingCharacterIndex>=0||!component||!component->m_data.m_model){StopCreation();return;}
 auto* current=component->m_data.m_model;
 const auto& p=component->m_data.m_preferences;
 if(actor!=current||serial!=component->m_wxlSerial||actorHandle!=current->m_handle||classId!=uint32_t(p.m_classId)||race!=p.m_raceID||sex!=p.m_sexID){
  StopCreation();actor=current;serial=component->m_wxlSerial;actorHandle=current->m_handle;classId=p.m_classId;race=p.m_raceID;sex=p.m_sexID;
 }
 const auto timeline=GetTimeline(classId);if(!timeline.count||finished)return;
 // Alternate form presentation owns a hidden primary. Wait until it is visible.
 if(!hidden&&actor->m_baseAlpha<=0.0f){if(start){ClearVisuals();start=0;nextStep=0;}return;}
 if(!actor->IsLoaded(0,0))return;
 if(!start)start=Now();
 const auto elapsed=Now()-start;
 if(elapsed>=3000)while(nextStep<timeline.count&&elapsed-3000>=timeline.data[nextStep].at){
  const auto step=timeline.data[nextStep++];
  if(step.visible)Visible(step.visible>0);
  if(step.animation!=NoAnimation)Animate(actor,step.animation);
  if(!step.phase){ClearVisuals();Visible(true);finished=true;break;}
  if(step.phase!=255)PlayPhase(step.phase);
 }
 UpdateWotlkBlizzardEmitter();UpdateWotlkPriestChannelEmitter();UpdateWotlkWarlockImpEmitter();UpdateWotlkWarlockRainEmitter();
 if(classId==11){
  UpdateDruidEffectLifecycle();
  if(g_druidFormPath)EnsureActiveVisualAnimation(g_druidFormPath,g_druidFormAnimation,1.0f,(g_druidFormAnimation==0||g_druidFormAnimation==52||g_druidFormAnimation==124)?1:0);
 }
 UpdateVisuals();
 if(classId==11){
  bool loadedForm=false;
  if(g_druidFormPath)for(const auto& visual:visuals)
   if(!std::strcmp(visual.effect.path,g_druidFormPath)&&visual.model->IsLoaded(0,0)&&visual.model->m_shared&&Canonical(visual.model->m_shared->m_fileName)==Canonical(g_druidFormPath)&&visual.originCaptured){loadedForm=true;break;}
  Visible(!loadedForm);
 }
}
}
