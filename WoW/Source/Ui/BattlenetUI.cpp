#include "BattlenetUI.h"

// STATIC: 0x00BD1B00
int32_t BattlenetUI::unk_BD1B00;

// STATIC: 0x00BD1B05
uint8_t BattlenetUI::m_friendListLoaded;

// STATIC: 0x00BD1B28
uint32_t BattlenetUI::m_numFriends;

// VERIFY: 79b13a59 auto
// OFFSET: 0x00532890
void BattlenetUI_LeaveRealm() {
    BattlenetUI::m_friendListLoaded = 0;
    BattlenetUI::m_numFriends = 0;
    BattlenetUI::unk_BD1B00 = 0;
}
