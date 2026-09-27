#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "ConsoleDevice.h"
#include "ConsoleDetect.h"
#include "Engine/Source/Base/CmdLine.h"
#include "Engine/Source/ConsoleBase/ConsoleCommand.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/Types.h"
#include "Engine/Source/Gx/CGxFormat.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Os/Gui.h"
#include "Engine/Source/Os/Input.h"
#include "Engine/Source/Os/System.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Storm/H/storm.h"
#include "_Triage/GxRestart.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/Hardware.h"

// STATIC: 0x00CABA98
static CVar* s_cvGxStereoEnabled;

// STATIC: 0x00CABA9C
static CVar* s_cvGxRefresh;

// STATIC: 0x00CABAA0
static CVar* s_cvGxMaximize;

// STATIC: 0x00CABAA4
static CVar* s_cvGxMultisample;

// STATIC: 0x00CABAA8
static CVar* s_cvGxCursor;

// STATIC: 0x00CABAAC
static CVar* s_cvGxStereoSeparation;

// STATIC: 0x00CABAB0
static CVar* s_cvGxMultisampleQuality;

// STATIC: 0x00CABAB4
static CVar* s_cvGxResolution;

// STATIC: 0x00CABAB8
static CVar* s_cvHwDetect;

// STATIC: 0x00CABABC
static CVar* s_cvGxOverride;

// STATIC: 0x00CABAC0
static CVar* s_cvGxFixLag;

// STATIC: 0x00CABAC4
static CVar* s_cvMaxFPS;

// STATIC: 0x00CABAC8
static int32_t s_consoleGxOverrideSet[GxOverrides_Last];

// STATIC: 0x00CABAEC
static CVar* s_cvGxVSync;

// STATIC: 0x00CABAF0
static DefaultSettings s_defaults;

// STATIC: 0x00CABB30
static CVar* s_cvVideoOptionsVersion;

// STATIC: 0x00CABB34
static CVar* s_cvGxStereoConvergence;

// STATIC: 0x00CABB38
static Hardware s_hardware;

// STATIC: 0x00CABB6C
static CVar* s_cvMaxFPSBk;

// STATIC: 0x00CABB70
static CVar* s_cvGxTripleBuffer;

// STATIC: 0x00CABB74
static CVar* s_cvGxDepthBits;

// STATIC: 0x00CABB78
static CVar* s_cvGxColorBits;

// STATIC: 0x00CABB7C
static uint32_t s_consoleGxOverrideVal[GxOverrides_Last];

// STATIC: 0x00CABBA0
static CVar* s_cvGxApi;

// STATIC: 0x00CABBA4
static CVar* s_cvGxAspect;

// STATIC: 0x00CABBA8
static CVar* s_cvFixedFunction;

// STATIC: 0x00CABBAC
static CVar* s_cvWidescreen;

// STATIC: 0x00CABBB0
static CVar* s_cvGxWindow;

// STATIC: 0x00CABBB4
static CVar* s_cvWindowResizeLock;

// STATIC: 0x00CABBB8
static char s_windowTitle[256];

// STATIC: 0x00CABCB8
static CGxDevice* s_device;

// STATIC: 0x00CABCBC
static bool s_hardwareDetected;

// STATIC: 0x00CABCBD
static bool s_hwChanged;

// STATIC: 0x00CABCBE
static bool s_hwDetect;

// STATIC: 0x00CABCC0
static int32_t s_unk_CABCC0;

// STATIC: 0x00CABCC4
static int32_t s_consoleAccessEnabled;

// STATIC: 0x00CABCC8
static TSGrowableArray<CGxMonitorMode> s_gxMonitorModes;

// STATIC: 0x00CABCD8
static CGxFormat s_requestedFormat;

// STATIC: 0x00CABD30
static TSGrowableArray<void (*)(OPTION_PAGE)> s_defaultCallbacks;

// STATIC: 0x00CABD40
static CGxFormat s_desktopFormat(true, { 800, 600 }, CGxFormat::Fmt_ArgbX888, CGxFormat::Fmt_Ds24X, 60, 1, true, false, true, true, 0);

// STATIC: 0x00CABDA8
static CGxFormat s_fallbackFormat(false, { 640, 480 }, CGxFormat::Fmt_Rgb565, CGxFormat::Fmt_Ds160, 60, 1, true, false, true, true, 0);

// STATIC: 0x00CABE00
static CGxFormat s_lastGoodFormat;

// STATIC: 0x00ADBE0C
static const uint32_t s_formatToBpp[] = { 16, 32, 32, 32 };

// VERIFY: 992d9d20 auto
// OFFSET: 0x00769220
int32_t ConsoleDeviceExists() {
    return s_device != nullptr;
}

// VERIFY: 6e7fe291 auto
// OFFSET: 0x00769230
int32_t IsConsoleEnabled() {
    return s_consoleAccessEnabled;
}

// VERIFY: b2a180e4 auto
// OFFSET: 0x00769240
static bool CVGxColorBitsCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    switch (SStrToInt(value)) {
        case 16:
            s_requestedFormat.m_colorFormat = CGxFormat::Fmt_Rgb565;
            ConsoleWrite("set pending gxRestart", DEFAULT_COLOR);
            return true;

        case 24:
            s_requestedFormat.m_colorFormat = CGxFormat::Fmt_ArgbX888;
            ConsoleWrite("set pending gxRestart", DEFAULT_COLOR);
            return true;

        case 30:
            s_requestedFormat.m_colorFormat = CGxFormat::Fmt_Argb2101010;
            ConsoleWrite("set pending gxRestart", DEFAULT_COLOR);
            return true;

        default:
            ConsoleWrite("Color bits must be 16, 24, or 30", DEFAULT_COLOR);
            return false;
    }
}

