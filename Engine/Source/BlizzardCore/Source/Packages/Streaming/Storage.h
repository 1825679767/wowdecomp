#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_STREAMING_STORAGE_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_STREAMING_STORAGE_H

#include <cstddef>
#include <cstdint>

namespace Blizzard { namespace Storage { struct StorageUnit; }}

namespace Blizzard {
namespace File {
typedef int32_t INODE;
}
}

namespace Blizzard {
namespace Storage {
struct StorageUnitHandle {
    /* 0000 */ Blizzard::File::INODE m_mNode;
    /* 0004 */ Blizzard::Storage::StorageUnit* m_mUnit;
};
}
}

namespace Blizzard {
namespace Storage {
struct StorageUnit {
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ uint32_t unk_0020;
    /* 0024 */ uint32_t unk_0024;
    /* 0028 */ uint32_t unk_0028;
    /* 002C */ uint32_t unk_002C;
    /* 0030 */ uint32_t unk_0030;
    /* 0034 */ uint32_t unk_0034;
    /* 0038 */ uint32_t unk_0038;
};
}
}

#endif
