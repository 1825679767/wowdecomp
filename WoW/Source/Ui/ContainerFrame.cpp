#include "ContainerFrame.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"

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
