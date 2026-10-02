#include "DanceStudio.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "WoW/Source/WowServices/WDataStore.h"

// STATIC: 0x00BEB630
uint64_t CGDanceStudio::m_learnedDanceMoves;

// STATIC: 0x00BEB638
static TSHashTable<KNOWNDANCES, HASHKEY_STRI> s_knownDances;

// VERIFY: 166fac44 auto
// OFFSET: 0x00576A90
void CGDanceStudio::InitializeGame() {
    ClientServices::SetMessageHandler(SMSG_PLAY_DANCE, Packet_SMSG_PLAY_DANCE, nullptr);
    ClientServices::SetMessageHandler(SMSG_STOP_DANCE, Packet_SMSG_STOP_DANCE, nullptr);
    ClientServices::SetMessageHandler(SMSG_NOTIFY_DANCE, Packet_SMSG_NOTIFY_DANCE, nullptr);
    ClientServices::SetMessageHandler(SMSG_LEARNED_DANCE_MOVES, Packet_SMSG_LEARNED_DANCE_MOVES, nullptr);
}

// VERIFY: 4758a0e1 auto
// OFFSET: 0x005758A0
int32_t Packet_SMSG_LEARNED_DANCE_MOVES(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    msg->Get(CGDanceStudio::m_learnedDanceMoves);
    return 1;
}

// VERIFY: 02b8c154 auto
// OFFSET: 0x00575F20
void CGDanceStudio::ShutdownGame() {
    ClientServices::ClearMessageHandler(SMSG_PLAY_DANCE);
    ClientServices::ClearMessageHandler(SMSG_STOP_DANCE);
    ClientServices::ClearMessageHandler(SMSG_NOTIFY_DANCE);
    ClientServices::ClearMessageHandler(SMSG_LEARNED_DANCE_MOVES);
    s_knownDances.InternalClear(0);
}

// VERIFY: 92fc2321 manual
// OFFSET: 0x00576100
void PlayDance(const char* name) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return;
    }

    if (player->unk_1944) {
        ConsoleWrite("WARNING! Cannot start a new dance while one is in progress", ERROR_COLOR);
        return;
    }

    WDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_PLAY_DANCE));
    KNOWNDANCES* dance = s_knownDances.Ptr(name);

    if (dance) {
        msg.Put(dance->m_id);
        msg.Put(dance->unk_001C);
        msg.Finalize();
        ClientServices::SendPacket(&msg);
    } else {
        ConsoleWrite("You haven't created a dance with that name", DEFAULT_COLOR);
    }
}

// VERIFY: 35bf2d49 manual
// OFFSET: 0x005761D0
bool CGDanceStudio::HasDance(const char* name) {
    return s_knownDances.Ptr(name) != nullptr;
}
