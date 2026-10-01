#include <cstddef>
#include <cstdint>
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/SoundEngine/SoundEngine.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ItemGroupSoundsRec.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/SoundInterface/SoundInterface2.h"
#include "WoW/Source/Ui/ChatFrame.h"

// STATIC: 0x00B4AE40
int32_t CGChat::m_paused;

// VERIFY: 88e41fa1 auto
// OFFSET: 0x004CFB50
void SI2::SetPitch(SE2SoundKitObject* object, float pitch) {
    object->m_sound.SetPitch(pitch);
}

// VERIFY: f573d0ba auto
// OFFSET: 0x004CFB70
void SI2::Detach(SE2SoundKitObject* object) {
    object->m_sound.Detach();
}

// VERIFY: f6cf06fb auto
// OFFSET: 0x004CFB80
void CGChat::LeaveWorld() {
    CGChat::m_paused++;
}

// VERIFY: 62ef2d2f auto
// OFFSET: 0x004CFB90
void CGChat::Resume() {
    CGChat::m_paused--;
}

// VERIFY: 7732767c auto
// OFFSET: 0x004CFBA0
int32_t CGChat::IsPaused() {
    return CGChat::m_paused;
}

// VERIFY: f4fa7734 manual
// OFFSET: 0x004CFC10
int32_t SI2::PlayFoleySound(int32_t materialId, const C3Vector* position, int32_t self) {
    MaterialRec* material = g_materialDB.GetRecord(materialId);

    if (!material || !material->m_foleySoundID) {
        return 0;
    }

    C3Vector soundPosition;
    soundPosition.x = position->x;
    soundPosition.y = position->y;
    soundPosition.z = position->z + 2.0f;

    if (!self) {
        return SI2::PlaySoundKit(material->m_foleySoundID, &soundPosition, nullptr, nullptr, 0, nullptr, 1, 0);
    }

    static CVar* s_listenerAtCharacter = CVar::Lookup("Sound_ListenerAtCharacter");

    int32_t listenerAtCharacter = s_listenerAtCharacter->m_intValue;

    SE2SoundKitProperties properties;
    properties.ResetToDefaults();

    if (listenerAtCharacter) {
        properties.m_volumeScale = 0.65f;
    }

    properties.m_priority = 110;

    return SI2::PlaySoundKit(material->m_foleySoundID, listenerAtCharacter == 0 ? &soundPosition : nullptr, nullptr, &properties, 0, nullptr, 1, 0);
}

// VERIFY: 9dd45739 manual
// OFFSET: 0x004CFE00
int32_t SI2::IsTooCloseToExistingSoundKit(int32_t soundKitId, const C3Vector* position) {
    SEChannelInfo info[512];
    uint32_t hardwareCount;
    uint32_t softwareCount;
    uint32_t virtualCount;
    uint32_t loadingCount;
    uint32_t otherCount;

    int32_t count = SE2::GetChannelInfo(info, 512, 1, 1, 0, 0, &hardwareCount, &softwareCount, &virtualCount, &loadingCount, &otherCount);

    for (int32_t i = 0; i < count; i++) {
        SI2SoundUserData* userData = static_cast<SI2SoundUserData*>(info[i].m_userData);

        if (userData && userData->m_soundKitId == soundKitId) {
            float dx = position->x - info[i].m_position.x;
            float dy = position->y - info[i].m_position.y;
            float dz = position->z - info[i].m_position.z;

            if (dx * dx + dz * dz + dy * dy < 6.0f) {
                return 1;
            }
        }
    }

    return 0;
}

// VERIFY: 4d8f58a6 manual
// OFFSET: 0x004D0020
int32_t SndInterfacePlayItemSound(int32_t type, CGItem_C* item) {
    if (item->m_item->m_itemFieldFlags & 0x8) {
        if (type == 1) {
            return SI2::PlaySoundKit("(DONOTRENAME)WrappedItemDrop", nullptr, nullptr, nullptr);
        }

        return SI2::PlaySoundKit("(DONOTRENAME)WrappedItemPickup", nullptr, nullptr, nullptr);
    }

    const ItemGroupSoundsRec* sounds = item->m_groupSounds;

    if (sounds) {
        return SI2::PlaySoundKit(sounds->m_sound[type], nullptr, nullptr, nullptr, 0, nullptr, 1, 0);
    }

    return 0;
}

// VERIFY: fc148494 manual
// OFFSET: 0x004D0090
void SI2::PlayItemSound(int32_t type, int32_t displayId) {
    ItemDisplayInfoRec display;

    if (!g_itemDisplayInfoDB.GetRecord(displayId, &display)) {
        return;
    }

    ItemGroupSoundsRec* sounds = g_itemGroupSoundsDB.GetRecord(display.m_groupSoundIndex);

    if (sounds) {
        SI2::PlaySoundKit(sounds->m_sound[type], nullptr, nullptr, nullptr, 0, nullptr, 1, 0);
    }
}
