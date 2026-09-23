#ifndef ENGINE_SOURCE_BLIZZARDCORE_BLIZZARDCORE_SOURCE_SYSTEM_FILE_FILESTACK_WIN32_H
#define ENGINE_SOURCE_BLIZZARDCORE_BLIZZARDCORE_SOURCE_SYSTEM_FILE_FILESTACK_WIN32_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/BlizzardCore/Source/System/Memory/Types.h"

namespace Blizzard { namespace File { struct Filesystem; }}
namespace Blizzard { namespace File { struct ProcessDirParms; }}
namespace Blizzard { namespace File { struct StreamRecord; }}
namespace Blizzard { namespace File { struct StreamingInfo; }}
namespace System_File { namespace Stacked { struct FileParms; }}

namespace Blizzard {
namespace File {
struct FileInfo {
    /* 0000 */ char* m_path;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ int64_t m_size;
    /* 0010 */ int32_t m_attributes;
    /* 0014 */ uint8_t unk_0014[4];
    /* 0018 */ Blizzard::Time::Timestamp m_createTime;
    /* 0020 */ Blizzard::Time::Timestamp m_modTime;
    /* 0028 */ Blizzard::Time::Timestamp m_accessTime;
    /* 0030 */ int32_t m_existence;
    /* 0034 */ int32_t m_regularFile;
};
}
}

namespace Blizzard {
namespace File {
union FileInfoMask {
    /* 0000 */ uint8_t unk_0000[4];
    /* 0000 */ uint32_t m_all;
};
}
}

namespace System_File {
namespace Stacked {
typedef bool (__stdcall* FileFunc )(Blizzard::File::Filesystem* fs, System_File::Stacked::FileParms* parms);
}
}

namespace Blizzard {
namespace File {
struct Functions {
    /* 0000 */ System_File::Stacked::FileFunc m_cd;
    /* 0004 */ System_File::Stacked::FileFunc m_close;
    /* 0008 */ System_File::Stacked::FileFunc m_create;
    /* 000C */ System_File::Stacked::FileFunc m_cwd;
    /* 0010 */ System_File::Stacked::FileFunc m_dirwalk;
    /* 0014 */ System_File::Stacked::FileFunc m_exists;
    /* 0018 */ System_File::Stacked::FileFunc m_flush;
    /* 001C */ System_File::Stacked::FileFunc m_getfileinfo;
    /* 0020 */ System_File::Stacked::FileFunc m_getfreespace;
    /* 0024 */ System_File::Stacked::FileFunc m_getpos;
    /* 0028 */ System_File::Stacked::FileFunc m_getrootchars;
    /* 002C */ System_File::Stacked::FileFunc m_isabspath;
    /* 0030 */ System_File::Stacked::FileFunc m_isreadonly;
    /* 0034 */ System_File::Stacked::FileFunc m_makeabspath;
    /* 0038 */ System_File::Stacked::FileFunc m_mkdir;
    /* 003C */ System_File::Stacked::FileFunc m_move;
    /* 0040 */ System_File::Stacked::FileFunc m_copy;
    /* 0044 */ System_File::Stacked::FileFunc m_open;
    /* 0048 */ System_File::Stacked::FileFunc m_read;
    /* 004C */ System_File::Stacked::FileFunc m_readp;
    /* 0050 */ System_File::Stacked::FileFunc m_rmdir;
    /* 0054 */ System_File::Stacked::FileFunc m_setcachemode;
    /* 0058 */ System_File::Stacked::FileFunc m_seteof;
    /* 005C */ System_File::Stacked::FileFunc m_setfileinfo;
    /* 0060 */ System_File::Stacked::FileFunc m_setpos;
    /* 0064 */ System_File::Stacked::FileFunc m_unlink;
    /* 0068 */ System_File::Stacked::FileFunc m_write;
    /* 006C */ System_File::Stacked::FileFunc m_writep;
    /* 0070 */ System_File::Stacked::FileFunc m_shutdown;
};
}
}

namespace Blizzard {
namespace File {
struct Filesystem {
    /* 0000 */ Blizzard::File::Filesystem* m_original;
    /* 0004 */ Blizzard::File::Filesystem* m_next;
    /* 0008 */ Blizzard::File::Functions m_verbs;
};
}
}

namespace Blizzard {
namespace File {
typedef bool (__stdcall* ProcessDirCallback_interface )(const Blizzard::File::ProcessDirParms* parms);
}
}

namespace Blizzard {
namespace File {
typedef Blizzard::File::ProcessDirCallback_interface ProcessDirCallback;
}
}

namespace Blizzard {
namespace File {
typedef Blizzard::File::StreamRecord* Stream;
}
}

namespace Blizzard {
namespace File {
struct StreamRecord {
    /* 0000 */ void* m_fileHandle;
    /* 0004 */ int32_t m_mode;
    /* 0008 */ bool m_infoValid;
    /* 0009 */ uint8_t unk_0009[3];
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ Blizzard::File::FileInfo m_info;
    /* 0048 */ Blizzard::File::StreamingInfo* m_streaming;
    /* 004C */ char* m_pathname;
};
}
}

namespace offset_in_Blizzard {
namespace File {
namespace Functions_to_System_File {
namespace Stacked {
typedef uintptr_t FileFunc;
}
}
}
}

namespace System_File {
namespace Stacked {
struct FileParms {
    /* 0000 */ offset_in_Blizzard::File::Functions_to_System_File::Stacked::FileFunc m_verb;
    /* 0004 */ char* m_path;
    /* 0008 */ char* m_newpath;
    /* 000C */ Blizzard::File::Stream m_stream;
    /* 0010 */ Blizzard::File::FileInfo* m_info;
    /* 0014 */ uint32_t m_extra;
    /* 0018 */ Blizzard::File::FileInfo m_info2;
    /* 0050 */ Blizzard::File::FileInfoMask m_has;
    /* 0054 */ Blizzard::File::FileInfoMask m_want;
    /* 0058 */ int32_t m_mode;
    /* 005C */ void* m_buffer;
    /* 0060 */ int32_t m_amount;
    /* 0064 */ uint8_t unk_0064[4];
    /* 0068 */ int64_t m_pos;
    /* 0070 */ int32_t m_posMode;
    /* 0074 */ char* m_cwdbuf;
    /* 0078 */ int32_t m_cwdbuflen;
    /* 007C */ bool m_recursive;
    /* 007D */ bool m_makeCanonical;
    /* 007E */ uint8_t unk_007E[2];
    /* 0080 */ void* m_cookie;
    /* 0084 */ Blizzard::File::ProcessDirCallback m_dircallback;
    /* 0088 */ bool m_sorted;
    /* 0089 */ bool m_overwrite;
    /* 008A */ bool m_grantEveryone;
    /* 008B */ bool m_zeroFillAll;
    /* 008C */ bool m_supportsSparseFiles;
    /* 008D */ uint8_t unk_008D[3];
};
}
}

#endif
