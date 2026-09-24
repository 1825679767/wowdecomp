#ifndef WOW_COMMON_COMPONENTCORE_CHARACTERCOMPONENT_H
#define WOW_COMMON_COMPONENTCORE_CHARACTERCOMPONENT_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Common/Types.h"
#include "WoW/Source/Glue/ComponentData.h"
#include "_Triage/C2iVector.h"

class CM2Model;
class CTexture;

struct CharacterBaseVariation {
    /* 0000 */ CTexture* m_texture [3];
};

struct CharacterSection {
    /* 0000 */ CTexture* m_layerTex [7];
    /* 001C */ uint32_t m_layerItemDisplayId [7];
    /* 0038 */ uint32_t m_layerMask;
};

class CCharacterComponent {
    public:
    /* 0000 */ TSLink<void> m_link;
    /* 0008 */ uint32_t m_flags;
    /* 000C */ uint32_t m_dirtySections;
    /* 0010 */ uint32_t m_heapIndex;
    /* 0014 */ EGxTexFormat m_gxTexFormat;
    /* 0018 */ ComponentData m_data;
    /* 0190 */ CTexture* m_baseSkinTexture;
    /* 0194 */ CharacterBaseVariation m_baseVariation [5];
    /* 01D0 */ CharacterSection m_section [10];
    /* 0428 */ uint32_t m_itemDisplayID [12];
    /* 0458 */ uint32_t m_handItemDisplayID [3];
    /* 0464 */ uint32_t m_itemSlotForAttachSlot [50];
    /* 052C */ void* m_request;
};

struct SomeCharacterData {
    /* 0000 */ uint32_t m_raceId;
    /* 0004 */ uint32_t m_genderId;
    /* 0008 */ uint32_t m_classId;
    /* 000C */ uint32_t m_hairColorId;
    /* 0010 */ uint32_t m_skinId;
    /* 0014 */ uint32_t m_faceId;
    /* 0018 */ uint32_t m_facialHairId;
    /* 001C */ uint32_t m_hairStyleId;
    /* 0020 */ CM2Model* m_model;
    /* 0024 */ uint32_t unk_0024;
    /* 0028 */ uint32_t unk_0028;
    /* 002C */ uint32_t unk_002C;
    /* 0030 */ uint32_t unk_0030;
    /* 0034 */ uint32_t unk_0034;
    /* 0038 */ uint32_t unk_0038;
    /* 003C */ uint32_t unk_003C;
    /* 0040 */ uint32_t unk_0040;
    /* 0044 */ uint32_t unk_0044;
    /* 0048 */ uint32_t unk_0048;
    /* 004C */ uint32_t unk_004C [56];
    /* 012C */ uint32_t m_charHairGeoset;
    /* 0130 */ uint32_t unk_0130 [18];
};

class CharacterComponent {
    public:
    /* 0000 */ uint8_t unk_0000 [8];
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint32_t m_hasTexturePointerMaybe;
    /* 0010 */ uint8_t unk_0010 [4];
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ SomeCharacterData m_data;
    /* 0190 */ uint32_t m_characterBaseSkin;
    /* 0194 */ uint32_t m_texturePointers [15];
    /* 01D0 */ uint32_t m_renderInfo [15];
    /* 020C */ uint32_t m_renderInfo2 [135];
    /* 0428 */ uint32_t unk_0428 [15];
    /* 0464 */ char m_char194;
    /* 0465 */ uint8_t unk_0465[3];
    /* 0468 */ uint32_t unk_0468 [20];
    /* 04B8 */ char m_char1D0;
    /* 04B9 */ uint8_t unk_04B9[3];
    /* 04BC */ uint32_t m_uknData1 [48];
    /* 057C */ uint32_t m_uknFlag;
};

struct CompSectionInfo {
    /* 0000 */ C2iVector m_pos;
    /* 0008 */ C2iVector m_size;
};

#endif