// VERIFY: 1b589d62 auto
// OFFSET: 0x007692D0
static bool CVGxDepthBitsCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    switch (SStrToInt(value)) {
        case 16:
            s_requestedFormat.m_depthFormat = CGxFormat::Fmt_Ds160;
            ConsoleWrite("set pending gxRestart", DEFAULT_COLOR);
            return true;

        case 24:
            s_requestedFormat.m_depthFormat = CGxFormat::Fmt_Ds24X;
            ConsoleWrite("set pending gxRestart", DEFAULT_COLOR);
            return true;

        case 32:
            s_requestedFormat.m_depthFormat = CGxFormat::Fmt_Ds320;
            ConsoleWrite("set pending gxRestart", DEFAULT_COLOR);
            return true;

        default:
            ConsoleWrite("Depth bits must be 16, 24, or 32", DEFAULT_COLOR);
            return false;
    }
}

// OFFSET: 0x00769360
static bool CVGxTripleBufferCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    uint32_t tripleBuffer = SStrToInt(value);

    if (tripleBuffer >= 2) {
        ConsoleWrite("TripleBuffer must be 0 or 1", DEFAULT_COLOR);
        return false;
    }

    s_requestedFormat.m_backBufferCount = (tripleBuffer != 0) + 1;
    ConsoleWrite("set pending gxRestart", DEFAULT_COLOR);

    return true;
}

// VERIFY: 1b35a664 auto
// OFFSET: 0x007693B0
static bool CVGxApiCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    for (int32_t api = GxApi_OpenGl; api < GxApis_Last; api++) {
        if (GxApiSupported(static_cast<EGxApi>(api)) && !SStrCmpI(value, g_gxApiNames[api], STORM_MAX_STR)) {
            ConsoleWrite("GxApi set pending gxRestart", DEFAULT_COLOR);
            return true;
        }
    }

    char message[1024] = "unsupported api, must be one of ";
    bool first = true;

    for (int32_t api = GxApi_OpenGl; api < GxApis_Last; api++) {
        if (!GxApiSupported(static_cast<EGxApi>(api))) {
            continue;
        }

        if (!first) {
            strcat(message, ", ");
        }

        strcat(message, "'");
        strcat(message, g_gxApiNames[api]);
        strcat(message, "'");
        first = false;
    }

    ConsoleWrite(message, DEFAULT_COLOR);

    return false;
}

// VERIFY: 05847e9e auto
// OFFSET: 0x00769520
static bool CVGxVSyncCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    s_requestedFormat.m_vsync = SStrToInt(value);
    ConsoleWrite("set pending gxRestart", DEFAULT_COLOR);
    return true;
}

// VERIFY: b82579a5 auto
// OFFSET: 0x00769550
static bool CVGxWindowCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    s_requestedFormat.m_window = SStrToInt(value) != 0;
    ConsoleWrite("set pending gxRestart", DEFAULT_COLOR);
    return true;
}

// VERIFY: 88a23b6c auto
// OFFSET: 0x00769580
static bool CVGxAspectCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    s_requestedFormat.m_aspect = SStrToInt(value) != 0;
    ConsoleWrite("set pending gxRestart", DEFAULT_COLOR);
    return true;
}

// VERIFY: f09230be auto
// OFFSET: 0x007695B0
static bool CVGxMaximizeCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    s_requestedFormat.m_maximize = SStrToInt(value) != 0;
    ConsoleWrite("set pending gxRestart", DEFAULT_COLOR);
    return true;
}

// VERIFY: 5986dd1d auto
// OFFSET: 0x007695E0
static bool CVGxCursorCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    s_requestedFormat.m_hwCursor = SStrToInt(value) != 0;
    ConsoleWrite("set pending gxRestart", DEFAULT_COLOR);
    return true;
}

// OFFSET: 0x00769610
static bool CVGxMultisampleCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    int32_t multisample = SStrToInt(value);

    if (multisample <= 1) {
        multisample = 1;
    } else if (multisample >= 16) {
        multisample = 16;
    }

    s_requestedFormat.m_multisampleCount = multisample;
    ConsoleWrite("set pending gxRestart", DEFAULT_COLOR);

    return true;
}

// VERIFY: 1bcaf62f auto
// OFFSET: 0x00769650
static bool CVGxMultisampleQualityCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    float quality = SStrToFloat(value);

    if (quality < 0.0f) {
        quality = 0.0f;
    } else if (quality >= 1.0f) {
        quality = 1.0f;
    }

    s_requestedFormat.m_multisampleQuality = quality;
    ConsoleWrite("set pending gxRestart", DEFAULT_COLOR);

    return true;
}

// VERIFY: c1f4a85d auto
// OFFSET: 0x007696A0
static bool CVGxFixLagCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    s_requestedFormat.m_fixLag = SStrToInt(value) != 0;
    ConsoleWrite("set pending gxRestart", DEFAULT_COLOR);
    return true;
}

