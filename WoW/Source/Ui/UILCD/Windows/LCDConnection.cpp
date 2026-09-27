#include "WoW/Source/Ui/UILCD/Windows/LCDConnection.h"
#include <cstring>
#include "WoW/Source/Ui/UILCD/Windows/LCDGfx.h"
#include "WoW/Source/Ui/UILCD/Windows/LCDOutput.h"

// STATIC: 0x00DCE1A4
LONG CLCDConnection::g_lInitCount;

// VERIFY: 45f8899c auto
// OFFSET: 0x0095A6E0
CLCDConnection::CLCDConnection() {
    this->m_hConnection = LGLCD_INVALID_CONNECTION;
    this->m_pColorOutput = nullptr;
    this->m_pGfxColor = nullptr;
    this->m_pMonoOutput = nullptr;
    this->m_pGfxMono = nullptr;
    this->unk_0040 = 0;
    this->m_pSoftbuttonsChangedCtx = nullptr;
    InitializeCriticalSection(&this->m_csCallback);
}

// VERIFY: 57a4e5e4 manual
// OFFSET: 0x0095A730
CLCDConnection::~CLCDConnection() {
    this->Disconnect();

    if (this->m_pMonoOutput) {
        delete this->m_pMonoOutput;
        this->m_pMonoOutput = nullptr;
    }

    if (this->m_pColorOutput) {
        delete this->m_pColorOutput;
        this->m_pColorOutput = nullptr;
    }

    if (this->m_pGfxMono) {
        delete this->m_pGfxMono;
        this->m_pGfxMono = nullptr;
    }

    if (this->m_pGfxColor) {
        delete this->m_pGfxColor;
        this->m_pGfxColor = nullptr;
    }
}

// VERIFY: 139c498f manual
// OFFSET: 0x0095AAB0
int32_t CLCDConnection::InitializeConnection(lgLcdConnectContextEx& connectContext, lgLcdSoftbuttonsChangedContext* softbuttonsChangedContext) {
    if (!connectContext.dwAppletCapabilitiesSupported || (connectContext.dwAppletCapabilitiesSupported & LGLCD_APPLET_CAP_BW)) {
        this->m_pMonoOutput = this->AllocMonoOutput();
        this->m_pGfxMono = new CLCDGfxMono();
        this->m_pGfxMono->Initialize();
        this->m_pMonoOutput->SetGfx(this->m_pGfxMono);
    }

    if (connectContext.dwAppletCapabilitiesSupported & LGLCD_APPLET_CAP_QVGA) {
        this->m_pColorOutput = this->AllocColorOutput();
        this->m_pGfxColor = new CLCDGfxColor();
        this->m_pGfxColor->Initialize();
        this->m_pColorOutput->SetGfx(this->m_pGfxColor);
    }

    if (InterlockedIncrement(&CLCDConnection::g_lInitCount) == 1 && lgLcdInit() != ERROR_SUCCESS) {
        InterlockedDecrement(&CLCDConnection::g_lInitCount);
        return 0;
    }

    memset(&this->m_lcdConnectCtxEx, 0, sizeof(this->m_lcdConnectCtxEx));
    this->m_lcdConnectCtxEx.onNotify.notificationCallback = nullptr;
    this->m_lcdConnectCtxEx.appFriendlyName = nullptr;
    this->m_lcdConnectCtxEx.isPersistent = FALSE;
    this->m_lcdConnectCtxEx.isAutostartable = FALSE;
    this->m_lcdConnectCtxEx.dwAppletCapabilitiesSupported = 0;
    this->m_lcdConnectCtxEx.dwReserved1 = 0;
    this->m_lcdConnectCtxEx.onNotify.notifyContext = nullptr;
    this->m_lcdConnectCtxEx.connection = LGLCD_INVALID_CONNECTION;
    memcpy(&this->m_lcdConnectCtxEx, &connectContext, sizeof(this->m_lcdConnectCtxEx));
    this->m_pSoftbuttonsChangedCtx = softbuttonsChangedContext;

    if (!this->m_lcdConnectCtxEx.onNotify.notificationCallback) {
        this->m_lcdConnectCtxEx.onNotify.notificationCallback = CLCDConnection::_OnNotificationCallback;
        this->m_lcdConnectCtxEx.onNotify.notifyContext = this;
    }

    this->ConnectEx();

    if (this->m_hConnection != LGLCD_INVALID_CONNECTION) {
        Sleep(50);
        this->Update();
    }

    return 1;
}

// VERIFY: 632aab96 manual
// OFFSET: 0x0095A3D0
void CLCDConnection::Shutdown() {
    this->Disconnect();

    if (!InterlockedDecrement(&CLCDConnection::g_lInitCount)) {
        lgLcdDeInit();
    }

    this->m_pSoftbuttonsChangedCtx = nullptr;
}

// VERIFY: 2ca96c41 auto
// OFFSET: 0x0095A480
int32_t CLCDConnection::IsConnected() {
    return this->m_hConnection != LGLCD_INVALID_CONNECTION;
}

// VERIFY: 85b8db82 auto
// OFFSET: 0x0095A440
void CLCDConnection::Disconnect() {
    if (this->m_pColorOutput) {
        this->m_pColorOutput->Close();
    }

    if (this->m_pMonoOutput) {
        this->m_pMonoOutput->Close();
    }

    if (this->m_hConnection != LGLCD_INVALID_CONNECTION) {
        lgLcdDisconnect(this->m_hConnection);
        this->m_hConnection = LGLCD_INVALID_CONNECTION;
    }
}

// VERIFY: 5261550b manual
// OFFSET: 0x0095A6C0
CLCDOutput* CLCDConnection::AllocMonoOutput() {
    return new CLCDOutput();
}

// VERIFY: 39e8db1c manual
// OFFSET: 0x0095A6C0
CLCDOutput* CLCDConnection::AllocColorOutput() {
    return new CLCDOutput();
}
