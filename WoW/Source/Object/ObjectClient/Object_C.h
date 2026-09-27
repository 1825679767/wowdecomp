#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_OBJECT_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_OBJECT_C_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/Object.h"
#include "WoW/Source/Ui/DBCACHEHASH_NAME.h"

class C3Vector;
class C44Matrix;
class C4Quaternion;
class CClientObjCreate;
class CEffect;
class CGBag_C;
class CGObject_C;
class CGWorldFrame;
class CHashKeyGUID;
class CImVector;
class CM2Model;
class CMapEntity;
class CPassenger;
class PLAYERNAMEDESC;
class SpellRec;
class SpellVisualKitRec;
class SpellVisualRec;
enum OUT_OF_RANGE_TYPE : int32_t;
enum SPELL_VISUAL_KIT_TYPE : int32_t;

typedef uint64_t WOWGUID;

class CHashKeyGUID {
    public:
    /* 0000 */ WOWGUID m_guid;

    CHashKeyGUID();
    CHashKeyGUID(WOWGUID guid);
    bool operator==(const CHashKeyGUID& key);
};

class CGObject_C : public CGObject, public TSHashObject<CGObject_C, CHashKeyGUID> {
    public:
    /* 0038 */ TSLink<void> m_link;
    /* 0040 */ uint32_t m_disableTime;
    /* 0044 */ TSList<void> m_objMirrorLists [6];
    /* 008C */ CM2Model* m_model;
    /* 0090 */ uint32_t unk_0090;
    /* 0094 */ uint32_t unk_0094;
    /* 0098 */ float m_scale;
    /* 009C */ uint32_t unk_009C;
    /* 00A0 */ uint32_t unk_00A0;
    /* 00A4 */ uint32_t unk_00A4;
    /* 00A8 */ uint32_t m_effectList;
    /* 00AC */ uint32_t m_height;
    /* 00B0 */ PLAYERNAMEDESC* m_nameDesc;
    /* 00B4 */ CM2Model* m_worldModel;
    /* 00B8 */ CMapEntity* m_worldObject;
    /* 00BC */ uint32_t m_modelFlags;
    /* 00C0 */ uint32_t unk_00C0;
    /* 00C4 */ uint32_t unk_00C4;
    /* 00C8 */ uint32_t unk_00C8;
    /* 00CC */ uint8_t unk_00CC [4];

    virtual ~CGObject_C();
    virtual void Disable();
    virtual void Reenable(const CClientObjCreate& create);
    virtual void PostReenable(const CClientObjCreate& create);
    virtual void HandleOutOfRange(OUT_OF_RANGE_TYPE type);
    virtual void UpdateWorldObject(int32_t a2);
    virtual int32_t ShouldFadeout();
    virtual void UpdateDisplayInfo(int32_t a2);
    virtual C3Vector GetNamePosition() const;
    virtual const CGBag_C* GetBag() const;
    virtual CGBag_C* GetBag();
    virtual C3Vector GetPosition() const;
    virtual C3Vector GetRawPosition() const;
    virtual float GetFacing() const;
    virtual float GetRawFacing() const;
    virtual float GetScale() const;
    virtual WOWGUID GetTransportGUID() const;
    virtual C4Quaternion GetRotation() const;
    virtual void SetFrameOfReference(const C44Matrix* matrix);
    virtual int32_t IsQuestGiver() const;
    virtual void RefreshInteractIcon();
    virtual void UpdateInteractIcon();
    virtual void UpdateInteractIconAttach();
    virtual void UpdateInteractIconScale();
    virtual int32_t GetModelFileName(const char** name) const;
    virtual void ScaleChangeUpdate();
    virtual void ScaleChangeFinished();
    virtual void RenderTargetSelection() const;
    virtual void RenderPetTargetSelection() const;
    virtual void Render();
    virtual int32_t GetSelectionHighlightColor(CImVector* color) const;
    virtual float GetTrueScale() const;
    virtual void ModelLoaded(CM2Model* model);
    virtual int32_t ApplyAlpha(uint32_t alpha);
    virtual void PreAnimate(CGWorldFrame* frame);
    virtual int32_t Animate(float elapsed);
    virtual int32_t ShouldRender(uint32_t a1, int32_t& a2, int32_t& a3);
    virtual float GetRenderFacing() const;
    virtual void OnSpecialMountAnim();
    virtual int32_t IsSolidSelectable() const;
    virtual int32_t CanHighlight() const;
    virtual int32_t CanBeTargetted() const;
    virtual int32_t FloatingTooltip() const;
    virtual int32_t OnRightClick();
    virtual void Unk44();
    virtual int32_t IsHighlightSuppressed() const;
    virtual void OnSpellEffectClear(CEffect* effect);
    virtual int32_t GetAppropriateSpellVisual(const SpellRec* spell, SpellVisualRec& visual, int32_t a3, int32_t a4) const;
    virtual void ConnectToLightningThisFrame(uint32_t a1, int32_t a2);
    virtual C44Matrix GetMatrix() const;
    virtual void ObjectNameVisibilityChanged();
    virtual int32_t UpdateObjectNameString(int32_t a1, char* buffer, uint32_t size);
    virtual int32_t ShouldRenderObjectName(int32_t a1) const;
    virtual CM2Model* GetObjectModel() const;
    virtual const char* GetObjectName() const;
    virtual int32_t GetPageTextID(void (*callback)(int32_t, const WOWGUID&, void*, bool)) const;
    virtual void CleanUpVehicleBoneAnimsBeforeObjectModelChange();
    virtual int32_t ShouldFadeIn() const;
    virtual float GetBaseAlpha() const;
    virtual bool IsTransport() const;
    virtual int32_t IsPointInside(const C3Vector& point) const;
    virtual void AddPassenger(CPassenger* passenger, int32_t a2);
    virtual float GetSpeed() const;
    virtual void PlaySpellVisualKit_PlayAnims(const SpellRec* spell, const SpellVisualKitRec* kit, SPELL_VISUAL_KIT_TYPE type, const C3Vector* position, int32_t a5, int32_t a6, WOWGUID guid, uint32_t a8, const SpellVisualRec* visual, uint32_t a10, int32_t a11);
    virtual void PlaySpellVisualKit_HandleWeapons(const SpellRec* spell, const SpellVisualKitRec* kit, const C3Vector* position, WOWGUID guid, uint32_t a5, uint32_t a6, void (*callback)(CM2Model*, uint32_t, uint32_t, int32_t, uint32_t, WOWGUID, int32_t), int32_t* a8, uint32_t a9, int32_t a10);
    virtual void PlaySpellVisualKit_DelayLightningEffects(const SpellRec* spell, const SpellVisualKitRec* kit, SPELL_VISUAL_KIT_TYPE type, const C3Vector* position, WOWGUID guid, int32_t a6, int32_t a7, int32_t a8, int32_t a9, uint32_t a10, const SpellVisualRec* visual, uint32_t a12, int32_t a13);
};

struct DBCACHEHASH_NAME : public TSHashObject<DBCACHEHASH_NAME, CHashKeyGUID> {
    /* 0020 */ uint8_t m_record [336];
    /* 0170 */ DBCACHETRAILER_GUID m_trailer;
};

#endif
