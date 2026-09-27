#ifndef WOW_SOURCE_CONSOLEDEVICE_CONSOLEDEVICE_H
#define WOW_SOURCE_CONSOLEDEVICE_CONSOLEDEVICE_H

#include <cstdint>
#include "WoW/Source/Types.h"

class CGxFormat;

enum OPTION_PAGE {
    OPTION_PAGE_0 = 0,
    OPTION_PAGE_1 = 1,
    OPTION_PAGE_2 = 2,
    NUM_OPTION_PAGES = 3,
};

void AddConsoleDeviceDefaultCallback(void (*callback)(OPTION_PAGE page));
int32_t ConsoleDeviceExists();
void ConsoleDeviceDefaults(OPTION_PAGE page);
void ConsoleDeviceDestroy();
DefaultSettings* ConsoleDeviceGetDefaultSettings();
int32_t ConsoleDeviceHardwareChanged();
void ConsoleDeviceInitialize(const char* title, bool a2, bool a3);
Hardware* ConsoleGetHardware();
void ConsoleGxOverride(const char* list);
int32_t IsConsoleEnabled();
void RemoveConsoleDeviceDefaultCallback(void (*callback)(OPTION_PAGE page));
void ValidateFormatMonitor(CGxFormat& format);

#endif