// VERIFY: e1f357be auto
// OFFSET: 0x007696D0
void ConsoleGxOverride(const char* list) {
    while (*list) {
        char key[256];
        char value[256];
        SStrTokenize(&list, key, sizeof(key), " ,", nullptr);
        SStrTokenize(&list, value, sizeof(value), " ;", nullptr);

        if (!key[0] || !value[0]) {
            continue;
        }

        uint32_t override = atol(key);
        uint32_t overrideValue = atol(value);

        if (override >= GxOverrides_Last) {
            continue;
        }

        if (override == GxOverride_PixelShader) {
            switch (overrideValue) {
                case 0:
                case 1:
                case 2:
                    overrideValue = 1;
                    break;

                case 3:
                    overrideValue = 2;
                    break;

                case 4:
                    overrideValue = 3;
                    break;

                case 5:
                    overrideValue = 7;
                    break;

                case 6:
                    overrideValue = 8;
                    break;

                case 7:
                    overrideValue = 9;
                    break;

                case 8:
                    overrideValue = 10;
                    break;

                case 10:
                    overrideValue = 12;
                    break;

                case 11:
                    overrideValue = 13;
                    break;

                default:
                    break;
            }
        }

        s_consoleGxOverrideSet[override] = 1;
        s_consoleGxOverrideVal[override] = overrideValue;
    }
}

// VERIFY: 34058bc1 auto
// OFFSET: 0x00769810
static bool CVGxOverrideCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    ConsoleGxOverride(value);
    return true;
}

// VERIFY: b867ad47 auto
// OFFSET: 0x00769830
static bool CVGxMaxFPSCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    uint32_t maxFPS = SStrToInt(value);

    if (maxFPS - 1 <= 6) {
        maxFPS = 8;
    }

    GxSetMaxFPS(maxFPS);

    return true;
}

// VERIFY: f691ee8b auto
// OFFSET: 0x00769860
static bool CVGxMaxFPSBkCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    uint32_t maxFPSBk = SStrToInt(value);

    if (maxFPSBk - 1 <= 6) {
        maxFPSBk = 8;
    }

    GxSetMaxFPSBk(maxFPSBk);

    return true;
}

// VERIFY: 7d022997 auto
// OFFSET: 0x00769890
static bool CVGxWindowResizeLockCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    OsInputSetWindowResizeLock(SStrToInt(value));
    return true;
}

// VERIFY: a99a0011 auto
// OFFSET: 0x007698B0
static bool UpdateGxCVars() {
    s_cvGxColorBits->Update();
    s_cvGxDepthBits->Update();
    s_cvGxWindow->Update();
    s_cvGxResolution->Update();
    s_cvGxRefresh->Update();
    s_cvGxTripleBuffer->Update();
    s_cvGxApi->Update();
    s_cvGxVSync->Update();
    s_cvGxAspect->Update();
    s_cvGxMaximize->Update();
    s_cvGxCursor->Update();
    s_cvGxMultisample->Update();
    s_cvGxMultisampleQuality->Update();
    return s_cvGxFixLag->Update();
}

// VERIFY: 6c4e6635 auto
// OFFSET: 0x00769950
static bool SetGxCVars(const CGxFormat& format) {
    char value[1024];

    s_cvGxColorBits->Set(s_formatToInt[format.m_colorFormat], true, false, false, true);
    s_cvGxDepthBits->Set(s_formatToInt[format.m_depthFormat], true, false, false, true);

    SStrPrintf(value, sizeof(value), "%d", format.m_window);
    s_cvGxWindow->Set(value, true, false, false, true);

    SStrPrintf(value, sizeof(value), "%dx%d", format.m_size.x, format.m_size.y);
    s_cvGxResolution->Set(value, true, false, false, true);

    SStrPrintf(value, sizeof(value), "%d", format.m_refreshRate);
    s_cvGxRefresh->Set(value, true, false, false, true);

    s_cvGxTripleBuffer->Set(format.m_backBufferCount > 1 ? "1" : "0", true, false, false, true);

    SStrPrintf(value, sizeof(value), "%d", format.m_vsync);
    s_cvGxVSync->Set(value, true, false, false, true);

    SStrPrintf(value, sizeof(value), "%d", format.m_aspect);
    s_cvGxAspect->Set(value, true, false, false, true);

    SStrPrintf(value, sizeof(value), "%d", format.m_maximize);
    s_cvGxMaximize->Set(value, true, false, false, true);

    SStrPrintf(value, sizeof(value), "%d", format.m_hwCursor);
    s_cvGxCursor->Set(value, true, false, false, true);

    SStrPrintf(value, sizeof(value), "%d", format.m_multisampleCount);
    s_cvGxMultisample->Set(value, true, false, false, true);

    SStrPrintf(value, sizeof(value), "%f", format.m_multisampleQuality);
    s_cvGxMultisampleQuality->Set(value, true, false, false, true);

    SStrPrintf(value, sizeof(value), "%d", format.m_fixLag);
    s_cvGxFixLag->Set(value, true, false, false, true);

    return UpdateGxCVars();
}

// VERIFY: a95866c0 auto
// OFFSET: 0x00769C00
static bool CVGxStereoEnabledCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    s_requestedFormat.m_stereoEnabled = SStrToInt(value) == 1;
    ConsoleWrite("set pending gxRestart", DEFAULT_COLOR);
    return true;
}

