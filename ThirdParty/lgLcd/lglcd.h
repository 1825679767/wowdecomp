#ifndef THIRDPARTY_LGLCD_LGLCD_H
#define THIRDPARTY_LGLCD_LGLCD_H

#include <windows.h>

#define LGLCD_INVALID_CONNECTION (-1)
#define LGLCD_INVALID_DEVICE (-1)

#define LGLCD_BMP_FORMAT_160x43x1 (0x00000001)
#define LGLCD_BMP_FORMAT_QVGAx32 (0x00000003)

#define LGLCD_BMP_WIDTH (160)
#define LGLCD_BMP_HEIGHT (43)
#define LGLCD_BMP_BPP (1)

#define LGLCD_QVGA_BMP_WIDTH (320)
#define LGLCD_QVGA_BMP_HEIGHT (240)
#define LGLCD_QVGA_BMP_BPP (4)

#define LGLCD_PRIORITY_NORMAL (128)

#define LGLCD_APPLET_CAP_BASIC (0x00000000)
#define LGLCD_APPLET_CAP_BW (0x00000001)
#define LGLCD_APPLET_CAP_QVGA (0x00000002)

#define LGLCD_NOTIFICATION_DEVICE_ARRIVAL (0x00000001)
#define LGLCD_NOTIFICATION_DEVICE_REMOVAL (0x00000002)
#define LGLCD_NOTIFICATION_CLOSE_CONNECTION (0x00000003)
#define LGLCD_NOTIFICATION_APPLET_DISABLED (0x00000004)
#define LGLCD_NOTIFICATION_APPLET_ENABLED (0x00000005)

typedef DWORD (WINAPI* lgLcdOnConfigureCB)(int connection, const PVOID pContext);

typedef struct {
    lgLcdOnConfigureCB configCallback;
    PVOID configContext;
} lgLcdConfigureContext;

typedef DWORD (WINAPI* lgLcdOnNotificationCB)(int connection, const PVOID pContext, DWORD notificationCode, DWORD notifyParm1, DWORD notifyParm2, DWORD notifyParm3, DWORD notifyParm4);

typedef struct {
    lgLcdOnNotificationCB notificationCallback;
    PVOID notifyContext;
} lgLcdNotificationContext;

typedef DWORD (WINAPI* lgLcdOnSoftButtonsCB)(int device, DWORD dwButtons, const PVOID pContext);

typedef struct {
    lgLcdOnSoftButtonsCB softbuttonsChangedCallback;
    PVOID softbuttonsChangedContext;
} lgLcdSoftbuttonsChangedContext;

typedef struct {
    LPCSTR appFriendlyName;
    BOOL isPersistent;
    BOOL isAutostartable;
    lgLcdConfigureContext onConfigure;
    int connection;
} lgLcdConnectContextA;

typedef struct {
    LPCSTR appFriendlyName;
    BOOL isPersistent;
    BOOL isAutostartable;
    lgLcdConfigureContext onConfigure;
    int connection;
    DWORD dwAppletCapabilitiesSupported;
    DWORD dwReserved1;
    lgLcdNotificationContext onNotify;
} lgLcdConnectContextExA;

typedef struct {
    int connection;
    int deviceType;
    lgLcdSoftbuttonsChangedContext onSoftbuttonsChanged;
    int device;
} lgLcdOpenByTypeContext;

typedef struct {
    DWORD Format;
} lgLcdBitmapHeader;

typedef struct {
    lgLcdBitmapHeader hdr;
    BYTE pixels[LGLCD_BMP_WIDTH * LGLCD_BMP_HEIGHT * LGLCD_BMP_BPP];
} lgLcdBitmap160x43x1;

typedef struct {
    lgLcdBitmapHeader hdr;
    BYTE pixels[LGLCD_QVGA_BMP_WIDTH * LGLCD_QVGA_BMP_HEIGHT * LGLCD_QVGA_BMP_BPP];
} lgLcdBitmapQVGAx32;

typedef lgLcdConnectContextA lgLcdConnectContext;
typedef lgLcdConnectContextExA lgLcdConnectContextEx;

DWORD WINAPI lgLcdInit(void);
DWORD WINAPI lgLcdDeInit(void);
DWORD WINAPI lgLcdConnectExA(lgLcdConnectContextExA* ctx);
DWORD WINAPI lgLcdDisconnect(int connection);
DWORD WINAPI lgLcdOpenByType(lgLcdOpenByTypeContext* ctx);
DWORD WINAPI lgLcdClose(int device);
DWORD WINAPI lgLcdUpdateBitmap(int device, const lgLcdBitmapHeader* bitmap, DWORD priority);
DWORD WINAPI lgLcdSetAsLCDForegroundApp(int device, int foregroundYesNoFlag);

#define lgLcdConnectEx lgLcdConnectExA

#endif
