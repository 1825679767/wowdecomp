#include "BattlenetLogin.h"

// STATIC: 0x00D45C98
TumorManager* g_tumorManager;

// VERIFY: 6048e810 manual
// OFFSET: 0x008C8C00
BattlenetLogin::BattlenetLogin() : unk_114C(0), unk_1188(0) {
    this->unk_10F0 = 0;
    g_tumorManager = nullptr;
    this->unk_10F4 = 0;
    this->unk_10F8 = 0;
    this->unk_120C = 4;

    this->m_realmLock.Enter();
    this->m_realms.Clear();
    this->m_realmLock.Leave();

    this->unk_1110 = 0;
    this->unk_1114 = 0;

    this->m_eventLock.Enter();
    this->m_events[0].Clear();
    this->m_events[1].Clear();
    this->m_eventLock.Leave();

    this->unk_1208 = 0;

    this->m_gameAccountLock.Enter();
    this->m_gameAccounts.Clear();
    this->m_gameAccountLock.Leave();

    this->unk_1178 = 0;

    this->m_patchInstructionLock.Enter();
    this->m_patchInstructions.Clear();
    this->m_patchInstructionLock.Leave();

    this->unk_11B8 = 0;
    this->unk_11BC = 0;
    this->unk_11C0 = 0;
    this->unk_11C4 = 0;
}
