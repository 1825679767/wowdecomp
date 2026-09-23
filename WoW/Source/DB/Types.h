#ifndef WOW_SOURCE_DB_TYPES_H
#define WOW_SOURCE_DB_TYPES_H

#include <cstddef>
#include <cstdint>

struct WoWClientDB {
    /* 0000 */ void* m_funcTable;
    /* 0004 */ int32_t m_isLoaded;
    /* 0008 */ int32_t m_numRows;
    /* 000C */ int32_t m_maxIndex;
    /* 0010 */ int32_t m_minIndex;
    /* 0014 */ int32_t m_stringTable;
    /* 0018 */ void* m_funcTable2;
    /* 001C */ int32_t* m_firstRow;
    /* 0020 */ int32_t* m_rows;
};

#endif
