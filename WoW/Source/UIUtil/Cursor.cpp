#include "Cursor.h"

// STATIC: 0x00C25DE0
static uint32_t s_cursorItemType;

// VERIFY: dd7c01bd auto
// OFFSET: 0x006167E0
void CursorDropItem() {
    if (s_cursorItemType) {
        s_cursorItemType = 0;
        UpdateCursor();
    }
}