// VERIFY: 30f5328f auto
// OFFSET: 0x00769C30
static bool CVGxStereoConvergenceCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    g_theGxDevicePtr->StereoSetConvergence(SStrToFloat(value));
    return true;
}

// VERIFY: 75b13ba9 auto
// OFFSET: 0x00769C60
static bool CVGxStereoSeparationCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    g_theGxDevicePtr->StereoSetSeparation(SStrToFloat(value));
    return true;
}

// OFFSET: 0x005EEB70
static void __fastcall StereoChangedCallback() {
}

// VERIFY: 1d47cb44 auto
// OFFSET: 0x00769C90
static void RegisterStereoCVars() {
    s_cvGxStereoConvergence = CVar::Register("gxStereoConvergence", "Set stereoscopic rendering convergence depth", 1, "1", CVGxStereoConvergenceCallback, CATEGORY_GRAPHICS, false, nullptr, false);
    s_cvGxStereoSeparation = CVar::Register("gxStereoSeparation", "Set stereoscopic rendering separation percentage", 1, "25", CVGxStereoSeparationCallback, CATEGORY_GRAPHICS, false, nullptr, false);
    g_theGxDevicePtr->AddStereoChangedCallback(StereoChangedCallback);
}

// VERIFY: 72262368 auto
// OFFSET: 0x00769D00
Hardware* ConsoleGetHardware() {
    return s_hardwareDetected ? &s_hardware : nullptr;
}

// VERIFY: a072ef73 auto
// OFFSET: 0x00769D10
int32_t ConsoleDeviceHardwareChanged() {
    return s_hwDetect || s_hwChanged;
}

// VERIFY: ce2a0330 auto
// OFFSET: 0x00769D30
DefaultSettings* ConsoleDeviceGetDefaultSettings() {
    return &s_defaults;
}

// VERIFY: d88758a1 auto
// OFFSET: 0x00769D40
void ConsoleDeviceDestroy() {
    g_theGxDevicePtr->RemoveStereoChangedCallback(StereoChangedCallback);
    s_cvVideoOptionsVersion->Update();
    GxDevDestroy(s_device);
    s_device = nullptr;
    GxLogClose();
}

// OFFSET: 0x00769E10
void ValidateFormatMonitor(CGxFormat& format) {
    static C2iVector s_resolutions[] = {
        { 1600, 1200 },
        { 1280, 1024 },
        { 1280, 960 },
        { 1152, 864 },
        { 1024, 768 },
        { 800, 600 },
        { 640, 480 },
    };

    int32_t width = format.m_size.x;
    int32_t height = format.m_size.y;
    int32_t index = -1;
    uint32_t bpp = s_formatToBpp[format.m_colorFormat];
    uint32_t minRefresh = 9999;

    CGxMonitorMode desktop;
    desktop.m_size.x = 0;
    desktop.m_size.y = 0;

    if (!format.m_window || !GxAdapterDesktopMode(desktop) || desktop.m_size.x < format.m_size.x || desktop.m_size.y < format.m_size.y) {
        while (true) {
            uint32_t i;

            for (i = 0; i < s_gxMonitorModes.Count(); i++) {
                if (width == s_gxMonitorModes[i].m_size.x && height == s_gxMonitorModes[i].m_size.y) {
                    break;
                }
            }

            if (i < s_gxMonitorModes.Count()) {
                format.m_size.x = width;
                format.m_size.y = height;
                break;
            }

            do {
                if (++index >= static_cast<int32_t>(sizeof(s_resolutions) / sizeof(s_resolutions[0]))) {
                    goto refresh;
                }
            } while (s_resolutions[index].x >= width || s_resolutions[index].y >= height);

            width = s_resolutions[index].x;
            height = s_resolutions[index].y;
        }
    }

refresh:
    for (uint32_t i = 0; i < s_gxMonitorModes.Count(); i++) {
        CGxMonitorMode& mode = s_gxMonitorModes[i];

        if (format.m_size.x == mode.m_size.x && format.m_size.y == mode.m_size.y && bpp == mode.m_bpp) {
            if (mode.m_refreshRate < minRefresh) {
                minRefresh = mode.m_refreshRate;
            }

            if (format.m_refreshRate == mode.m_refreshRate) {
                return;
            }
        }
    }

    if (minRefresh == 9999) {
        minRefresh = 60;
        GxLog("ValidateFormatMonitor(): unable to find monitor refresh");
    }

    GxLog("ValidateFormatMonitor(): invalid refresh rate %d, set to %d", format.m_refreshRate, minRefresh);
    format.m_refreshRate = minRefresh;
}

