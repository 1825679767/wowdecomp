#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <windows.h>
#include "SCmd.h"
#include "Storm/H/stpl.h"
#include "Storm/H/storm.h"

// STATIC: 0x00A39F84
static const char* const s_errorstr[3] = {
    "Invalid argument: %s",
    "The syntax of the command is incorrect.",
    "Unable to open response file: %s",
};

// STATIC: 0x00ADE8A0
static TSList<CMDDEF> s_arglist;

// STATIC: 0x00ADE8AC
static TSList<CMDDEF> s_flaglist;

// STATIC: 0x00CAE944
static int32_t s_addedoptional;

static int32_t ProcessString(const char** stringptr, _PROCESSING* processing, CMDDEF** nextarg, int32_t (STORMAPI* extracallback)(const char*), void (STORMAPI* errorcallback)(_CMDERROR*));

// VERIFY: 2ffd6149 manual
// OFFSET: 0x00772B30
static void ConvertBool(CMDDEF* ptr, const char* string, int32_t* datachars) {
    int32_t set;

    if (*string == '-') {
        set = 0;
        *datachars = 1;
    } else if (*string == '+') {
        set = 1;
        *datachars = 1;
    } else {
        set = (ptr->flags & 1) != 1;
    }

    ptr->currvalue &= ~ptr->setmask;

    if (set) {
        ptr->currvalue |= ptr->setvalue;
    }

    if (ptr->variableptr) {
        *static_cast<uint32_t*>(ptr->variableptr) &= ~ptr->setmask;

        if (set) {
            *static_cast<uint32_t*>(ptr->variableptr) |= ptr->setvalue;
        }
    }
}

// VERIFY: 371a4de5 manual
// OFFSET: 0x00772BA0
static void ConvertNumber(CMDDEF* ptr, const char* string, int32_t* datachars) {
    char* endptr = nullptr;

    if (ptr->flags & 1) {
        ptr->currvalue = strtol(string, &endptr, 0);
    } else {
        ptr->currvalue = strtoul(string, &endptr, 0);
    }

    if (endptr) {
        *datachars = static_cast<int32_t>(endptr - string);
    } else {
        *datachars = SStrLen(string);
    }

    if (ptr->variableptr) {
        memcpy(ptr->variableptr, &ptr->currvalue, ptr->variablebytes > sizeof(uint32_t) ? sizeof(uint32_t) : ptr->variablebytes);
    }
}

// VERIFY: bfa55d35 manual
// OFFSET: 0x00772C20
static void ConvertString(CMDDEF* ptr, const char* string, int32_t* datachars) {
    *datachars = SStrLen(string);

    if (ptr->currvaluestr) {
        SMemFree(ptr->currvaluestr, __FILE__, __LINE__, 0);
    }

    ptr->currvaluestr = static_cast<char*>(SMemAlloc(SStrLen(string) + 1, __FILE__, __LINE__, 0));
    SStrCopy(ptr->currvaluestr, string, 0x7FFFFFFF);

    if (ptr->variableptr) {
        SStrCopy(static_cast<char*>(ptr->variableptr), string, ptr->variablebytes);
    }
}

// VERIFY: 65c3005a manual
// OFFSET: 0x00772C90
static void GenerateError(void (STORMAPI* errorcallback)(_CMDERROR*), uint32_t errorcode, const char* itemstring) {
    uint32_t index;
    UINT stringid;

    switch (errorcode) {
        case 0x85100065:
            index = 0;
            stringid = 0x5201;
            break;
        case 0x8510006D:
            index = 1;
            stringid = 0x5202;
            break;
        case ERROR_OPEN_FAILED:
            index = 2;
            stringid = 0x5203;
            break;
        default:
            return;
    }

    char buffer[256];
    buffer[0] = 0;
    LoadStringA(StormGetInstance(), stringid, buffer, sizeof(buffer));

    if (!buffer[0]) {
        SStrCopy(buffer, s_errorstr[index], sizeof(buffer));
    }

    char errorstr[256];

    if (strstr(buffer, "%s")) {
        wsprintfA(errorstr, buffer, itemstring);
    } else {
        SStrCopy(errorstr, buffer, sizeof(errorstr));
    }

    if (errorstr[0]) {
        SStrPack(errorstr, "\n", sizeof(errorstr));
    }

    SErrSetLastError(errorcode);

    _CMDERROR data;
    data.errorcode = errorcode;
    data.itemstr = itemstring;
    data.errorstr = errorstr;
    errorcallback(&data);
}

