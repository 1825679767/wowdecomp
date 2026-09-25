#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <new>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/IOMpqUnit.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFile.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFileArchives.h"
#include "Engine/Source/BlizzardCore/Source/System/Debug/Debug.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Streaming.h"
#include "Engine/Source/BlizzardCore/Source/System/Debug/ErrorStack.h"
#include "Engine/Source/BlizzardCore/Source/System/File/File.h"
#include "Engine/Source/BlizzardCore/Source/System/Log/Log.h"
#include "Engine/Source/BlizzardCore/Source/System/Memory/MemoryStorm.h"
#include "Engine/Source/BlizzardCore/Source/System/String/Path.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Util/Hash.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00B32360
char SFile::m_basePath[260];

// STATIC: 0x00B32258
char System_SFile2::s_dataPath[260];

// STATIC: 0x00B32150
char System_SFile2::s_dataPath2[260];

// STATIC: 0x00AB7B7C
bool System_SFile2::s_checkDisk = true;

// STATIC: 0x00AB7B80
uint32_t System_SFile2::s_directAccess = 1;

// STATIC: 0x00B32040
uint32_t System_SFile2::s_useOldFindFile;

// STATIC: 0x00B32048
char System_SFile2::s_hashedBasePath[260];

// STATIC: 0x00B3214C
Blizzard::Lock::Event System_SFile2::s_asyncOpenEvent;

// STATIC: 0x00B32464
Blizzard::Lock::Mutex System_SFile2::s_soundCacheLock;

// STATIC: 0x00B3247C
Blizzard::Lock::Mutex System_SFile2::s_asyncOpenLock;

// STATIC: 0x00B32494
System_SFile::ARCHIVEREC* System_SFile2::s_soundCacheArchive;

// STATIC: 0x00B324A4
bool System_SFile2::s_fileHashBuilt;

// STATIC: 0x00B324A8
std::deque<std::string> System_SFile2::s_asyncOpenQueue;

// STATIC: 0x00B324BC
Blizzard::Lock::RMutex System_SFile2::s_fileHashLock;

// STATIC: 0x00B324D4
System_SFile2::FileHash System_SFile2::s_fileHash;

// STATIC: 0x00B324FC
Blizzard::Thread::LocalObject<System_SFile2::SFileFatalInfo> System_SFile2::s_fatalInfo;

// VERIFY: 99b101b5 manual
// OFFSET: 0x004216C0
static void BlizzardAssertCallback(const char* expression, const char* message, const char* fileName, uint32_t lineNumber) {
    if (*message) {
        SErrDisplayError(0, fileName, lineNumber, message, 0, 1, 0x11111111);
    } else {
        SErrDisplayError(0, fileName, lineNumber, expression, 0, 1, 0x11111111);
    }
}

// VERIFY: 8bc79d04 auto
// OFFSET: 0x00421700
int32_t SFile::Initialize() {
    Blizzard::Mopaq::SFileSetThreadingMode(2);
    Blizzard::Debug::SetAssertHandler(BlizzardAssertCallback);
    return 1;
}

void SFile::Destroy() {
}

// VERIFY: bd1bd823 auto
// OFFSET: 0x00421720
int32_t STORMAPI SFile::CloseArchive(SArchive* archive) {
    int32_t result = Blizzard::Mopaq::SFileCloseArchive(archive->m_archive);
    delete archive;
    return result;
}

// VERIFY: 23e2e3fe auto
// OFFSET: 0x00421750
void SFile::DisableSFileCheckDisk() {
    System_SFile2::s_checkDisk = false;
}

// VERIFY: 63e762ba auto
// OFFSET: 0x00421760
int32_t STORMAPI SFile::EnableDirectAccess(uint32_t flags) {
    System_SFile2::s_directAccess = (flags & 1) != 0;

    if (flags & 2) {
        System_SFile2::s_directAccess |= 2;
    }

    Blizzard::Mopaq::SFileEnableDirectAccess(flags);

    return 1;
}

// VERIFY: eb299239 auto
// OFFSET: 0x00421850
int32_t STORMAPI SFile::GetActualFileName(SFileImpl* file, char* buffer, uint32_t size) {
    const char* name = static_cast<const char*>(file->unk_0010);

    if (!name) {
        name = file->m_name;
    }

    Blizzard::String::Copy(buffer, name, size);

    return 1;
}

