#include "PlayerName.h"
#include <typeinfo>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/ConsoleBase/ConsoleCommand.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gxu/GxuFont.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Model2/M2Model.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/RaidInfo.h"

// STATIC: 0x00D380A0
uint32_t s_nameMask;

// STATIC: 0x00D380A4
static uint32_t s_lastRenderFrame;

// STATIC: 0x00D380A8
static CGxStringBatch* s_playerNameBatch;

// STATIC: 0x00D380AC
static CGxFont* s_playerNameFont;

// STATIC: 0x00D380B0
static CTexture* s_playerNameIcons;

// STATIC: 0x00D380B4
static CDataAllocator s_freePlayerNames(sizeof(PLAYERNAMEDESC), 256);

// STATIC: 0x00AF46FC
TSExplicitList<PLAYERNAMEDESC, offsetof(PLAYERNAMEDESC, m_link)> s_playerNames;

// VERIFY: e90bacd6 auto
// OFFSET: 0x007E53A0
void PlayerNameShutdown() {
    if (s_playerNameFont) {
        GxuFontDestroyFont(s_playerNameFont);
    }

    s_playerNameFont = nullptr;
    GxuFontDestroyBatch(s_playerNameBatch);
    s_playerNameBatch = nullptr;
    s_freePlayerNames.Clear(typeid(PLAYERNAMEDESC).raw_name(), SLOG_OBJECT, 1);
    ConsoleCommandUnregister("PlayerNames");

    if (s_playerNameIcons) {
        HandleClose(reinterpret_cast<HOBJECT>(s_playerNameIcons));
        s_playerNameIcons = nullptr;
    }
}

// VERIFY: c59ab0e6 manual
// OFFSET: 0x007E64D0
void PlayerNameInitialize() {
    PlayerNameShutdown();

    const char* fontName;

    if (FrameScript_GetVariable("UNIT_NAME_FONT", fontName)) {
        GxuFontCreateFont(fontName, 0.99f, s_playerNameFont, 4);
    }

    s_playerNameBatch = GxuFontCreateBatch(true, true);

    CStatus status;
    s_playerNameIcons = TextureCreate("Interface\TargetingFrame\UI-RaidTargetingIcons", CGxTexFlags(GxTex_Linear, 0, 0, 0, 0, 0, 1, 0, 0, 0), &status, 0);
}

// VERIFY: 6b580d5d auto
// OFFSET: 0x007E50F0
void PlayerNameTriggerColorUpdate(PLAYERNAMEDESC* desc) {
    if (desc) {
        desc->m_flags |= 0x2;
    }
}

// VERIFY: 6e7f45e2 manual
// OFFSET: 0x007E6390
void PlayerNameUpdateDrawCallback(PLAYERNAMEDESC* desc) {
    if (!desc) {
        return;
    }

    CGObject_C* object = ClntObjMgrObjectPtr(desc->m_guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!object) {
        return;
    }

    CM2Model* model = object->m_worldModel;

    if (!model) {
        return;
    }

    desc->m_renderFrame = s_lastRenderFrame;
    uint32_t raidTarget = GetRaidTargetIndexFromGuid(desc->m_guid);

    if (object->ShouldRenderObjectName(s_nameMask) || PLAYERNAMEDESC::ShouldRender(object, raidTarget)) {
        if (!(desc->m_flags & 0x8)) {
            desc->m_flags |= 0x8;
            object->ObjectNameVisibilityChanged();
        }

        model->m_fFlags &= ~0x20;
        model->m_drawCallback = &PLAYERNAMEDESC::RenderIfVisible;
        model->m_drawArg = desc;
    } else {
        model->m_fFlags |= 0x20;
        model->m_drawCallback = nullptr;
        model->m_drawArg = nullptr;

        if (desc->m_flags & 0x8) {
            desc->m_flags &= ~0x8;
            object->ObjectNameVisibilityChanged();
        }
    }
}

// VERIFY: 6a232d70 auto
// OFFSET: 0x007E5120
void PlayerNameUpdateEarly() {
    s_lastRenderFrame++;
}

// VERIFY: fb153d0c auto
// OFFSET: 0x007E5130
void PlayerNameTriggerNameRegenerate(PLAYERNAMEDESC* desc) {
    if (desc) {
        desc->m_flags |= 0x1;
    }
}

// VERIFY: 04249170 auto
// OFFSET: 0x007E5150
bool PlayerNameShowing(PLAYERNAMEDESC* desc) {
    return desc && (desc->m_flags & 0x8);
}