// OFFSET: 0x00769FF0
static int32_t CCGxRestart(const char* command, const char* arguments) {
    OnGxRestartClearWorld();
    ValidateFormatMonitor(s_requestedFormat);

    if (g_theGxDevicePtr->DeviceSetFormat(s_requestedFormat)) {
        s_lastGoodFormat = s_requestedFormat;
        UpdateGxCVars();
    } else {
        ConsoleWrite("unable to set requested display mode", DEFAULT_COLOR);
        s_requestedFormat = s_lastGoodFormat;

        if (!g_theGxDevicePtr->DeviceSetFormat(s_requestedFormat)) {
            ConsoleWrite("unable to set last good mode", DEFAULT_COLOR);
            s_requestedFormat = s_desktopFormat;

            if (!g_theGxDevicePtr->DeviceSetFormat(s_requestedFormat)) {
                ConsoleWrite("unable to set default format", DEFAULT_COLOR);
                s_requestedFormat = s_fallbackFormat;

                if (!g_theGxDevicePtr->DeviceSetFormat(s_requestedFormat)) {
                    goto done;
                }
            }

            s_lastGoodFormat = s_requestedFormat;
        }

        SetGxCVars(s_requestedFormat);
    }

done:
    OsGuiSetGxWindow(g_theGxDevicePtr->DeviceWindow());
    OsGuiSetWindowTitle(g_theGxDevicePtr->DeviceWindow(), s_windowTitle);
    TextureNotifyGxRestart();

    return 1;
}

// VERIFY: 35de767a auto
// OFFSET: 0x0076A130
void ConsoleDeviceDefaults(OPTION_PAGE page) {
    ConsoleDetectSetDefaults(s_defaults, s_hardware);
    s_cvFixedFunction->Set("0", true, false, false, true);

    if (page == OPTION_PAGE_0) {
        s_requestedFormat = *s_defaults.m_format;
        SetGxCVars(s_requestedFormat);
        CCGxRestart(nullptr, nullptr);
    }

    for (uint32_t i = 0; i < s_defaultCallbacks.Count(); i++) {
        s_defaultCallbacks[i](page);
    }
}

// VERIFY: dc6233ea auto
// OFFSET: 0x0076A220
static bool CVGxResolutionCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    int32_t width = -1;
    int32_t height = -1;
    char separator;
    sscanf(value, "%d%c%d", &width, &separator, &height);

    if (s_requestedFormat.m_window) {
        s_requestedFormat.m_size.x = width;
        s_requestedFormat.m_size.y = height;
        ConsoleWrite("set pending gxRestart", DEFAULT_COLOR);
        return true;
    }

    TSGrowableArray<C2iVector> resolutions;
    C2iVector resolution;

    if (s_cvWidescreen->m_intValue) {
        TSGrowableArray<CGxMonitorMode> modes;
        GxAdapterMonitorModes(modes);

        C2iVector last = { 0, 0 };

        for (uint32_t i = 0; i < modes.Count(); i++) {
            const C2iVector& size = modes[i].m_size;

            if (static_cast<float>(size.x) / size.y >= 1.248f && size.x >= 640 && size.y >= 480 && (size.x != last.x || size.y != last.y)) {
                last = size;
                resolutions.New(last);
            }
        }
    }

    if (!resolutions.Count()) {
        resolution.x = 640;
        resolution.y = 480;
        resolutions.New(resolution);
        resolution.x = 800;
        resolution.y = 600;
        resolutions.New(resolution);
        resolution.x = 1024;
        resolution.y = 768;
        resolutions.New(resolution);
        resolution.x = 1152;
        resolution.y = 864;
        resolutions.New(resolution);
        resolution.x = 1280;
        resolution.y = 960;
        resolutions.New(resolution);
        resolution.x = 1280;
        resolution.y = 1024;
        resolutions.New(resolution);
        resolution.x = 1600;
        resolution.y = 1200;
        resolutions.New(resolution);
    }

    uint32_t i;

    for (i = 0; i < resolutions.Count(); i++) {
        if (width == resolutions[i].x && height == resolutions[i].y) {
            break;
        }
    }

    if (i == resolutions.Count()) {
        char message[256] = "invalid resolution, must be one of ";

        for (uint32_t j = 0; j < resolutions.Count(); j++) {
            if (j) {
                strcat(message, ", ");
            }

            if (SStrLen(message) > 100) {
                ConsoleWrite(message, DEFAULT_COLOR);
                message[0] = '\0';
            }

            char entry[16];
            SStrPrintf(entry, 32, "%dx%d", resolutions[j].x, resolutions[j].y);
            strcat(message, entry);
        }

        ConsoleWrite(message, DEFAULT_COLOR);

        return false;
    }

    s_requestedFormat.m_size.x = width;
    s_requestedFormat.m_size.y = height;
    ConsoleWrite("set pending gxRestart", DEFAULT_COLOR);

    return true;
}

// OFFSET: 0x0076A580
static bool CVGxRefreshCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    uint32_t refreshRate = SStrToUnsigned(value);

    TSGrowableArray<CGxMonitorMode> modes;
    GxAdapterMonitorModes(modes);

    uint32_t i;

    for (i = 0; i < modes.Count(); i++) {
        if (refreshRate == modes[i].m_refreshRate) {
            break;
        }
    }

    if (i == modes.Count()) {
        ConsoleWrite("Unsupported refresh rate", DEFAULT_COLOR);
        return false;
    }

    s_requestedFormat.m_refreshRate = refreshRate;
    ConsoleWrite("set pending gxRestart", DEFAULT_COLOR);

    return true;
}

// VERIFY: 05a0817f auto
// OFFSET: 0x008A1420
static bool CVVideoOptionsVersionCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    return true;
}

