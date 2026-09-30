#include "Cursor.h"
#include <cstring>
#include "Engine/Source/BLPFile/Types.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00AD2808
static const char* s_cursorNames[54] = {
    nullptr,
    "Point",
    "Cast",
    "Buy",
    "Attack",
    "Interact",
    "Speak",
    "Inspect",
    "Pickup",
    "Taxi",
    "Trainer",
    "Mine",
    "Skin",
    "GatherHerbs",
    "PickLock",
    "Mail",
    "LootAll",
    "Repair",
    "RepairNPC",
    "Item",
    "SkinHorde",
    "SkinAlliance",
    "Innkeeper",
    "Quest",
    "QuestRepeatable",
    "QuestTurnIn",
    "vehichleCursor",
    "UnablePoint",
    "UnableCast",
    "UnableBuy",
    "UnableAttack",
    "UnableInteract",
    "UnableSpeak",
    "UnableInspect",
    "UnablePickup",
    "UnableTaxi",
    "UnableTrainer",
    "UnableMine",
    "UnableSkin",
    "UnableGatherHerbs",
    "UnablePickLock",
    "UnableMail",
    "UnableLootAll",
    "UnableRepair",
    "UnableRepairNPC",
    "UnableItem",
    "UnableSkinHorde",
    "UnableSkinAlliance",
    "UnableInnkeeper",
    "UnableQuest",
    "UnableQuestRepeatable",
    "UnableQuestTurnIn",
    "UnablevehichleCursor",
    nullptr,
};

// STATIC: 0x00C25CD0
static char s_cursorFile[260];

// STATIC: 0x00C25DD4
static uint32_t s_cursorItemMipsHeight;

// STATIC: 0x00C25DD8
static uint32_t s_cursorItemMipsWidth;

// STATIC: 0x00C25DDC
static MipBits* s_cursorItemMips;

// STATIC: 0x00C25DE0
static uint32_t s_cursorItemType;

// STATIC: 0x00C25DE4
static uint32_t s_cursorResetMode;

// STATIC: 0x00C25DE8
static uint32_t s_cursorItem[1024];

// STATIC: 0x00C26DE8
static uint32_t s_cursorMode;

// STATIC: 0x00C26DF0
static uint32_t s_cursorImages[54][1024];

// VERIFY: dd7c01bd auto
// OFFSET: 0x006167E0
void CursorDropItem() {
    if (s_cursorItemType) {
        s_cursorItemType = 0;
        UpdateCursor();
    }
}

// VERIFY: be139389 auto
// OFFSET: 0x00616260
uint32_t CursorGetResetMode() {
    return s_cursorResetMode;
}

// VERIFY: 2ca86d65 auto
// OFFSET: 0x00616270
void CursorSetResetMode(uint32_t mode) {
    s_cursorResetMode = mode;
}

// VERIFY: 510ad1f1 manual
// OFFSET: 0x006160B0
int32_t CopyCursorImage(uint32_t* image, MipBits* mipImages, uint32_t width, uint32_t height) {
    if (mipImages) {
        if (width == 32) {
            if (height == 32) {
                memcpy(image, mipImages->m_mip[0], 4096);
                return 1;
            }
        } else if (width == 64 && height == 64) {
            const uint32_t* source = reinterpret_cast<const uint32_t*>(mipImages->m_mip[0]);

            for (uint32_t* row = image; row < image + 1024; row += 32) {
                const uint32_t* top = source;
                const uint32_t* bottom = source + 64;

                for (uint32_t* pixel = row; pixel < row + 32; pixel++) {
                    uint32_t redBlue = ((top[0] & 0xFF00FF) + (top[1] & 0xFF00FF) + (bottom[0] & 0xFF00FF) + (bottom[1] & 0xFF00FF) + 0x20002) >> 2;
                    uint32_t green = ((top[0] >> 2) & 0x3FC0) + ((top[1] >> 2) & 0x3FC0) + ((bottom[0] >> 2) & 0x3FC0) + ((bottom[1] >> 2) & 0x3FC0) + 0x80;
                    *pixel = (redBlue & 0xFFFF00FF) | (green & 0xFF00) | 0xFF000000;

                    top += 2;
                    bottom += 2;
                }

                source += 128;
            }

            return 1;
        }
    }

    memset(image, 0, 4096);
    return 0;
}

// VERIFY: ffd92c97 manual
// OFFSET: 0x006162C0
void UpdateCursor() {
    uint32_t* pixels = g_theGxDevicePtr->CursorLock();

    if (s_cursorItemType) {
        if (s_cursorItemMips) {
            memcpy(pixels, s_cursorImages[19], 4096);
        } else {
            for (uint32_t i = 0; i < 1024; i++) {
                uint32_t pixel = s_cursorImages[19][i];

                if (!(pixel & 0xFF000000)) {
                    pixel = s_cursorItem[i];
                }

                pixels[i] = pixel;
            }
        }
    } else {
        memcpy(pixels, s_cursorImages[s_cursorMode], 4096);
    }

    g_theGxDevicePtr->CursorUnlock(0, 0);
}

// VERIFY: 448b2353 auto
// OFFSET: 0x006163B0
void CursorInitialize() {
    s_cursorMode = 1;
    s_cursorResetMode = 1;
    s_cursorItemType = 0;
    s_cursorItemMips = nullptr;
    s_cursorItemMipsWidth = 0;
    s_cursorItemMipsHeight = 0;

    for (uint32_t i = 0; i < 54; i++) {
        MipBits* images = nullptr;
        uint32_t width;
        uint32_t height;
        PIXEL_FORMAT format;

        if (s_cursorNames[i]) {
            char fileName[260];
            SStrPrintf(fileName, sizeof(fileName), "Interface\\Cursor\\%s.blp", s_cursorNames[i]);

            width = 0;
            height = 0;
            format = PIXEL_ARGB8888;
            images = TextureLoadImage(fileName, &width, &height, &format, nullptr, nullptr, nullptr, 0);
        }

        if (images) {
            CopyCursorImage(s_cursorImages[i], images, width, height);
            TextureFreeMippedImg(images, format, width, height);
        } else {
            memset(s_cursorImages[i], 0, sizeof(s_cursorImages[i]));
        }
    }

    s_cursorFile[0] = '\0';
    UpdateCursor();
}
