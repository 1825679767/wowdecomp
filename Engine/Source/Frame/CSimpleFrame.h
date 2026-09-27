#ifndef ENGINE_SOURCE_FRAME_CSIMPLEFRAME_H
#define ENGINE_SOURCE_FRAME_CSIMPLEFRAME_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Frame/CSimpleAnim.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/CRect.h"

class CBackdropGenerator;
class CRenderBatch;
class CSimpleFrame;
struct CSimpleRegion;
struct CSimpleTop;
struct SIMPLEFRAMENODE;
struct lua_State;

class CSimpleFrame : public CScriptRegion {
    public:
    /* 00A0 */ CSimpleTop* m_simpleTop;
    /* 00A4 */ CSimpleFrame* unk_00A4;
    /* 00A8 */ CScriptRegion* m_titleRegion;
    /* 00AC */ uint32_t unk_00AC;
    /* 00B0 */ uint32_t m_id;
    /* 00B4 */ uint32_t m_frameFlags;
    /* 00B8 */ float m_frameScale;
    /* 00BC */ uint8_t m_alpha;
    /* 00BD */ uint8_t m_uknByte2;
    /* 00BE */ uint8_t m_uknByte3;
    /* 00BF */ uint8_t m_uknByte4;
    /* 00C0 */ uint32_t m_simpleFrameUkn1;
    /* 00C4 */ float m_depth;
    /* 00C8 */ float unk_00C8;
    /* 00CC */ uint32_t m_ignoreDepth;
    /* 00D0 */ uint32_t m_strata;
    /* 00D4 */ uint32_t m_level;
    /* 00D8 */ uint32_t m_enabledEventsBitmask;
    /* 00DC */ uint32_t m_isShown;
    /* 00E0 */ uint32_t m_isVisible;
    /* 00E4 */ CRect m_hitRect;
    /* 00F4 */ CRect m_hitRectInsets;
    /* 0104 */ CRect m_clampRectInsets;
    /* 0114 */ uint32_t m_highlightLocked;
    /* 0118 */ uint32_t m_uknMouseDragMaybe;
    /* 011C */ uint32_t m_isMouseDown;
    /* 0120 */ uint32_t m_isMouseDragging;
    /* 0124 */ uint32_t m_mouseDragButton;
    /* 0128 */ C2Vector m_mousePosition;
    /* 0130 */ uint32_t m_isLoading;
    /* 0134 */ FrameScript_Object::ScriptIx m_onLoad;
    /* 013C */ FrameScript_Object::ScriptIx m_onSizeChanged;
    /* 0144 */ FrameScript_Object::ScriptIx m_onUpdate;
    /* 014C */ FrameScript_Object::ScriptIx m_onShow;
    /* 0154 */ FrameScript_Object::ScriptIx m_onHide;
    /* 015C */ FrameScript_Object::ScriptIx m_onCursorEnter;
    /* 0164 */ FrameScript_Object::ScriptIx m_onCursorExit;
    /* 016C */ FrameScript_Object::ScriptIx m_onMouseDown;
    /* 0174 */ FrameScript_Object::ScriptIx m_onMouseUp;
    /* 017C */ FrameScript_Object::ScriptIx m_onMouseWheel;
    /* 0184 */ FrameScript_Object::ScriptIx m_onDragStart;
    /* 018C */ FrameScript_Object::ScriptIx m_onDragStop;
    /* 0194 */ FrameScript_Object::ScriptIx m_onReceiveDrag;
    /* 019C */ FrameScript_Object::ScriptIx m_onChar;
    /* 01A4 */ FrameScript_Object::ScriptIx m_onKeyDown;
    /* 01AC */ FrameScript_Object::ScriptIx m_onKeyUp;
    /* 01B4 */ FrameScript_Object::ScriptIx m_onAttributeChanged;
    /* 01BC */ FrameScript_Object::ScriptIx m_onEnable;
    /* 01C4 */ FrameScript_Object::ScriptIx m_onDisable;
    /* 01CC */ uint32_t unk_01CC [10];
    /* 01F4 */ uint32_t m_drawLayerEnabled [5];
    /* 0208 */ CBackdropGenerator* m_backdropGenerator;
    /* 020C */ TSList<CSimpleRegion> m_regions;
    /* 0218 */ TSList<CSimpleRegion> m_drawLayers [5];
    /* 0254 */ uint32_t m_batchDirty;
    /* 0258 */ CRenderBatch* m_renderBatch [5];
    /* 026C */ TSList<CRenderBatch> m_renderList;
    /* 0278 */ TSList<SIMPLEFRAMENODE> m_children;
    /* 0284 */ TSLink<CSimpleFrame> m_framesLink;
    /* 028C */ TSLink<CSimpleFrame> m_destroyedLink;
    /* 0294 */ TSLink<CSimpleFrame> m_strataLink;

    static CDataAllocator s_allocator;

    CSimpleFrame(CSimpleFrame* parent);
    void AllowAttributeChanges(int32_t allow);
    int32_t AttributeChangesAllowed();
    void DisableDrawLayer(uint32_t drawLayer);
    void EnableDrawLayer(uint32_t drawLayer);
    void GetHitRectInsets(float& left, float& right, float& top, float& bottom);
    void NotifyDrawLayerChanged(uint32_t drawLayer);
    void Raise();
    void SetClampRectInsets(float left, float right, float top, float bottom);
    void SetFrameFlag(uint32_t flag, bool set);
    virtual void PreLoadXML(XMLNode* node, CStatus* status);
    virtual int32_t GetScriptTime(lua_State* L, double* time, uint32_t* count, int32_t a4);
    virtual void PostLoadXML(XMLNode* node, CStatus* status);

    static int32_t s_metatable;
    static int32_t s_objectType;
    static const char* s_objectTypeName;

    static int32_t GetObjectType();
    static void CreateScriptMetaTable();
    static void DestroyScriptMetaTable();
    static void RegisterScriptMethods(lua_State* L);

    virtual int32_t GetScriptMetaTable();
    virtual bool IsA(int32_t type);
    virtual bool IsA(const char* typeName);
    virtual const char* GetObjectTypeName();
    virtual bool IsDragging();
};

#endif
