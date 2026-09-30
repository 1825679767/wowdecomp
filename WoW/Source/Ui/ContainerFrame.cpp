#include "ContainerFrame.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Object/ObjectClient/Bag_C.h"
#include "WoW/Source/Object/ObjectClient/Container_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/CGBankInfo.h"
#include "WoW/Source/Ui/Types.h"

// STATIC: 0x00C23540
WOWGUID CGContainerInfo::m_containers[11];

// VERIFY: 2f485abd auto
// OFFSET: 0x005D9C20
void CGContainerInfo::EnterWorld() {
    for (uint32_t offset = 0x358; offset <= 0x370; offset += sizeof(WOWGUID)) {
        ClntObjMgrSetObjMirrorHandler(ClntObjMgrGetActivePlayer(), ID_PLAYER, offset, sizeof(WOWGUID), InvUpdateHandler, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    }

    for (uint32_t offset = 0x378; offset <= 0x3F0; offset += sizeof(WOWGUID)) {
        ClntObjMgrSetObjMirrorHandler(ClntObjMgrGetActivePlayer(), ID_PLAYER, offset, sizeof(WOWGUID), UpdateInvContents, nullptr, static_cast<HANDLER_PRIORITY>(0), 1);
    }

    for (uint32_t offset = 0x4D8; offset <= 0x508; offset += sizeof(WOWGUID)) {
        ClntObjMgrSetObjMirrorHandler(ClntObjMgrGetActivePlayer(), ID_PLAYER, offset, sizeof(WOWGUID), InvUpdateHandler, nullptr, static_cast<HANDLER_PRIORITY>(0), 0);
    }

    for (uint32_t offset = 0x570; offset <= 0x668; offset += sizeof(WOWGUID)) {
        ClntObjMgrSetObjMirrorHandler(ClntObjMgrGetActivePlayer(), ID_PLAYER, offset, sizeof(WOWGUID), UpdateInvContents, nullptr, static_cast<HANDLER_PRIORITY>(0), 1);
    }

    for (uint32_t offset = 0x670; offset <= 0x768; offset += sizeof(WOWGUID)) {
        ClntObjMgrSetObjMirrorHandler(ClntObjMgrGetActivePlayer(), ID_PLAYER, offset, sizeof(WOWGUID), UpdateInvContents, nullptr, static_cast<HANDLER_PRIORITY>(0), 1);
    }

    for (uint32_t i = 0; i < sizeof(CGContainerInfo::m_containers) / sizeof(CGContainerInfo::m_containers[0]); i++) {
        CGContainerInfo::m_containers[i] = WOWGUID();
    }

    CGContainerInfo::UpdateContainers();
}

// VERIFY: bf489b49 auto
// OFFSET: 0x005D6F10
void CGContainerInfo::UpdateCooldowns() {
    FrameScript_SignalEvent(SCRIPT_EVENT_BAG_UPDATE_COOLDOWN, nullptr);
}

// VERIFY: 1e625228 manual
// OFFSET: 0x005D6F20
WOWGUID GetBagAtIndex(uint32_t index) {
    if (index <= 10 && (index < 4 || CGBankInfo::m_banker)) {
        return CGContainerInfo::m_containers[index];
    }

    return WOWGUID();
}

// VERIFY: 8542c504 manual
// OFFSET: 0x005D7380
bool CGContainerInfo::GetLuaBagAndSlot(lua_State* L, CGBag_C** bag, int32_t* slot, int32_t* bank) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return false;
    }

    *bag = nullptr;
    *bank = 0;
    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    *slot = static_cast<int32_t>(lua_tonumber(L, 2)) - 1;

    if (index >= 0) {
        if (index < 11) {
            CGContainer_C* container = static_cast<CGContainer_C*>(ClntObjMgrObjectPtr(GetBagAtIndex(index), TYPEMASK_CONTAINER, __FILE__, __LINE__));

            if (container) {
                *bag = container->GetBag();
            }

            if (index >= 4) {
                *bank = 1;
            }
        }
    } else {
        *bag = &player->m_bag;

        switch (index) {
            case -5:
                return false;

            case -3:
                *slot += 86;

                if (*slot > 117) {
                    return false;
                }

                break;

            case -2:
                *slot += 39;

                if (*slot > 66) {
                    return false;
                }

                *bank = 1;
                break;

            case -1:
                *slot += 23;

                if (*slot > 38) {
                    return false;
                }

                break;
        }
    }

    if (!*bag) {
        return false;
    }

    return *slot >= 0 && *slot < (*bag)->m_numSlots;
}
