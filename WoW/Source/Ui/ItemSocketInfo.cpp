#include "ItemSocketInfo.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/GemPropertiesRec.h"
#include "WoW/Source/Object/ItemStats.h"
#include <cstring>

// STATIC: 0x00C20FA8
uint8_t CGItemSocketInfo::unk_C20FA8[3];

// STATIC: 0x00C20FB0
WOWGUID CGItemSocketInfo::m_gemItemContainer[3];

// STATIC: 0x00C20FC8
WOWGUID CGItemSocketInfo::m_gemItems[3];

// STATIC: 0x00C20FE0
WOWGUID CGItemSocketInfo::m_socketItem;

// STATIC: 0x00C20FE8
int32_t CGItemSocketInfo::unk_C20FE8;

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

// VERIFY: ef895c80 auto
// OFFSET: 0x005C44F0
void CGItemSocketInfo::EnterWorld() {
    CGItemSocketInfo::m_socketItem = WOWGUID{ 0 };
    CGItemSocketInfo::unk_C20FE8 = 0;
    CGItemSocketInfo::m_gemItems[0] = WOWGUID{ 0 };
    CGItemSocketInfo::m_gemItems[1] = WOWGUID{ 0 };
    CGItemSocketInfo::m_gemItems[2] = WOWGUID{ 0 };
    CGItemSocketInfo::m_gemItemContainer[0] = WOWGUID{ 0 };
    CGItemSocketInfo::m_gemItemContainer[1] = WOWGUID{ 0 };
    CGItemSocketInfo::m_gemItemContainer[2] = WOWGUID{ 0 };
    memset(CGItemSocketInfo::unk_C20FA8, 0, sizeof(CGItemSocketInfo::unk_C20FA8));
}