// VERIFY: 6078252d auto
// OFFSET: 0x0076A630
static void RegisterGxCVars() {
    const CGxFormat& format = *s_defaults.m_format;
    bool window = s_locale == 4;
    char value[260];

    s_cvWidescreen = CVar::Register("widescreen", "Allow widescreen support", 0, "1", nullptr, CATEGORY_GRAPHICS, false, nullptr, false);
    s_cvGxWindow = CVar::Register("gxWindow", "toggle fullscreen/window", 3, window ? "1" : "0", CVGxWindowCallback, CATEGORY_GRAPHICS, false, nullptr, false);
    s_cvGxMaximize = CVar::Register("gxMaximize", "maximize game window", 3, window ? "1" : "0", CVGxMaximizeCallback, CATEGORY_GRAPHICS, false, nullptr, false);

    SStrPrintf(value, sizeof(value), "%s", s_formatToInt[format.m_colorFormat]);
    s_cvGxColorBits = CVar::Register("gxColorBits", "color bits", 3, value, CVGxColorBitsCallback, CATEGORY_GRAPHICS, false, nullptr, false);

    SStrPrintf(value, sizeof(value), "%s", s_formatToInt[format.m_depthFormat]);
    s_cvGxDepthBits = CVar::Register("gxDepthBits", "depth bits", 3, value, CVGxDepthBitsCallback, CATEGORY_GRAPHICS, false, nullptr, false);

    SStrPrintf(value, sizeof(value), "%dx%d", format.m_size.x, format.m_size.y);
    s_cvGxResolution = CVar::Register("gxResolution", "resolution", 3, value, CVGxResolutionCallback, CATEGORY_GRAPHICS, false, nullptr, false);

    s_cvGxRefresh = CVar::Register("gxRefresh", "refresh rate", 3, "75", CVGxRefreshCallback, CATEGORY_GRAPHICS, false, nullptr, false);
    s_cvGxTripleBuffer = CVar::Register("gxTripleBuffer", "triple buffer", 3, "0", CVGxTripleBufferCallback, CATEGORY_GRAPHICS, false, nullptr, false);
    s_cvGxApi = CVar::Register("gxApi", "graphics api", 3, g_gxApiNames[GxDefaultApi()], CVGxApiCallback, CATEGORY_GRAPHICS, false, nullptr, false);
    s_cvGxVSync = CVar::Register("gxVSync", "vsync on or off", 3, "1", CVGxVSyncCallback, CATEGORY_GRAPHICS, false, nullptr, false);
    s_cvGxAspect = CVar::Register("gxAspect", "constrain window aspect", 3, "1", CVGxAspectCallback, CATEGORY_GRAPHICS, false, nullptr, false);
    s_cvGxCursor = CVar::Register("gxCursor", "toggle hardware cursor", 3, "1", CVGxCursorCallback, CATEGORY_GRAPHICS, false, nullptr, false);

    SStrPrintf(value, sizeof(value), "%d", s_hardware.m_videoHw->m_multisample);
    s_cvGxMultisample = CVar::Register("gxMultisample", "multisample", 3, value, CVGxMultisampleCallback, CATEGORY_GRAPHICS, false, nullptr, false);

    s_cvGxMultisampleQuality = CVar::Register("gxMultisampleQuality", "multisample quality", 3, "0.0", CVGxMultisampleQualityCallback, CATEGORY_GRAPHICS, false, nullptr, false);

    SStrPrintf(value, sizeof(value), "%d", s_hardware.m_videoHw->m_fixLag);
    s_cvGxFixLag = CVar::Register("gxFixLag", "prevent cursor lag", 3, value, CVGxFixLagCallback, CATEGORY_GRAPHICS, false, nullptr, false);

    s_cvGxStereoEnabled = CVar::Register("gxStereoEnabled", "Enable stereoscopic rendering", 1, "0", CVGxStereoEnabledCallback, CATEGORY_GRAPHICS, false, nullptr, false);
    s_cvGxOverride = CVar::Register("gxOverride", "gx overrides", 1, "", CVGxOverrideCallback, CATEGORY_GRAPHICS, false, nullptr, false);
    s_cvMaxFPS = CVar::Register("maxFPS", "Set FPS limit", 1, "200", CVGxMaxFPSCallback, CATEGORY_GRAPHICS, false, nullptr, false);
    s_cvMaxFPSBk = CVar::Register("maxFPSBk", "Set background FPS limit", 1, "30", CVGxMaxFPSBkCallback, CATEGORY_GRAPHICS, false, nullptr, false);
    s_cvVideoOptionsVersion = CVar::Register("videoOptionsVersion", "Video options version", 3, "0", CVVideoOptionsVersionCallback, CATEGORY_GRAPHICS, false, nullptr, false);
    s_cvWindowResizeLock = CVar::Register("windowResizeLock", "prevent resizing in windowed mode", 1, "0", CVGxWindowResizeLockCallback, CATEGORY_GRAPHICS, false, nullptr, false);
    s_cvFixedFunction = CVar::Register("fixedFunction", "Force fixed function rendering", 3, "0", nullptr, CATEGORY_GRAPHICS, false, nullptr, false);
}

// VERIFY: 575661ec auto
// OFFSET: 0x0076AAB0
void AddConsoleDeviceDefaultCallback(void (*callback)(OPTION_PAGE page)) {
    s_defaultCallbacks.New(callback);

    if (s_unk_CABCC0) {
        for (uint32_t page = OPTION_PAGE_0; page < NUM_OPTION_PAGES; page++) {
            callback(static_cast<OPTION_PAGE>(page));
        }
    }
}

