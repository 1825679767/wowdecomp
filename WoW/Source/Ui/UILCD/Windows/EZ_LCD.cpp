#include "WoW/Source/Ui/UILCD/Windows/EZ_LCD.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Ui/UILCD/Windows/EZ_LCD_Page.h"

// VERIFY: 12b1cb5e auto
// OFFSET: 0x009591B0
DWORD WINAPI CEzLcd::OnSoftButtonCB(int device, DWORD dwButtons, const PVOID pContext) {
    static_cast<CEzLcd*>(pContext)->OnSoftButtons(dwButtons);
    return 0;
}

// VERIFY: e8967e5f manual
// OFFSET: 0x00959940
CEzLcd::CEzLcd() {
    this->unk_01D4 = -1;
    this->unk_01D8 = -1;
    this->m_output = nullptr;
    this->unk_019C = 0;
    this->unk_0198 = 0;
    this->m_colorGlobalPage = nullptr;
    this->m_monoGlobalPage = nullptr;
    this->unk_01AC = 0;
    this->unk_01A8 = 0;
    this->unk_01B4 = 0;
    this->unk_01B0 = 0;
    this->m_lcdType = 3;
    this->m_buttonsPending = 0;
    this->m_buttonsCurrent = 0;
    InitializeCriticalSection(&this->m_buttonLock);

    this->m_monoPages.SetCount(7);
    this->m_colorPages.SetCount(13);

    for (uint32_t i = 0; i < this->m_monoPages.Count(); i++) {
        this->m_monoPages[i] = nullptr;
    }

    for (uint32_t i = 0; i < this->m_colorPages.Count(); i++) {
        this->m_colorPages[i] = nullptr;
    }
}

// VERIFY: e63fd5eb manual
// OFFSET: 0x00959AE0
CEzLcd::~CEzLcd() {
    for (uint32_t i = 0; i < this->m_monoPages.Count(); i++) {
        if (this->m_monoPages[i]) {
            delete this->m_monoPages[i];
        }

        this->m_monoPages[i] = nullptr;
    }

    this->m_monoPages.Clear();

    for (uint32_t i = 0; i < this->m_colorPages.Count(); i++) {
        if (this->m_colorPages[i]) {
            delete this->m_colorPages[i];
        }

        this->m_colorPages[i] = nullptr;
    }

    this->m_monoPages.Clear();
    this->m_colorPages.Clear();

    delete this->m_colorGlobalPage;
    this->m_colorGlobalPage = nullptr;

    delete this->m_monoGlobalPage;
    this->m_monoGlobalPage = nullptr;

    this->m_connection.Shutdown();

    DeleteCriticalSection(&this->m_buttonLock);
}

// VERIFY: 6c0ec21a auto
// OFFSET: 0x009591D0
void CEzLcd::OnSoftButtons(uint32_t buttons) {
    EnterCriticalSection(&this->m_buttonLock);
    this->m_buttonsPending |= buttons;
    LeaveCriticalSection(&this->m_buttonLock);
}

// VERIFY: a5ba63e3 auto
// OFFSET: 0x009592F0
int32_t CEzLcd::Initialize(const char* friendlyName, int32_t lcdType, int32_t isAutoStartable, int32_t isPersistent, lgLcdConfigureContext* configContext, lgLcdSoftbuttonsChangedContext* softbuttonsChangedContext) {
    if (this->unk_01A8 || this->unk_01AC) {
        return E_FAIL;
    }

    SStrCopy(this->m_friendlyName, friendlyName, STORM_MAX_STR);

    this->unk_01CC = 0x80000000;
    this->unk_01D0 = 0x80000000;
    this->unk_01B8 = 1;
    this->m_initialized = 0;
    this->m_configContext = configContext;
    this->m_isPersistent = isPersistent;
    this->m_isAutoStartable = isAutoStartable;
    this->unk_019C = 0;
    this->unk_0198 = 0;
    this->unk_01B4 = 0;
    this->unk_01B0 = 0;

    lgLcdConnectContextEx connectContext;
    connectContext.appFriendlyName = this->m_friendlyName;
    connectContext.isPersistent = isPersistent;
    connectContext.isAutostartable = isAutoStartable;

    if (configContext) {
        connectContext.onConfigure = *configContext;
    } else {
        connectContext.onConfigure.configCallback = nullptr;
        connectContext.onConfigure.configContext = nullptr;
    }

    switch (lcdType) {
        case 0: {
            connectContext.dwAppletCapabilitiesSupported = LGLCD_APPLET_CAP_BW;
            break;
        }

        case 1: {
            connectContext.dwAppletCapabilitiesSupported = LGLCD_APPLET_CAP_QVGA;
            break;
        }

        case 2: {
            connectContext.dwAppletCapabilitiesSupported = LGLCD_APPLET_CAP_BW | LGLCD_APPLET_CAP_QVGA;
            break;
        }
    }

    this->m_lcdType = lcdType;
    connectContext.dwReserved1 = 0;
    connectContext.onNotify.notificationCallback = nullptr;
    connectContext.onNotify.notifyContext = nullptr;

    if (!softbuttonsChangedContext) {
        this->m_softbuttonsChangedContext.softbuttonsChangedCallback = CEzLcd::OnSoftButtonCB;
        this->m_softbuttonsChangedContext.softbuttonsChangedContext = this;
    } else {
        this->m_softbuttonsChangedContext = *softbuttonsChangedContext;
    }

    if (!this->m_connection.InitializeConnection(connectContext, &this->m_softbuttonsChangedContext)) {
        this->m_initialized = 0;
        return E_FAIL;
    }

    this->m_initialized = 1;

    if (this->m_lcdType == 1 || this->m_lcdType == 2) {
        this->m_output = this->m_connection.ColorOutput();
    }

    if (this->m_lcdType == 0 || this->m_lcdType == 2) {
        this->m_output = this->m_connection.MonoOutput();
    }

    this->unk_01B8 = 0;

    return S_OK;
}

// VERIFY: e1f25790 auto
// OFFSET: 0x00959110
void CEzLcd::Update() {
    if (this->m_initialized) {
        this->m_connection.Update();
        this->m_buttonsPrevious = this->m_buttonsCurrent;

        EnterCriticalSection(&this->m_buttonLock);
        this->m_buttonsCurrent = this->m_buttonsPending;
        this->m_buttonsPending = 0;
        LeaveCriticalSection(&this->m_buttonLock);
    }
}

// VERIFY: 23e6b3c3 manual
// OFFSET: 0x00959240
int32_t CEzLcd::IsConnected() {
    return this->m_connection.IsConnected();
}
