#ifndef ENGINE_SOURCE_OS_GUI_H
#define ENGINE_SOURCE_OS_GUI_H

#include <cstdint>

void* OsGuiGetWindow(int32_t type);
int32_t OsGuiIsModifierKeyDown(int32_t key);
int32_t OsGuiProcessMessage(void* message);
void OsGuiSetGxWindow(void* window);
int32_t OsGuiMessageBox(void* parentWindow, int32_t style, const char* message, const char* title);
void OsGuiSetWindowTitle(void* window, const char* title);

#endif
