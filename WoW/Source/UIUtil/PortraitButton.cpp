#include "PortraitButton.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Tempest/CiRect.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Services/Texture.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/Types.h"
#include "Engine/Source/Model2/M2Model.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"

// STATIC: 0x00C5CDF0
static CTexture* s_portraitMaskTexture;

// STATIC: 0x00C5CDF4
static CGxTex* s_portraitMaskGxTex;

// STATIC: 0x00C5CDF8
static CTexture* s_portraitDepthTexture;

// STATIC: 0x00C5CDFC
static int32_t unk_C5CDFC;

// STATIC: 0x00C5CE00
static uint32_t unk_C5CE00;

// STATIC: 0x00C5CE04
static uint32_t unk_C5CE04;

// STATIC: 0x00C5CE08
static TSHashTable<PLAYERPORTRAIT, CHashKeyGUID> unk_C5CE08;

// STATIC: 0x00C5CE30
static TSHashTable<UNITPORTRAIT, HASHKEY_DWORD> unk_C5CE30;

// STATIC: 0x00AD28E4
static TSList<PORTRAITREQUEST, TSGetLink<PORTRAITREQUEST>> s_pendingPortraits;

// STATIC: 0x00AD28F0
static TSList<PORTRAITREQUEST, TSGetLink<PORTRAITREQUEST>> s_freePortraits;

static void PortraitDepthTextureCallback(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
}

// VERIFY: 9cb5c23b auto
// OFFSET: 0x00616DC0
int32_t RTAlphaSupported() {
    if (g_theGxDevicePtr->DeviceApi() == GxApi_OpenGl && !g_theGxDevicePtr->Caps().m_oglRtAlpha) {
        return 0;
    }

    int32_t supported = 0;
    GxSceneClear(3, CImVector());

    CiRect rect;
    rect.minY = 0;
    rect.minX = 0;
    rect.maxY = 64;
    rect.maxX = 64;

    TSGrowableArray<CImVector> pixels;
    g_theGxDevicePtr->DeviceReadPixels(rect, pixels);

    for (uint32_t i = 0; i < pixels.Count(); i++) {
        if (pixels[i].a != 0xFF) {
            supported = 1;
            break;
        }
    }

    return supported;
}

// VERIFY: 4a08269a manual
// OFFSET: 0x00617070
void __fastcall UpdateAllPortraits() {
    unk_C5CDFC = RTAlphaSupported();

    for (PLAYERPORTRAIT* portrait = unk_C5CE08.m_fulllist.Head(); portrait; portrait = unk_C5CE08.Next(portrait)) {
        portrait->unk_0020 = 1;
    }

    for (UNITPORTRAIT* portrait = unk_C5CE30.m_fulllist.Head(); portrait; portrait = unk_C5CE30.Next(portrait)) {
        portrait->unk_0018 = 1;
    }

    Script_SendUniqueSignalToAllUnits(SCRIPT_EVENT_UNIT_PORTRAIT_UPDATE);
}

// VERIFY: 237bf2e5 manual
// OFFSET: 0x00618010
void PortraitInitialize() {
    CStatus status;

    s_portraitMaskTexture = TextureCreate("Interface\\CharacterFrame\\TempPortraitAlphaMaskSmall.blp", CGxTexFlags(GxTex_Linear, 0, 0, 0, 0, 0, 1), &status, 0);

    if (s_portraitMaskTexture) {
        s_portraitMaskGxTex = TextureGetGxTex(s_portraitMaskTexture, 1, nullptr);
    } else {
        s_portraitMaskGxTex = nullptr;
    }

    unk_C5CDFC = RTAlphaSupported();

    if (unk_C5CDFC && (g_theGxDevicePtr->DeviceApi() == GxApi_OpenGl || g_theGxDevicePtr->DeviceApi() == GxApi_GLL)) {
        CGxTexFlags flags(GxTex_Linear, 0, 0, 0, 0, 0, 1);
        flags.m_renderTarget = 1;
        s_portraitDepthTexture = TextureCreate(GxTex_2d, 64, 64, 24, GxTex_D24X8, GxTex_D24X8, flags, nullptr, PortraitDepthTextureCallback, "PortraitDepth", 1);
    }

    unk_C5CE00 = 0;
    unk_C5CE04 = 0;
}

// VERIFY: 59c574b9 auto
// OFFSET: 0x00616B40
static int32_t IsPortraitDrawable(CGUnit_C* unit) {
    if (unit->m_clientFlags & 0x400000) {
        return 0;
    }

    if (unit->m_characterComponent && (!unit->m_characterComponent->RenderPrep(0) || (unit->m_characterComponent->m_flags & 4))) {
        return 0;
    }

    if (!unit->m_worldModel) {
        return 0;
    }

    return unit->m_worldModel->IsDrawable(0, 1);
}

// VERIFY: 66b97025 manual
// OFFSET: 0x00616E80
void UpdatePendingPortraits() {
    if (!g_theGxDevicePtr || !g_theGxDevicePtr->CapsIsWindowVisible(-1) || !g_theGxDevicePtr || !g_theGxDevicePtr->CapsHasContext(-1)) {
        return;
    }

    PORTRAITREQUEST* next;

    for (PORTRAITREQUEST* request = s_pendingPortraits.Head(); !s_pendingPortraits.IsEnd(request); request = next) {
        next = s_pendingPortraits.RawNext(request);
        WOWGUID guid = request->m_guid;
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (!unit) {
            s_freePortraits.LinkToTail(request);
            continue;
        }

        if ((unit->m_clientFlags & 0x400000) && !unit->InitializeComponent()) {
            continue;
        }

        if (!IsPortraitDrawable(unit)) {
            continue;
        }

        uint32_t flags = request->m_flags;
        s_freePortraits.LinkToTail(request);

        if (flags & 2) {
            Script_SendUnitSignal(guid, SCRIPT_EVENT_UNIT_PORTRAIT_UPDATE);
        }

        if (flags & 1) {
            Script_SendUnitSignal(guid, SCRIPT_EVENT_UNIT_MODEL_CHANGED);
        }
    }
}

// VERIFY: 90c0e005 auto
// OFFSET: 0x00618110
void UpdateModelForUI(const WOWGUID& guid, uint32_t flags) {
    if (flags & 2) {
        PLAYERPORTRAIT* portrait = unk_C5CE08.Ptr(static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid));

        if (portrait) {
            portrait->unk_0020 = 1;
        }
    }

    if (flags & 2) {
        Script_SendUniqueSignal(guid, SCRIPT_EVENT_UNIT_PORTRAIT_UPDATE);
    }

    if (flags & 1) {
        Script_SendUniqueSignal(guid, SCRIPT_EVENT_UNIT_MODEL_CHANGED);
    }
}
