#ifndef ENGINE_SOURCE_FRAME_CSIMPLEEDITBOX_H
#define ENGINE_SOURCE_FRAME_CSIMPLEEDITBOX_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Frame/CSimpleAnim.h"
#include "Engine/Source/Frame/CSimpleFont.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/CRect.h"

struct CSimpleFontString;
class CSimpleFontedFrame;
class CSimpleFontedFrameFont;
struct CSimpleTexture;

class CSimpleFontedFrame {
    public:
    /* 0000 */ uint32_t m_vftable;
};

class CSimpleEditBox : public CSimpleFrame {
    public:
    /* 029C */ CSimpleFontedFrame m_baseFontedFrame;
    /* 02A0 */ uint32_t m_editBoxFlags;
    /* 02A4 */ uint32_t m_dirtyBits;
    /* 02A8 */ uint32_t m_simpleEditBoxUkn4;
    /* 02AC */ CSimpleFontedFrameFont* m_font;
    /* 02B0 */ CSimpleFontString* m_fontString;
    /* 02B4 */ char* m_text;
    /* 02B8 */ uint32_t* m_textInfo;
    /* 02BC */ char* m_textHidden;
    /* 02C0 */ uint32_t m_textLength;
    /* 02C4 */ uint32_t m_textMaxLength;
    /* 02C8 */ uint32_t m_textMaxLetters;
    /* 02CC */ uint32_t m_textSize;
    /* 02D0 */ uint32_t m_visiblePosition;
    /* 02D4 */ uint32_t m_visibleLength;
    /* 02D8 */ CSimpleTexture* m_highlightTextures [3];
    /* 02E4 */ uint32_t m_highlightLeft;
    /* 02E8 */ uint32_t m_highlightRight;
    /* 02EC */ uint32_t m_highlightDrag;
    /* 02F0 */ CSimpleTexture* m_cursorTexture;
    /* 02F4 */ uint32_t m_cursorPos;
    /* 02F8 */ float m_cursorBlinkSpeed;
    /* 02FC */ float m_blinkElapsedTime;
    /* 0300 */ TSGrowableArray<uint32_t> m_visibleLines;
    /* 0310 */ uint32_t m_simpleEditBoxUkn30;
    /* 0314 */ uint32_t m_simpleEditBoxUkn31;
    /* 0318 */ uint32_t m_simpleEditBoxUkn32;
    /* 031C */ uint32_t m_simpleEditBoxUkn33;
    /* 0320 */ uint32_t m_simpleEditBoxUkn34;
    /* 0324 */ uint32_t m_simpleEditBoxUkn35;
    /* 0328 */ uint32_t m_simpleEditBoxUkn36;
    /* 032C */ uint32_t m_simpleEditBoxUkn37;
    /* 0330 */ uint32_t m_simpleEditBoxUkn38;
    /* 0334 */ uint32_t m_simpleEditBoxUkn39;
    /* 0338 */ uint32_t m_simpleEditBoxUkn40;
    /* 033C */ uint32_t m_simpleEditBoxUkn41;
    /* 0340 */ uint32_t m_simpleEditBoxUkn42;
    /* 0344 */ uint32_t m_simpleEditBoxUkn43;
    /* 0348 */ uint32_t m_simpleEditBoxUkn44;
    /* 034C */ uint32_t m_simpleEditBoxUkn45;
    /* 0350 */ uint32_t m_simpleEditBoxUkn46;
    /* 0354 */ uint32_t m_simpleEditBoxUkn47;
    /* 0358 */ uint32_t m_simpleEditBoxUkn48;
    /* 035C */ uint32_t m_simpleEditBoxUkn49;
    /* 0360 */ uint32_t m_simpleEditBoxUkn50;
    /* 0364 */ uint32_t m_simpleEditBoxUkn51;
    /* 0368 */ uint32_t m_simpleEditBoxUkn52;
    /* 036C */ uint32_t m_simpleEditBoxUkn53;
    /* 0370 */ CRect m_textInsets;
    /* 0380 */ FrameScript_Object::ScriptIx m_onEnterPressed;
    /* 0388 */ FrameScript_Object::ScriptIx m_onEscapePressed;
    /* 0390 */ FrameScript_Object::ScriptIx m_onSpacePressed;
    /* 0398 */ FrameScript_Object::ScriptIx m_onTabPressed;
    /* 03A0 */ FrameScript_Object::ScriptIx m_onTextChanged;
    /* 03A8 */ FrameScript_Object::ScriptIx m_onTextSet;
    /* 03B0 */ FrameScript_Object::ScriptIx m_onCursorChanged;
    /* 03B8 */ FrameScript_Object::ScriptIx m_onInputLanguageChanged;
    /* 03C0 */ FrameScript_Object::ScriptIx m_onEditFocusGained;
    /* 03C8 */ FrameScript_Object::ScriptIx m_onEditFocusLost;
    /* 03D0 */ FrameScript_Object::ScriptIx m_onCharComposition;

    static CDataAllocator s_allocator;
    static CSimpleEditBox* s_currentFocus;

    CSimpleEditBox(CSimpleFrame* parent);

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
};

class CSimpleFontedFrameFont : public CSimpleFont {
    public:
    /* 006C */ CSimpleFontedFrame* m_frame;
};

#endif
