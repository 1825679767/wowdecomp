#include "BattlenetUI.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/WowServices/BattlenetLogin.h"
#include "WoW/Source/WowServices/Login.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00BD1B00
int32_t BattlenetUI::unk_BD1B00;

// STATIC: 0x00BD1B05
uint8_t BattlenetUI::m_friendListLoaded;

// STATIC: 0x00BD1B24
TSGrowableArray<BattlenetUIFriend> BattlenetUI::m_friends;

// VERIFY: 02cceac3 auto
// OFFSET: 0x00532890
void BattlenetUI_LeaveRealm() {
    BattlenetUI::m_friendListLoaded = 0;
    BattlenetUI::m_friends.m_count = 0;
    BattlenetUI::unk_BD1B00 = 0;
}

// VERIFY: f8478297 auto
// OFFSET: 0x005342D0
bool BattlenetUI_IsAvailable() {
    Login* login = ClientServices::LoginConnection();
    return login && login->GetLoginServerType() == 1 && !login->IsTrialAccount() && BattlenetAPI::IsRIDEnabled() && static_cast<BattlenetLogin*>(login)->IsConnected();
}

// VERIFY: ac1af170 auto
// OFFSET: 0x0053B370
void BattlenetUI_EnterRealm() {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled() || !static_cast<BattlenetLogin*>(login)->IsConnected()) {
        return;
    }

    BattlenetUI::PopulateAutoComplete();

    const char* nameFormat = FrameScript_GetText("BATTLENET_NAME_FORMAT", -1, GENDER_NOT_APPLICABLE);

    if (*nameFormat) {
        BattlenetUI::EnterRealm(nameFormat);
    }
}
