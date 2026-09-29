#ifndef WOW_SOURCE_UIUTIL_PORTRAITBUTTON_H
#define WOW_SOURCE_UIUTIL_PORTRAITBUTTON_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"

class CTexture;

struct PLAYERPORTRAIT : public TSHashObject<PLAYERPORTRAIT, CHashKeyGUID> {
    /* 0020 */ uint8_t unk_0020;
    /* 0021 */ uint8_t unk_0021[3];
    /* 0024 */ CTexture* unk_0024;
};

struct UNITPORTRAIT : public TSHashObject<UNITPORTRAIT, HASHKEY_DWORD> {
    /* 0018 */ uint8_t unk_0018;
    /* 0019 */ uint8_t unk_0019[3];
    /* 001C */ CTexture* unk_001C;
};

int32_t RTAlphaSupported();
void __fastcall UpdateAllPortraits();

void PortraitInitialize();
void UpdatePendingPortraits();

#endif