// VERIFY: 0594ecab auto
// OFFSET: 0x00421880
int32_t STORMAPI SFile::GetBasePath(char* path, uint32_t size) {
    Blizzard::String::Copy(path, SFile::m_basePath, size);
    return 1;
}

// VERIFY: e2371a6e auto
// OFFSET: 0x004218B0
const char* SFile::GetDataPath() {
    return System_SFile2::s_dataPath;
}

// VERIFY: 2f5c6797 manual
// OFFSET: 0x00421950
int32_t STORMAPI SFile::OpenArchive(const char* path, int32_t priority, uint32_t flags, SArchive** archive) {
    int32_t result = 0;
    Blizzard::File::SetLastErrorStack(nullptr);

    *archive = new (0u, __FILE__, __LINE__) SArchive;
    (*archive)->m_archive = nullptr;

    System_SFile::ARCHIVEREC* handle;

    if (Blizzard::String::EqualI(Blizzard::String::FindExtension(path), "mpq")) {
        (*archive)->unk_0000 = 0;
        result = Blizzard::Mopaq::SFileOpenArchive(path, priority, flags, &handle);
        (*archive)->m_archive = handle;

        if (result) {
            fprintf(stderr, "archive %s opened\n", path);
            return result;
        }
    }

    delete *archive;
    *archive = nullptr;
    SErrSetLastError(Blizzard::Mopaq::SFileGetLastError());

    return result;
}

// VERIFY: d9cadfcd manual
// OFFSET: 0x00421A10
int32_t STORMAPI SFile::OpenPathAsArchive(SArchive* archive, const char* path, int32_t priority, uint32_t flags, SArchive** result) {
    *result = new (0u, __FILE__, __LINE__) SArchive;
    (*result)->m_archive = nullptr;

    System_SFile::ARCHIVEREC* handle;
    int32_t opened = Blizzard::Mopaq::SFileOpenArchiveView(archive->m_archive, path, priority, &handle);
    (*result)->m_archive = handle;

    if (!opened) {
        delete *result;
        *result = nullptr;
    }

    return opened;
}

// VERIFY: dcbd9ac4 manual
// OFFSET: 0x00421A80
int32_t STORMAPI SFile::SetBasePath(const char* path) {
    Blizzard::String::Copy(SFile::m_basePath, path, sizeof(SFile::m_basePath));
    uint32_t length = Blizzard::String::Length(SFile::m_basePath);

    if (SFile::m_basePath[0]) {
        char last = SFile::m_basePath[length - 1];

        if (last != '\\' && last != '/') {
            Blizzard::String::Append(SFile::m_basePath, "\\", sizeof(SFile::m_basePath));
        }
    }

    Blizzard::Mopaq::SFileSetBasePath(path);

    return 1;
}

// VERIFY: b35a022c auto
// OFFSET: 0x00421AF0
int32_t STORMAPI SFile::SetDataPath(const char* path) {
    Blizzard::String::Copy(System_SFile2::s_dataPath, path, sizeof(System_SFile2::s_dataPath));
    uint32_t length = Blizzard::String::Length(System_SFile2::s_dataPath);

    if (System_SFile2::s_dataPath[0]) {
        char last = System_SFile2::s_dataPath[length - 1];

        if (last != '\\' && last != '/') {
            Blizzard::String::Append(System_SFile2::s_dataPath, "\\", sizeof(System_SFile2::s_dataPath));
        }
    }

    return 1;
}

// VERIFY: 2344e5e7 auto
// OFFSET: 0x00421B50
int32_t STORMAPI SFile::SetDataPathAlternate(const char* path) {
    Blizzard::String::Copy(System_SFile2::s_dataPath2, path, sizeof(System_SFile2::s_dataPath2));
    uint32_t length = Blizzard::String::Length(System_SFile2::s_dataPath2);

    if (System_SFile2::s_dataPath2[0]) {
        char last = System_SFile2::s_dataPath2[length - 1];

        if (last != '\\' && last != '/') {
            Blizzard::String::Append(System_SFile2::s_dataPath2, "\\", sizeof(System_SFile2::s_dataPath2));
        }
    }

    return 1;
}

// VERIFY: 3e9c3828 auto
// OFFSET: 0x00422130
int32_t SFile::IsStreamingMode() {
    return Blizzard::Streaming::IsStreamingMode();
}

// VERIFY: 2e559d16 auto
void STORMAPI SFile::SetFilePriority(SFileImpl* file, int32_t urgent, int32_t unused) {
}