// VERIFY: bad4e87b manual
// OFFSET: 0x00772DA0
static int32_t ProcessFile(const char* filename, _PROCESSING* processing, CMDDEF** nextarg, int32_t (STORMAPI* extracallback)(const char*), void (STORMAPI* errorcallback)(_CMDERROR*)) {
    HANDLE file = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr);

    if (file == INVALID_HANDLE_VALUE) {
        if (errorcallback) {
            GenerateError(errorcallback, ERROR_OPEN_FAILED, filename);
        }

        return 0;
    }

    DWORD size = GetFileSize(file, nullptr);
    char* data = static_cast<char*>(SMemAlloc(size + 1, __FILE__, __LINE__, 0));
    DWORD bytesread;
    ReadFile(file, data, size, &bytesread, nullptr);
    CloseHandle(file);
    data[bytesread] = 0;

    const char* curr = data;
    int32_t result = ProcessString(&curr, processing, nextarg, extracallback, errorcallback);
    SMemFree(data, __FILE__, __LINE__, 0);

    return result;
}

// VERIFY: 32d9d033 manual
// OFFSET: 0x00772E70
static CMDDEF* FindFlagDef(const char* string, CMDDEF* firstdef, int32_t minlength) {
    int32_t strlength = SStrLen(string);
    int32_t bestlength = minlength - 1;
    CMDDEF* bestptr = nullptr;

    for (CMDDEF* ptr = firstdef; ptr; ptr = ptr->Next()) {
        if (ptr->namelength > bestlength && ptr->namelength <= strlength) {
            int32_t match;

            if (ptr->flags & 0x100) {
                match = !strncmp(ptr->name, string, ptr->namelength);
            } else {
                match = !_strnicmp(ptr->name, string, ptr->namelength);
            }

            if (match) {
                bestlength = ptr->namelength;
                bestptr = ptr;
            }
        }
    }

    return bestptr;
}

// VERIFY: a4c6f104 manual
// OFFSET: 0x00772F60
static int32_t PerformConversion(CMDDEF* ptr, const char* string, int32_t* datachars) {
    *datachars = 0;

    switch (ptr->flags & 0x30000) {
        case 0:
            ConvertBool(ptr, string, datachars);
            break;
        case 0x10000:
            ConvertNumber(ptr, string, datachars);
            break;
        case 0x20000:
            ConvertString(ptr, string, datachars);
            break;
        default:
            return 0;
    }

    ptr->found = 1;

    if (ptr->callback) {
        _CMDPARAMS params;
        params.flags = ptr->flags;
        params.id = ptr->id;
        params.name = ptr->name;
        params.variable = ptr->variableptr;
        params.setvalue = ptr->setvalue;
        params.setmask = ptr->setmask;
        params.stringvalue = ptr->currvaluestr;

        if (!ptr->callback(&params, string)) {
            return 0;
        }
    }

    for (int32_t i = 0; i <= 1; i++) {
        TSList<CMDDEF>* list = i ? &s_flaglist : &s_arglist;

        for (CMDDEF* curr = list->Head(); !TSList<CMDDEF>::IsEnd(curr); curr = list->RawNext(curr)) {
            if (curr->id == ptr->id && (curr->flags & 0x30000) == (ptr->flags & 0x30000) && curr != ptr) {
                curr->found = 1;

                if ((curr->flags & 0x30000) == 0x20000) {
                    if (curr->currvaluestr) {
                        SMemFree(curr->currvaluestr, __FILE__, __LINE__, 0);
                    }

                    curr->currvaluestr = static_cast<char*>(SMemAlloc(SStrLen(ptr->currvaluestr) + 1, __FILE__, __LINE__, 0));
                    SStrCopy(curr->currvaluestr, ptr->currvaluestr, 0x7FFFFFFF);
                } else {
                    curr->currvalue = ptr->currvalue;
                }
            }
        }
    }

    return 1;
}

// VERIFY: 10be8097 manual
// OFFSET: 0x007730E0
static int32_t ProcessCurrentFlag(const char* string, _PROCESSING* processing, int32_t* datachars) {
    *datachars = 0;

    CMDDEF* ptr = processing->ptr;
    processing->ptr = nullptr;

    while (ptr) {
        int32_t currchars;

        if (!PerformConversion(ptr, string, &currchars)) {
            return 0;
        }

        *datachars = *datachars > currchars ? *datachars : currchars;
        ptr = FindFlagDef(processing->name, ptr->Next(), processing->namelength);
    }

    return 1;
}

