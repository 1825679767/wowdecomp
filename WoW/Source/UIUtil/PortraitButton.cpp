#include "PortraitButton.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Tempest/CiRect.h"
#include "WoW/Source/Ui/ScriptEvents.h"

// STATIC: 0x00C5CDFC
static int32_t unk_C5CDFC;

// STATIC: 0x00C5CE08
static TSHashTable<PLAYERPORTRAIT, CHashKeyGUID> unk_C5CE08;

// STATIC: 0x00C5CE30
static TSHashTable<UNITPORTRAIT, HASHKEY_DWORD> unk_C5CE30;

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

// VERIFY: 406a4532 manual
// OFFSET: 0x00617070
void UpdateAllPortraits() {
    unk_C5CDFC = RTAlphaSupported();

    for (PLAYERPORTRAIT* portrait = unk_C5CE08.m_fulllist.Head(); portrait; portrait = unk_C5CE08.Next(portrait)) {
        portrait->unk_0020 = 1;
    }

    for (UNITPORTRAIT* portrait = unk_C5CE30.m_fulllist.Head(); portrait; portrait = unk_C5CE30.Next(portrait)) {
        portrait->unk_0018 = 1;
    }

    Script_SendUniqueSignalToAllUnits(145);
}