// VERIFY: be859ae3 auto
// OFFSET: 0x00422150
const char* SFile::GetErrorDetails() {
    return Blizzard::Debug::ErrorStackAsText(Blizzard::File::GetLastErrorStack());
}

// INLINED
System_SFile2::SFileFatalInfo::SFileFatalInfo() {
    this->m_fatal = true;
}

// OFFSET: 0x00430600
size_t System_SFile2::FileHashCompare::operator()(const std::string& key) const {
    const char* str = key.c_str();

    if (str) {
        return Blizzard::Util::Hash32I(str, static_cast<uint32_t>(strlen(str)));
    }

    return Blizzard::Util::Hash32I(nullptr, 0);
}

// OFFSET: 0x00430580
bool System_SFile2::FileHashCompare::operator()(const std::string& key1, const std::string& key2) const {
    const char* str1 = key1.c_str();
    const char* str2 = key2.c_str();

    if (str1 == str2) {
        return false;
    }

    if (!str1) {
        return true;
    }

    if (str2) {
        return _stricmp(str1, str2) < 0;
    }

    return false;
}

// VERIFY: 3c5a19b0 manual
// OFFSET: 0x00421BB0
int32_t STORMAPI SFile::SetFilePointer(SFileImpl* file, int32_t distance, int32_t* distanceHigh, uint32_t method) {
    int32_t result = 0;
    Blizzard::Lock::MutexEnter(file->m_lock->m_mutex);

    switch (file->m_type) {
        case 0: {
            int32_t whence = method;

            if (method >= 2) {
                whence = method == 2 ? 2 : -1;
            }

            Blizzard::File::SetPos(file->m_stream, distance, whence);

            int64_t position;

            if (Blizzard::File::GetPos(file->m_stream, &position)) {
                result = static_cast<int32_t>(position);
            }

            break;
        }

        case 2:
        case 3: {
            result = Blizzard::Mopaq::SFileSetFilePointer(file->m_file, distance, distanceHigh, method);
            break;
        }

        case 5: {
            if (method == 0) {
                file->m_position = distance;
            } else if (method == 1) {
                file->m_position += distance;
            } else if (method == 2) {
                file->m_position = file->m_size - distance;
            }

            result = file->m_position;
            break;
        }

        default: {
            break;
        }
    }

    Blizzard::Lock::MutexLeave(file->m_lock->m_mutex);

    return result;
}

// VERIFY: 8ae9fd4a auto
// OFFSET: 0x00421CA0
int32_t STORMAPI SFile::Unload(void* data) {
    Blizzard::Memory::Free(data);
    return 1;
}

// VERIFY: d0fc3ff9 auto
// OFFSET: 0x00421CC0
bool System_SFile2::OldFindFile(const char* name, char* path, uint32_t size, uint32_t flags, uint32_t* type) {
    Blizzard::File::FileInfo info;

    Blizzard::String::Copy(path, SFile::m_basePath, size);
    Blizzard::String::Append(path, name, size);

    if (Blizzard::File::GetFileInfo(path, &info) && info.m_regularFile) {
        *type = 0;
        return true;
    }

    const char* filename = Blizzard::String::FindReverse(name, '\\', 0x7FFFFFFF);

    if (filename) {
        Blizzard::String::Copy(path, SFile::m_basePath, size);
        Blizzard::String::Append(path, filename + 1, size);

        if (Blizzard::File::GetFileInfo(path, &info) && info.m_regularFile) {
            *type = 0;
            return true;
        }
    }

    Blizzard::String::Copy(path, SFile::m_basePath, size);
    Blizzard::String::Append(path, s_dataPath, size);
    Blizzard::String::Append(path, name, size);

    if (Blizzard::File::GetFileInfo(path, &info)) {
        if (info.m_regularFile) {
            *type = 0;
            return true;
        }

        return false;
    }

    Blizzard::String::Append(path, ".bz", size);

    if (Blizzard::File::GetFileInfo(path, &info)) {
        if (info.m_regularFile) {
            *type = 1;
            return true;
        }

        return false;
    }

    if (SFile::m_basePath[0]) {
        Blizzard::String::Copy(path, s_dataPath, size);
        Blizzard::String::Append(path, name, size);

        if (Blizzard::File::GetFileInfo(path, &info) && info.m_regularFile) {
            *type = 0;
            return true;
        }

        Blizzard::String::Append(path, ".bz", size);

        if (Blizzard::File::GetFileInfo(path, &info) && info.m_regularFile) {
            *type = 1;
            return true;
        }
    }

    Blizzard::String::Copy(path, SFile::m_basePath, size);
    Blizzard::String::Append(path, s_dataPath, size);
    Blizzard::String::Append(path, name, size);
    Blizzard::String::Append(path, ".MPQ", size);

    if (Blizzard::File::GetFileInfo(path, &info)) {
        if (info.m_regularFile) {
            *type = 2;
            return true;
        }

        return false;
    }

    if (s_dataPath2[0]) {
        Blizzard::String::Copy(path, SFile::m_basePath, size);
        Blizzard::String::Append(path, s_dataPath2, size);
        Blizzard::String::Append(path, name, size);

        if (Blizzard::File::GetFileInfo(path, &info)) {
            if (info.m_regularFile) {
                *type = 0;
                return true;
            }

            return false;
        }
    }

    if (!s_checkDisk) {
        flags &= ~3u;
    }

    if (Blizzard::Mopaq::SFileFileExists(nullptr, name, flags, nullptr)) {
        Blizzard::String::Copy(path, name, size);
        *type = 3;
        return true;
    }

    SErrSetLastError(2);

    return false;
}