// VERIFY: 811be472 manual
// OFFSET: 0x00773160
static int32_t ProcessFlags(const char* string, _PROCESSING* processing, void (STORMAPI* errorcallback)(_CMDERROR*)) {
    char lastflag[256];
    lastflag[0] = 0;

    while (*string) {
        int32_t strlength = SStrLen(string);
        int32_t namelength = 1 > SStrLen(lastflag) ? 1 : SStrLen(lastflag);
        CMDDEF* flagdef = nullptr;

        while (namelength--) {
            if (strlength + namelength < static_cast<int32_t>(sizeof(lastflag))) {
                SStrCopy(&lastflag[namelength], string, sizeof(lastflag));
                flagdef = FindFlagDef(lastflag, s_flaglist.Head(), 0);

                if (flagdef) {
                    namelength = flagdef->namelength;
                    lastflag[namelength] = 0;
                    break;
                }
            }
        }

        if (!flagdef) {
            if (errorcallback) {
                GenerateError(errorcallback, 0x85100065, string);
            }

            return 0;
        }

        const char* curr = &string[namelength];

        while (SStrChr("=:", *curr)) {
            curr++;
        }

        processing->ptr = flagdef;
        processing->namelength = namelength;
        SStrCopy(processing->name, lastflag, 0x7FFFFFFF);

        if (!*curr && (flagdef->flags & 0x30000)) {
            flagdef->found = 1;
            return 1;
        }

        int32_t datachars;

        if (!ProcessCurrentFlag(curr, processing, &datachars)) {
            return 0;
        }

        string = curr + datachars;
    }

    return 1;
}

// VERIFY: 1b062e8d manual
// OFFSET: 0x007732F0
static int32_t ProcessToken(const char* string, int32_t quoted, _PROCESSING* processing, CMDDEF** nextarg, int32_t (STORMAPI* extracallback)(const char*), void (STORMAPI* errorcallback)(_CMDERROR*)) {
    if (*string == '@' && !quoted) {
        return ProcessFile(string + 1, processing, nextarg, extracallback, errorcallback);
    }

    if (SStrChr("-/", *string) && !quoted) {
        processing->ptr = nullptr;
        return ProcessFlags(string + 1, processing, errorcallback);
    }

    int32_t datachars;

    if (processing->ptr) {
        return ProcessCurrentFlag(string, processing, &datachars);
    }

    if (*nextarg) {
        if (PerformConversion(*nextarg, string, &datachars)) {
            *nextarg = (*nextarg)->Next();
            return 1;
        }
    } else {
        if (extracallback) {
            return extracallback(string);
        }

        if (errorcallback) {
            GenerateError(errorcallback, 0x85100065, string);
        }
    }

    return 0;
}

// VERIFY: a65d2107 manual
// OFFSET: 0x007733D0
int32_t STORMAPI SCmdDestroy() {
    for (int32_t i = 0; i <= 1; i++) {
        TSList<CMDDEF>* list = i ? &s_flaglist : &s_arglist;

        for (CMDDEF* ptr = list->Head(); !TSList<CMDDEF>::IsEnd(ptr); ptr = list->RawNext(ptr)) {
            if ((ptr->flags & 0x30000) == 0x20000 && ptr->currvaluestr) {
                SMemFree(ptr->currvaluestr, __FILE__, __LINE__, 0);
                ptr->currvaluestr = nullptr;
            }
        }
    }

    s_arglist.Clear();
    s_flaglist.Clear();
    s_addedoptional = 0;

    return 1;
}

// VERIFY: f850c285 manual
// OFFSET: 0x00773460
uint32_t STORMAPI SCmdGetNum(uint32_t id) {
    for (int32_t i = 0; i <= 1; i++) {
        TSList<CMDDEF>* list = i ? &s_flaglist : &s_arglist;

        for (CMDDEF* ptr = list->Head(); !TSList<CMDDEF>::IsEnd(ptr); ptr = list->RawNext(ptr)) {
            if (ptr->id == id) {
                return ptr->currvalue;
            }
        }
    }

    return 0;
}

