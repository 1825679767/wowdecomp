#include "BattlenetLogin.h"
#include "Engine/Source/Storm/H/storm.h"

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

// VERIFY: 191ce575 auto
// OFFSET: 0x008C52D0
LOGIN_STATE BattlenetLogin::NextSecurityState(LOGIN_STATE state) {
    return static_cast<LOGIN_STATE>(14);
}

// VERIFY: 9df286c6 auto
// OFFSET: 0x008C5E20
const uint8_t* BattlenetLogin::GetKeyMap() {
    return &this->unk_105D[1];
}

// VERIFY: 9fe999b5 manual
// OFFSET: 0x008C5B40
bool BattlenetLogin::IsTrialAccount() {
    return (this->unk_11B8 & 0x8) != 0;
}

// VERIFY: 56874f0c manual
// OFFSET: 0x008C5B60
bool BattlenetLogin::IsProPassAccount() {
    return (this->unk_11B8 & 0x800000) != 0;
}

// VERIFY: 20b15a2f manual
// OFFSET: 0x008C5B80
bool BattlenetLogin::IsGMAccount() {
    return (this->unk_11B8 & 0x1) != 0;
}

// VERIFY: 781acb9e auto
// OFFSET: 0x008C5E30
bool BattlenetLogin::GetMatrixInfo(uint32_t* width, uint32_t* height, uint32_t* minDigits, uint32_t* maxDigits, bool* flipCoords, uint32_t* challengeCount) {
    if (!width || !height || !minDigits || !maxDigits || !challengeCount || !flipCoords) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    if (!this->m_matrixEnabled) {
        return false;
    }

    *width = this->m_matrixWidth;
    *height = this->m_matrixHeight;
    *minDigits = this->m_matrixMinDigitCount;
    *maxDigits = this->m_matrixMaxDigitCount;
    *flipCoords = this->m_matrixFlipCoords;
    *challengeCount = this->m_matrixChallengeCount;
    return true;
}

// VERIFY: d0ecbbe1 auto
// OFFSET: 0x008C5ED0
bool BattlenetLogin::GetMatrixCoordinates(uint32_t index, uint32_t* x, uint32_t* y) {
    if (!x || !y) {
        SErrDisplayAppFatalMessage("", "", 0);
        return false;
    }

    if (!this->m_matrixEnabled || !this->m_matrixData || index >= this->m_matrixChallengeCount) {
        return false;
    }

    uint32_t coordinate = this->m_matrixData[index];
    *x = coordinate % this->m_matrixWidth;
    *y = coordinate / this->m_matrixWidth;

    if (*y >= this->m_matrixHeight) {
        SErrDisplayAppFatalMessage("", "", 0);
        return false;
    }

    return true;
}

// VERIFY: 5782eea7 auto
// OFFSET: 0x008C6AF0
uint32_t BattlenetLogin::GetNumGameAccounts() {
    return this->m_gameAccounts.m_count;
}

// VERIFY: e9ea1376 manual
// OFFSET: 0x008C6B00
const char* BattlenetLogin::GetGameAccountName(uint32_t index) {
    if (index < this->m_gameAccounts.m_count) {
        return this->m_gameAccounts.m_data[index].m_name;
    }

    return nullptr;
}

// VERIFY: d642e343 manual
// OFFSET: 0x008C6B60
uint32_t BattlenetLogin::GetGameAccountRegion(uint32_t index) {
    if (index < this->m_gameAccounts.m_count) {
        return this->m_gameAccounts.m_data[index].m_region;
    }

    return 0;
}
