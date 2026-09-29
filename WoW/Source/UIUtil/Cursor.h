#ifndef WOW_SOURCE_UIUTIL_CURSOR_H
#define WOW_SOURCE_UIUTIL_CURSOR_H

#include <cstdint>

void CursorInitialize();

void CursorReleaseItemMips();
uint32_t CursorGetResetMode();
void CursorResetMode();
void CursorSetMode(uint32_t mode);
void CursorSetResetMode(uint32_t mode);
void CursorDropItem();
void UpdateCursor();
void CursorGrabSpell(const char* texture);
void CursorGrabVirtualItem(int32_t displayId);

#endif