// VERIFY: 10d3c4a7 manual
// OFFSET: 0x007734C0
int32_t STORMAPI SCmdGetString(uint32_t id, char* buffer, uint32_t bufferchars) {
    if (!buffer) {
        DebugValidate("buffer");
        SErrSetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    *buffer = 0;

    if (!bufferchars) {
        DebugValidate("bufferchars");
        SErrSetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    for (int32_t i = 0; i <= 1; i++) {
        TSList<CMDDEF>* list = i ? &s_flaglist : &s_arglist;

        for (CMDDEF* ptr = list->Head(); !TSList<CMDDEF>::IsEnd(ptr); ptr = list->RawNext(ptr)) {
            if (ptr->id == id) {
                if (ptr->currvaluestr) {
                    SStrCopy(buffer, ptr->currvaluestr, bufferchars);
                }

                return 1;
            }
        }
    }

    return 0;
}

// INLINED
int32_t STORMAPI SCmdRegisterArgument(uint32_t flags, uint32_t id, const char* name, void* variableptr, uint32_t variablebytes, uint32_t setvalue, uint32_t setmask, int32_t (STORMAPI* callback)(_CMDPARAMS*, const char*)) {
    if (!name) {
        name = "";
    }

    int32_t namelength = SStrLen(name);

    if (namelength >= 16) {
        DebugValidate("namelength < 16");
        SErrSetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    if (variablebytes && !variableptr) {
        DebugValidate("(!variablebytes) || variableptr");
        SErrSetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    if ((flags & 0x3000000) == 0x2000000 && s_addedoptional) {
        DebugValidate("(((flags) & ((0 << 24) | (1 << 24) | (2 << 24))) != (2 << 24)) || (!s_addedoptional)");
        SErrSetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    if ((flags & 0x3000000) == 0 && namelength <= 0) {
        DebugValidate("(((flags) & ((0 << 24) | (1 << 24) | (2 << 24))) != (0 << 24)) || (namelength > 0)");
        SErrSetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    if ((flags & 0x30000) == 0 && variableptr && variablebytes != sizeof(DWORD)) {
        DebugValidate("(((flags) & ((0 << 16) | (1 << 16) | (2 << 16))) != (0 << 16)) || (!variableptr) || (variablebytes == sizeof(DWORD))");
        SErrSetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    CMDDEF* ptr = ((flags & 0x3000000) ? &s_arglist : &s_flaglist)->NewNode(2, 0, 0);
    SStrCopy(ptr->name, name, sizeof(ptr->name));
    ptr->id = id;
    ptr->namelength = namelength;
    ptr->variableptr = variableptr;
    ptr->variablebytes = variablebytes;
    ptr->flags = flags;
    ptr->setvalue = setvalue;
    ptr->setmask = setmask;
    ptr->callback = callback;

    if ((flags & 0x30000) || (flags & 1) != 1) {
        ptr->currvalue = 0;
    } else {
        ptr->currvalue = setvalue;
    }

    if ((flags & 0x3000000) == 0x1000000) {
        s_addedoptional = 1;
    }

    return 1;
}

// VERIFY: 39e1c13e manual
// OFFSET: 0x00773590
int32_t STORMAPI SCmdRegisterArgList(const _ARGLIST* listptr, uint32_t numargs) {
    if (!listptr) {
        DebugValidate("listptr");
        SErrSetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    for (uint32_t i = 0; i < numargs; i++) {
        if (!SCmdRegisterArgument(listptr[i].flags, listptr[i].id, listptr[i].name, nullptr, 0, 1, 0xFFFFFFFF, listptr[i].callback)) {
            return 0;
        }
    }

    return 1;
}

// VERIFY: 552a551b auto
// OFFSET: 0x007737D0
static int32_t ProcessString(const char** stringptr, _PROCESSING* processing, CMDDEF** nextarg, int32_t (STORMAPI* extracallback)(const char*), void (STORMAPI* errorcallback)(_CMDERROR*)) {
    while (**stringptr) {
        const char* nextptr = *stringptr;
        char buffer[256];
        int32_t quoted = 0;
        SStrTokenize(&nextptr, buffer, sizeof(buffer), " ,;\"\t\n\r\x1A", &quoted);

        if (buffer[0] && !ProcessToken(buffer, quoted, processing, nextarg, extracallback, errorcallback)) {
            break;
        }

        *stringptr = nextptr;
    }

    return **stringptr == 0;
}

// VERIFY: 47f5c91d auto
// OFFSET: 0x00773870
int32_t STORMAPI SCmdGetBool(uint32_t id) {
    return SCmdGetNum(id) != 0;
}

// VERIFY: 448e3761 manual
// OFFSET: 0x00773890
int32_t STORMAPI SCmdProcess(const char* cmdline, int32_t skipprogname, int32_t (STORMAPI* extracallback)(const char*), void (STORMAPI* errorcallback)(_CMDERROR*)) {
    if (!cmdline) {
        DebugValidate("cmdline");
        SErrSetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    if (skipprogname) {
        SStrTokenize(&cmdline, nullptr, 0, " ,;\"\t\n\r\x1A", nullptr);
    }

    _PROCESSING processing;
    memset(&processing, 0, sizeof(processing));

    CMDDEF* nextarg = s_arglist.Head();

    if (!ProcessString(&cmdline, &processing, &nextarg, extracallback, errorcallback)) {
        return 0;
    }

    int32_t result = 1;

    while (nextarg && result) {
        if ((nextarg->flags & 0x3000000) == 0x2000000) {
            result = 0;
        } else {
            nextarg = nextarg->Next();
        }
    }

    if (errorcallback && !result) {
        GenerateError(errorcallback, 0x8510006D, "");
    }

    return result;
}

// VERIFY: 6e0ffbb4 auto
// OFFSET: 0x00773990
int32_t STORMAPI SCmdProcessCommandLine(int32_t (STORMAPI* extracallback)(const char*), void (STORMAPI* errorcallback)(_CMDERROR*)) {
    return SCmdProcess(GetCommandLineA(), 1, extracallback, errorcallback);
}