// VERIFY: ad785843 auto
// OFFSET: 0x00421FC0
int32_t STORMAPI SFile::FileExists(const char* name) {
    int32_t result = Blizzard::Mopaq::SFileFileExists(nullptr, name, 0, nullptr);

    if (result) {
        SErrSetLastError(2);
    }

    return result;
}

// VERIFY: 43a7d076 manual
// OFFSET: 0x00422330
SFileImpl::SFileImpl(uint32_t type) {
    this->m_lock = new (0u, __FILE__, __LINE__) SFileLock();
    this->m_lock->unk_0018 = 0;
    this->m_stream = nullptr;
    this->m_archive = nullptr;
    this->m_name = nullptr;
    this->m_data = nullptr;
    this->m_file = nullptr;
    this->unk_0024 = 0;
    this->unk_0010 = nullptr;
    this->unk_0028 = 0;
    this->m_type = type;
}

// INLINED
SFileImpl::~SFileImpl() {
    Blizzard::Memory::Free(this->m_name);
    Blizzard::Memory::Free(this->unk_0010);
    Blizzard::Memory::Free(this->m_data);

    if (this->m_lock) {
        delete this->m_lock;
    }
}

// INLINED
SFileLock::SFileLock() {
    Blizzard::Lock::MutexCreate(this->m_mutex);
}

// INLINED
SFileLock::~SFileLock() {
    Blizzard::Lock::MutexDestroy(this->m_mutex);
}

