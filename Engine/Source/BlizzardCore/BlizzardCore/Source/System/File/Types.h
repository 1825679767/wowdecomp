#ifndef ENGINE_SOURCE_BLIZZARDCORE_BLIZZARDCORE_SOURCE_SYSTEM_FILE_TYPES_H
#define ENGINE_SOURCE_BLIZZARDCORE_BLIZZARDCORE_SOURCE_SYSTEM_FILE_TYPES_H

#include <cstddef>
#include <cstdint>

namespace Blizzard {
namespace File {
struct ProcessDirParms {
    /* 0000 */ const char* m_pathRoot;
    /* 0004 */ const char* m_filename;
    /* 0008 */ void* m_cookie;
    /* 000C */ bool m_isDir;
    /* 000D */ uint8_t unk_000D[3];
};
}
}

#endif
