#include "WorldText.h"
#include <typeinfo>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gxu/GxuFont.h"
#include "Engine/Source/Services/TextBlock.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Game/GameClient/WTOBJECT.h"

// STATIC: 0x00D380C8
static CGxStringBatch* s_worldTextBatch;

// STATIC: 0x00D380CC
static HTEXTFONT s_worldTextFont;

// STATIC: 0x00D380D0
static CTexture* s_pvpRankTextures[15];

// STATIC: 0x00D3810C
static CDataAllocator s_freeStrings(sizeof(WORLDTEXTSTRING), 30);

// STATIC: 0x00AF4708
static const char* s_pvpRankTextureNames[15] = { "Interface\PvPRankBadges\PvPRank01.blp", "Interface\PvPRankBadges\PvPRank02.blp", "Interface\PvPRankBadges\PvPRank03.blp", "Interface\PvPRankBadges\PvPRank04.blp", "Interface\PvPRankBadges\PvPRank05.blp", "Interface\PvPRankBadges\PvPRank06.blp", "Interface\PvPRankBadges\PvPRank07.blp", "Interface\PvPRankBadges\PvPRank08.blp", "Interface\PvPRankBadges\PvPRank09.blp", "Interface\PvPRankBadges\PvPRank10.blp", "Interface\PvPRankBadges\PvPRank11.blp", "Interface\PvPRankBadges\PvPRank12.blp", "Interface\PvPRankBadges\PvPRank13.blp", "Interface\PvPRankBadges\PvPRank14.blp", "Interface\PvPRankBadges\PvPRank15.blp" };

// VERIFY: 9b0c9bfd auto
// OFFSET: 0x007E6A30
void WorldTextShutdown() {
    if (s_worldTextFont) {
        HandleClose(reinterpret_cast<HOBJECT>(s_worldTextFont));
    }

    s_freeStrings.Clear(typeid(WORLDTEXTSTRING).raw_name(), SLOG_OBJECT, 1);
    GxuFontDestroyBatch(s_worldTextBatch);

    for (uint32_t i = 0; i < 15; i++) {
        if (s_pvpRankTextures[i]) {
            HandleClose(reinterpret_cast<HOBJECT>(s_pvpRankTextures[i]));
            s_pvpRankTextures[i] = nullptr;
        }
    }
}

// VERIFY: 816c5db7 manual
// OFFSET: 0x007E7BB0
void WorldTextInitialize() {
    const char* fontName = FrameScript_GetText("DAMAGE_TEXT_FONT", -1, GENDER_NOT_APPLICABLE);
    s_worldTextFont = TextBlockGenerateFont(fontName, 0, DDCToNDCHeight(0.018333333f));
    s_worldTextBatch = GxuFontCreateBatch(false, false);

    CStatus status;

    for (uint32_t i = 0; i < 15; i++) {
        s_pvpRankTextures[i] = TextureCreate(s_pvpRankTextureNames[i], CGxTexFlags(GxTex_Linear, 0, 0, 0, 0, 0, 1, 0, 0, 0), &status, 0);
    }
}