// VERIFY: 9e6a9095 manual
// OFFSET: 0x00422530
int32_t STORMAPI SFileReadFile(SFileImpl* file, void* buffer, uint32_t count, uint32_t* bytesRead, SOVERLAPPED* overlapped, uint32_t unused) {
    Blizzard::File::SetLastErrorStack(nullptr);

    if (overlapped) {
        return 0;
    }

    if (!count) {
        if (bytesRead) {
            *bytesRead = 0;
        }

        return 1;
    }

    switch (file->m_type) {
        case 0: {
            Blizzard::Lock::MutexEnter(file->m_lock->m_mutex);

            int32_t read = count;

            if (Blizzard::File::Read(file->m_stream, buffer, &read)) {
                if (bytesRead) {
                    *bytesRead = read;
                }

                if (read == static_cast<int32_t>(count)) {
                    Blizzard::Lock::MutexLeave(file->m_lock->m_mutex);
                    return 1;
                }
            }

            SErrSetLastError(38);
            Blizzard::Lock::MutexLeave(file->m_lock->m_mutex);

            return 0;
        }

        case 1:
        case 2:
        case 3: {
            int32_t result = 0;
            uint32_t read = 0;

            Blizzard::Lock::MutexEnter(file->m_lock->m_mutex);
            Blizzard::File::SetLastError(0);

            if (Blizzard::Mopaq::SFileReadFile(file->m_file, buffer, count, &read)) {
                if (bytesRead) {
                    *bytesRead = read;
                }

                if (read == count) {
                    result = 1;
                    Blizzard::Lock::MutexLeave(file->m_lock->m_mutex);
                    return result;
                }
            } else {
                bool fatal = static_cast<System_SFile2::SFileFatalInfo&>(System_SFile2::s_fatalInfo).m_fatal;
                Blizzard::Debug::ErrorStackRecord* stack = Blizzard::File::GetLastErrorStack();

                if ((!stack || !stack->FindCategoryCode(1, 10)) && fatal) {
                    bool stormError = SErrGetLastError() != 0;
                    char message[256];
                    SErrGetErrorStr(SErrGetLastError(), message, sizeof(message));

                    if (stack->FindCategory(21)) {
                        const char* format = stormError ? "Failed to read data from the network. Please check your Internet connection and try again.\n\nDebug Details:\n\n%sStorm Error Msg:%s" : "Failed to read data from the network. Please check your Internet connection and try again.\n\nDebug Details:\n\n%s";
                        SErrDisplayAppFatal(format, Blizzard::Debug::ErrorStackAsText(stack), message);
                    }

                    const char* format = stormError ? "Failed to read file %s.\n\nDebug Details:\n\n%sStorm Error Msg:%s" : "Failed to read file %s.\n\nDebug Details:\n\n%s";
                    SErrDisplayAppFatal(format, file->m_name, Blizzard::Debug::ErrorStackAsText(stack), message);
                }
            }

            SErrSetLastError(38);
            Blizzard::Lock::MutexLeave(file->m_lock->m_mutex);

            return result;
        }

        case 4:
        case 5: {
            uint32_t available = file->m_position >= file->m_size ? 0 : file->m_size - file->m_position;
            uint32_t read = std::min(available, count);

            if (read) {
                Blizzard::String::MemCopy(buffer, file->m_data + file->m_position, read);
                file->m_position += read;
            }

            if (bytesRead) {
                *bytesRead = read;
            }

            if (read != count) {
                SErrSetLastError(38);
            }

            return read == count;
        }

        default: {
            return 0;
        }
    }
}

// VERIFY: 9f1580de auto
// OFFSET: 0x00422910
int32_t STORMAPI SFileCloseFile(SFileImpl* file) {
    if (file) {
        if (file->m_stream) {
            Blizzard::File::Close(file->m_stream);
        }

        if (file->m_file) {
            Blizzard::Mopaq::SFileCloseFile(file->m_file);
        }

        if (file->m_archive) {
            if (file->m_archive->m_archive) {
                Blizzard::Mopaq::SFileCloseArchive(file->m_archive->m_archive);
            }

            delete file->m_archive;
        }

        delete file;
    }

    return 1;
}

// VERIFY: 6c6892e3 manual
// OFFSET: 0x004218C0
uint32_t STORMAPI SFileGetFileSize(SFileImpl* file, uint32_t* sizeHigh) {
    switch (file->m_type) {
        case 0: {
            Blizzard::File::FileInfo info;

            if (!file->m_stream || !Blizzard::File::GetFileInfo(file->m_stream, &info)) {
                return 0;
            }

            if (sizeHigh) {
                *sizeHigh = static_cast<uint32_t>(info.m_size >> 32);
            }

            return static_cast<uint32_t>(info.m_size);
        }

        case 1:
        case 5: {
            return file->m_size;
        }

        case 2:
        case 3: {
            return Blizzard::Mopaq::SFileGetFileSize(file->m_file, sizeHigh);
        }

        default: {
            return 0;
        }
    }
}

// VERIFY: 4868457e manual
// OFFSET: 0x00423900
bool System_SFile2::QueueAsyncOpen(const char* name) {
    Blizzard::Lock::MutexEnter(s_asyncOpenLock);
    s_asyncOpenQueue.push_back(std::string(name, strlen(name)));
    Blizzard::Lock::MutexLeave(s_asyncOpenLock);
    Blizzard::Lock::EventSet(s_asyncOpenEvent);

    return true;
}

// VERIFY: 70694f87 manual
// OFFSET: 0x00423D70
void SFile::RebuildHash() {
    Blizzard::Lock::MutexEnter(System_SFile2::s_fileHashLock.m_mutex);
    System_SFile2::s_fileHash.clear();
    System_SFile2::s_fileHashBuilt = false;
    Blizzard::Lock::MutexLeave(System_SFile2::s_fileHashLock.m_mutex);
}