// OFFSET: 0x0076AAF0
void RemoveConsoleDeviceDefaultCallback(void (*callback)(OPTION_PAGE page)) {
    uint32_t count = s_defaultCallbacks.Count();

    for (uint32_t i = 0; i < count; i++) {
        if (s_defaultCallbacks[i] == callback) {
            s_defaultCallbacks[i] = s_defaultCallbacks[--count];
            s_defaultCallbacks.SetCount(count);
        }
    }
}

// OFFSET: 0x0076AB80
void ConsoleDeviceInitialize(const char* title, bool a2, bool a3) {
    GxLogOpen();

    s_cvHwDetect = CVar::Register("hwDetect", "do hardware detection", 1, "1", nullptr, CATEGORY_GRAPHICS, false, nullptr, false);
    ConsoleDetectDetectHardware(s_hardware, s_hwChanged);
    s_hardwareDetected = true;

    if (CmdLineGetBool(static_cast<CMDOPT>(CMD_HW_DETECT)) || s_cvHwDetect->m_intValue) {
        s_hwDetect = true;
        s_cvHwDetect->Set("0", true, false, false, true);
    } else {
        s_hwDetect = false;
    }

    s_consoleAccessEnabled = CmdLineGetBool(static_cast<CMDOPT>(CMD_CONSOLE));

    ConsoleDetectSetDefaultsFormat(s_defaults, s_hardware);
    RegisterGxCVars();
    ConsoleCommandRegister("gxRestart", CCGxRestart, CATEGORY_GRAPHICS, nullptr);

    GxAdapterMonitorModes(s_gxMonitorModes);
    ValidateFormatMonitor(s_fallbackFormat);

    CGxMonitorMode desktop;
    desktop.m_size.x = 0;
    desktop.m_size.y = 0;

    if (GxAdapterDesktopMode(desktop)) {
        s_desktopFormat.m_size.x = desktop.m_size.x;
        s_desktopFormat.m_size.y = desktop.m_size.y;
        s_desktopFormat.m_colorFormat = desktop.m_bpp > 16 ? CGxFormat::Fmt_ArgbX888 : CGxFormat::Fmt_Rgb565;
        s_desktopFormat.m_refreshRate = desktop.m_refreshRate;
    }

    GxLog("ConsoleDeviceInitialize(): hwDetect = %d, hwChanged = %d", s_hwDetect, s_hwChanged);

    if (CmdLineGetBool(static_cast<CMDOPT>(CMD_RES_800x600))) {
        s_requestedFormat.m_size.x = 800;
        s_requestedFormat.m_size.y = 600;
    } else if (CmdLineGetBool(static_cast<CMDOPT>(CMD_RES_1024x768))) {
        s_requestedFormat.m_size.x = 1024;
        s_requestedFormat.m_size.y = 768;
    } else if (CmdLineGetBool(static_cast<CMDOPT>(CMD_RES_1280x960))) {
        s_requestedFormat.m_size.x = 1280;
        s_requestedFormat.m_size.y = 960;
    } else if (CmdLineGetBool(static_cast<CMDOPT>(CMD_RES_1280x1024))) {
        s_requestedFormat.m_size.x = 1280;
        s_requestedFormat.m_size.y = 1024;
    } else if (CmdLineGetBool(static_cast<CMDOPT>(CMD_RES_1600x1200))) {
        s_requestedFormat.m_size.x = 1600;
        s_requestedFormat.m_size.y = 1200;
    }

    if (s_cvFixedFunction->m_intValue) {
        s_requestedFormat.m_fixedFunction[4] = 0;
        s_requestedFormat.m_fixedFunction[0] = 0;
    }

    if (s_hwDetect || s_hwChanged) {
        ConsoleDetectSetDefaults(s_defaults, s_hardware);
        s_cvFixedFunction->Set("0", true, false, false, true);
        s_requestedFormat = *s_defaults.m_format;
        s_requestedFormat.m_window = s_cvGxWindow->m_intValue != 0;
        s_requestedFormat.m_maximize = s_cvGxMaximize->m_intValue != 0;
        SetGxCVars(s_requestedFormat);
    }

    EGxApi api = GxDefaultApi();
    CVar* apiVar = CVar::Lookup("gxApi");

    if (apiVar) {
        const char* apiName = apiVar->m_stringValue.m_str;

        for (int32_t i = GxApi_OpenGl; i < GxApis_Last; i++) {
            if (GxApiSupported(static_cast<EGxApi>(i)) && !SStrCmpI(apiName, g_gxApiNames[i], STORM_MAX_STR)) {
                api = static_cast<EGxApi>(i);
                break;
            }
        }
    }

    if (CmdLineGetBool(CMD_OPENGL) && GxApiSupported(GxApi_OpenGl)) {
        api = GxApi_OpenGl;
    }

    if (CmdLineGetBool(CMD_D3D) && GxApiSupported(GxApi_D3d9)) {
        api = GxApi_D3d9;
    }

    if (CmdLineGetBool(CMD_D3D9EX) && GxApiSupported(GxApi_D3d9Ex)) {
        api = GxApi_D3d9Ex;
    }

    s_requestedFormat.m_fixLag = s_cvGxFixLag->m_intValue != 0;
    s_requestedFormat.m_hwTnL = !CmdLineGetBool(CMD_SW_TNL);

    bool window = s_cvGxWindow->m_intValue != 0;

    if (CmdLineGetBool(CMD_FULL_SCREEN)) {
        window = false;
    } else if (CmdLineGetBool(static_cast<CMDOPT>(CMD_WINDOWED))) {
        window = true;
    }

    s_requestedFormat.m_window = window;
    s_desktopFormat.m_window = window;

    bool desktopTried = false;
    CGxFormat format = s_requestedFormat;
    ValidateFormatMonitor(format);

    for (s_device = CreateGxDevice(api, OsWindowProc, format); !s_device; s_device = CreateGxDevice(api, OsWindowProc, format)) {
        if (format.m_multisampleCount > 1) {
            if (format.m_multisampleCount - 2 <= 1) {
                format.m_multisampleCount = 1;
            } else {
                format.m_multisampleCount -= 2;
            }
        } else if (format.m_colorFormat > CGxFormat::Fmt_Rgb565) {
            if (format.m_depthFormat <= CGxFormat::Fmt_Ds160) {
                format.m_depthFormat = format.m_colorFormat == CGxFormat::Fmt_ArgbX888 ? CGxFormat::Fmt_Ds160 : CGxFormat::Fmt_Ds320;
                format.m_colorFormat = static_cast<CGxFormat::Format>(format.m_colorFormat - 1);
            } else {
                format.m_depthFormat = static_cast<CGxFormat::Format>(format.m_depthFormat - 1);
            }
        } else {
            if (desktopTried) {
                GxLog("ConsoleDeviceInitialize(): no output device available!");

                Startup_StringsRec* titleRec = g_startup_StringsDB.GetRecord(1);
                const char* caption = titleRec ? titleRec->m_message : "World of Warcraft";
                const char* message;

                if (api == GxApi_D3d9 || api == GxApi_D3d9Ex) {
                    Startup_StringsRec* rec = g_startup_StringsDB.GetRecord(2);
                    message = rec ? rec->m_message : "World of Warcraft was unable to start up 3D acceleration. Please make sure DirectX 9.0c is installed and your video drivers are up-to-date.";
                } else {
                    Startup_StringsRec* rec = g_startup_StringsDB.GetRecord(3);
                    message = rec ? rec->m_message : "World of Warcraft was unable to start up 3D acceleration.";
                }

                OsGuiMessageBox(nullptr, 0, message, caption);
                GxLogClose();
                exit(0);
            }

            format = s_desktopFormat;
            desktopTried = true;
        }

        ValidateFormatMonitor(format);
    }

    s_requestedFormat = format;
    s_lastGoodFormat = format;
    SetGxCVars(s_requestedFormat);

    if (g_theGxDevicePtr->Caps().m_numTmus < 2) {
        GxDevDestroy(s_device);
        GxLog("ConsoleDeviceInitialize(): output device does not have dual TMUs!");
    } else if (!g_theGxDevicePtr->Caps().m_numStreams) {
        GxDevDestroy(s_device);
        GxLog("ConsoleDeviceInitialize(): output device has 0 streams");
    } else {
        switch (g_theGxDevicePtr->DeviceApi()) {
            case GxApi_OpenGl:
            case GxApi_GLL:
                ConsoleGxOverride(s_hardware.m_videoHw->m_oglOverrides);
                break;

            case GxApi_D3d9:
            case GxApi_D3d9Ex:
                ConsoleGxOverride(s_hardware.m_videoHw->m_d3DOverrides);
                break;

            default:
                break;
        }

        ConsoleGxOverride(CmdLineGetString(static_cast<CMDOPT>(CMD_GX_OVERRIDE)));

        for (uint32_t i = 0; i < GxOverrides_Last; i++) {
            if (s_consoleGxOverrideSet[i]) {
                g_theGxDevicePtr->DeviceOverride(static_cast<EGxOverride>(i), s_consoleGxOverrideVal[i]);
            }
        }

        OsGuiSetGxWindow(g_theGxDevicePtr->DeviceWindow());
        SStrCopy(s_windowTitle, title ? title : "", sizeof(s_windowTitle));

        void* window = g_theGxDevicePtr->DeviceWindow();

        if (window) {
            OsGuiSetWindowTitle(window, s_windowTitle);
        }

        if (s_hwDetect || s_hwChanged) {
            s_defaults.unk_0019 = g_theGxDevicePtr->Caps().m_shaderTargets[4] != 0;
        }

        char version[32];
        SStrPrintf(version, sizeof(version), "%d", 3);
        s_cvVideoOptionsVersion->Set(version, true, false, false, true);

        RegisterStereoCVars();
        OsSetSleepInBackground(1);
        OsSetBackgroundSleepMs(250);

        return;
    }

    Startup_StringsRec* titleRec = g_startup_StringsDB.GetRecord(1);
    const char* caption = titleRec ? titleRec->m_message : "World of Warcraft";
    Startup_StringsRec* rec = g_startup_StringsDB.GetRecord(4);
    const char* message = rec ? rec->m_message : "Your 3D accelerator card is not supported by World of Warcraft. Please install a 3D accelerator card with dual-TMU support.";

    OsGuiMessageBox(nullptr, 0, message, caption);
    GxLogClose();
    exit(0);
}
