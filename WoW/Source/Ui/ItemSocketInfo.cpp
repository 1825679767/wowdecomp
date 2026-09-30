#include "ItemSocketInfo.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/GemPropertiesRec.h"
#include "WoW/Source/Object/ItemStats.h"

// STATIC: 0x00C20FC8
WOWGUID CGItemSocketInfo::m_gemItems[3];

// VERIFY: c7291b92 manual
// OFFSET: 0x005C48D0
int32_t CGItemSocketInfo::GemMatchesSocket(const ItemStats_C* item, const ItemStats_C* gem, int32_t socket) {
    if (!item || !gem) {
        return 0;
    }

    uint32_t color = item->m_socketColor[socket];

    if (!color) {
        return 1;
    }

    GemPropertiesRec* properties = g_gemPropertiesDB.GetRecord(gem->m_gemProperties);

    if (!properties) {
        return 0;
    }

    int32_t type = properties->m_type;
    int32_t match = 0;

    if ((type & 1) && (color & 1)) {
        match = 1;
    }

    if ((type & 2) && (color & 2)) {
        match = 1;
    }

    if ((type & 4) && (color & 4)) {
        match = 1;
    }

    if ((type & 8) && (color & 8)) {
        return 1;
    }

    return match;
}