// VERIFY: b07104c0 manual
// OFFSET: 0x00424610
bool System_SFile2::ScanDirForFileSystemHash(const char* directory) {
    char path[512];
    Blizzard::String::MakeBackslashPath(directory, path, sizeof(path));
    Blizzard::String::ForceTrailingSeparator(path, sizeof(path), '\\');

    ScanDirCookie cookie;
    cookie.unk_0000 = 1;
    cookie.m_root = path;
    cookie.m_rootLength = Blizzard::String::Length(path);
    cookie.m_prefix = "";

    return Blizzard::File::ProcessDirFast(path, &cookie, Blizzard::Streaming::ScanDirectoryForManifest, false);
}

// VERIFY: a965489d auto
// OFFSET: 0x00424690
void System_SFile2::BuildFileSystemHash() {
    Blizzard::String::Copy(s_hashedBasePath, SFile::m_basePath, sizeof(s_hashedBasePath));

    const char* root = SFile::m_basePath;

    if (!Blizzard::File::IsDirectory(SFile::m_basePath)) {
        root = ".";
    }

    if (Blizzard::File::IsDirectory(root)) {
        ScanDirForFileSystemHash(root);
    }

    char path[512];
    Blizzard::String::Copy(path, root, sizeof(path));
    Blizzard::String::Append(path, s_dataPath, sizeof(path));

    if (Blizzard::File::IsDirectory(path)) {
        ScanDirForFileSystemHash(path);
    }

    if (s_dataPath2[0]) {
        Blizzard::String::Copy(path, s_dataPath2, sizeof(path));

        if (Blizzard::File::IsDirectory(path)) {
            ScanDirForFileSystemHash(path);
        }
    }

    s_fileHashBuilt = true;
    Blizzard::Log::Flush(0);
}

// VERIFY: f3281a6b manual
// OFFSET: 0x00424780
bool System_SFile2::FindFile(const char* name, char* path, uint32_t size, uint32_t flags, uint32_t* type, System_SFile::ARCHIVEREC** archive) {
    flags |= s_directAccess;

    if (archive) {
        *archive = nullptr;
    }

    if (s_useOldFindFile || (name[0] == '\\' && name[1] == '\\') || name[1] == ':') {
        return OldFindFile(name, path, size, flags, type);
    }

    if (flags & 3) {
        bool found = false;
        Blizzard::Lock::MutexEnter(s_fileHashLock.m_mutex);

        if (!s_fileHashBuilt) {
            Blizzard::Lock::MutexLeave(s_fileHashLock.m_mutex);
            Blizzard::Lock::MutexEnter(s_fileHashLock.m_mutex);
            BuildFileSystemHash();
            Blizzard::Lock::MutexLeave(s_fileHashLock.m_mutex);
            Blizzard::Lock::MutexEnter(s_fileHashLock.m_mutex);
        }

        FileHash::iterator it = s_fileHash.end();

        if (flags & 2) {
            it = s_fileHash.find(std::string(Blizzard::String::FindFilename(name)));
        }

        if (it == s_fileHash.end()) {
            it = s_fileHash.find(std::string(name));

            if (it == s_fileHash.end() && !Blizzard::String::Equal(s_hashedBasePath, SFile::m_basePath)) {
                char fullPath[260];
                Blizzard::String::Copy(fullPath, SFile::m_basePath, sizeof(fullPath));
                Blizzard::String::Append(fullPath, name, sizeof(fullPath));
                it = s_fileHash.find(std::string(fullPath));
            }
        }

        if (it != s_fileHash.end()) {
            Blizzard::String::Copy(path, it->second.m_path.c_str(), size);
            *type = it->second.m_type;
            found = true;
        }

        Blizzard::Lock::MutexLeave(s_fileHashLock.m_mutex);

        if (found) {
            return true;
        }
    }

    if (!s_checkDisk) {
        flags &= ~3u;
    }

    System_SFile::ARCHIVEREC* found;

    if (Blizzard::Mopaq::SFileFileExists(nullptr, name, flags, &found)) {
        if (archive) {
            *archive = found;
        }

        Blizzard::String::Copy(path, name, size);
        *type = 3;

        return true;
    }

    return false;
}

// VERIFY: 8c71c618 manual
// OFFSET: 0x00424B50
int32_t STORMAPI SFile::OpenEx(SArchive* archive, const char* name, uint32_t flags, SFileImpl** result) {
    Blizzard::File::SetLastErrorStack(nullptr);

    if (CheckIfStreamingAndTrial() && System_SFile2::s_soundCacheArchive && Blizzard::String::FindI(name, ".wav", 0x7FFFFFFF)) {
        *result = new (0u, __FILE__, __LINE__) SFileImpl(3);

        Blizzard::Lock::MutexEnter(System_SFile2::s_soundCacheLock);
        int32_t opened = Blizzard::Mopaq::SFileOpenFileEx(System_SFile2::s_soundCacheArchive, name, 0, &(*result)->m_file);
        Blizzard::Lock::MutexLeave(System_SFile2::s_soundCacheLock);

        if (opened) {
            return 1;
        }

        if (*result) {
            delete *result;
        }

        *result = nullptr;
        System_SFile2::QueueAsyncOpen(name);
    }

    *result = nullptr;

    System_SFile::ARCHIVEREC* handle = nullptr;
    uint32_t type;
    char path[260];

    if (!archive) {
        if (!System_SFile2::FindFile(name, path, sizeof(path), flags, &type, &handle)) {
            SErrSetLastError(2);
            return 0;
        }
    } else if (archive->unk_0000 == 0) {
        handle = archive->m_archive;
        type = 3;
    } else if (archive->unk_0000 == 1) {
        handle = archive->m_archive;
        type = 4;
    } else {
        return 0;
    }

    int32_t opened = 0;
    *result = new (0u, __FILE__, __LINE__) SFileImpl(type);

    switch (type) {
        case 0: {
            if (!Blizzard::File::Open(path, 0x1005, (*result)->m_stream)) {
                goto fail;
            }

            (*result)->m_name = Blizzard::String::Duplicate(path);
            opened = 2;
            break;
        }

        case 1:
        case 2:
        case 4: {
            break;
        }

        case 3: {
            if (!Blizzard::Mopaq::SFileOpenFileEx(handle, name, handle ? 0 : flags, &(*result)->m_file)) {
                goto fail;
            }

            (*result)->m_name = Blizzard::String::Duplicate(name);
            opened = 1;
            break;
        }

        default: {
            return 0;
        }
    }

    if (!(flags & 0x20000)) {
        return opened;
    }

    (*result)->m_size = SFileGetFileSize(*result, nullptr);
    (*result)->m_data = static_cast<uint8_t*>(Blizzard::Memory::Allocate((*result)->m_size, 0, __FILE__, __LINE__));

    if (!SFileReadFile(*result, (*result)->m_data, (*result)->m_size, nullptr, nullptr, 0)) {
        goto fail;
    }

    (*result)->m_type = 5;
    (*result)->m_position = 0;

    return opened;

fail:
    if (*result) {
        delete *result;
    }

    *result = nullptr;

    return 0;
}

// VERIFY: 6dee9c33 manual
// OFFSET: 0x00424E80
int32_t STORMAPI SFile::ReadFile(SArchive* archive, const char* name, void** data, uint32_t* size, uint32_t extra, uint32_t flags, SOVERLAPPED* overlapped) {
    SFileImpl* file;
    int32_t result = SFile::OpenEx(archive, name, flags, &file);

    if (!result) {
        return result;
    }

    uint32_t fileSize = SFileGetFileSize(file, nullptr);
    uint8_t* buffer = static_cast<uint8_t*>(Blizzard::Memory::Allocate(fileSize + extra));

    if (overlapped) {
        file->unk_0024 = 1;
    }

    if (!SFileReadFile(file, buffer, fileSize, size, overlapped, 0)) {
        Blizzard::Memory::Free(buffer, 0, __FILE__, __LINE__);
        SFileCloseFile(file);
        return 0;
    }

    if (extra) {
        Blizzard::String::MemFill(buffer + fileSize, extra, 0);
    }

    *data = buffer;

    if (size) {
        *size = fileSize;
    }

    if (!overlapped) {
        SFileCloseFile(file);
    }

    return 1;
}

// VERIFY: 9a40aba6 auto
// OFFSET: 0x00424F50
int32_t STORMAPI SFile::LoadFile(const char* name, void** data, uint32_t* size, uint32_t extra, SOVERLAPPED* overlapped) {
    return SFile::ReadFile(nullptr, name, data, size, extra, 0, overlapped);
}

// VERIFY: e18d80b5 auto
// OFFSET: 0x00424F80
int32_t STORMAPI SFileOpenFile(const char* name, SFileImpl** result) {
    return SFile::OpenEx(nullptr, name, 0, result);
}
