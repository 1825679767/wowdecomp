#include "UIMacros.h"
#include "WoW/Source/Ui/ActionBarFrame.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/Object/ItemStats.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/PetInfo.h"
#include "WoW/Source/Ui/UIMacroOptions.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/AccountData.h"
#include <cstdlib>
#include <cstring>
#include <malloc.h>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Os/OsFile.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "Engine/Source/Base/ConvertUTF.h"
#include "WoW/Source/Ui/ContainerFrame.h"
#include "WoW/Source/Ui/SpellBookFrame.h"
#include "WoW/Source/Object/ObjectClient/Bag_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Ui/MacroConditions.h"

// STATIC: 0x00ACE2A0
static TSExplicitList<MACRONODE, offsetof(MACRONODE, m_dynamicLink)> s_dynamicMacros;

// STATIC: 0x00BEAE20
uint32_t CGUIMacros::m_sortedMacros[72];

// STATIC: 0x00BEAF68
static TSHashTable<MACRONODE, HASHKEY_NONE> s_macros;

// STATIC: 0x00ACE1E8
static const char* s_macroIconPath = "Interface\\Icons\\";

// STATIC: 0x00BEAF40
int32_t CGUIMacros::m_saveMacros;

// STATIC: 0x00BEAF44
uint32_t CGUIMacros::m_state;

// STATIC: 0x00BEAF48
const char* CGUIMacros::m_macroRunning;

// STATIC: 0x00BEAF4C
MACRONODE* CGUIMacros::m_macroButton;

// STATIC: 0x00BEAF50
uint32_t CGUIMacros::m_numMacros[2];

// STATIC: 0x00BEAF58
static uint32_t s_maxMacroID[2];

// STATIC: 0x00A0F834
static const uint32_t s_macroIDBase[2] = { 0, 0x1000000 };

// STATIC: 0x00BEAF90
static TSGrowableArray<char*> s_macroIcons;

// STATIC: 0x00BEAFA0
static TSGrowableArray<char*> s_macroItemIcons;

// STATIC: 0x00BEAF60
static uint16_t s_initCounter;

// STATIC: 0x00BEAF64
static uint32_t s_pendingLoads;

// VERIFY: 395f9dc1 auto
// OFFSET: 0x00564AB0
int32_t CGUIMacros::GetMacroIndexByID(uint32_t id) {
    for (int32_t i = 0; i < static_cast<int32_t>(sizeof(CGUIMacros::m_sortedMacros) / sizeof(CGUIMacros::m_sortedMacros[0])); i++) {
        if (CGUIMacros::m_sortedMacros[i] == id) {
            return i;
        }
    }

    return -1;
}

// VERIFY: 3cc88c82 auto
// OFFSET: 0x005660E0
void MACRONODE::IncrementActionBarCount() {
    if (this->m_dynamic && !this->m_actionBarCount) {
        s_dynamicMacros.LinkToTail(this);
    }

    this->m_actionBarCount++;
}

// VERIFY: 462f140c manual
// OFFSET: 0x00566110
void MACRONODE::DecrementActionBarCount() {
    this->m_actionBarCount--;

    if (this->m_dynamic && !this->m_actionBarCount) {
        s_dynamicMacros.UnlinkNode(this);
    }
}

// VERIFY: 83d22513 auto
// OFFSET: 0x00566160
void CGUIMacros::UpdateMacroSpells() {
    for (MACRONODE* macro = s_macros.m_fulllist.Head(); macro; macro = s_macros.m_fulllist.Next(macro)) {
        if (macro->m_spellID < 0 && macro->FindSpellID()) {
            CGActionBar::UpdateMacro(macro->m_id);
        }
    }
}

// VERIFY: e9bab52e manual
// OFFSET: 0x00566660
void CGUIMacros::UpdateDynamicMacros() {
    for (MACRONODE* macro = s_dynamicMacros.Head(); macro; macro = s_dynamicMacros.Next(macro)) {
        if (macro->FindSpellID()) {
            CGActionBar::UpdateMacro(macro->m_id);
        }
    }
}

// VERIFY: 67f7b9f9 manual
// OFFSET: 0x00566A70
MACRONODE* CGUIMacros::GetMacroByIndex(uint32_t index) {
    if (index < sizeof(CGUIMacros::m_sortedMacros) / sizeof(CGUIMacros::m_sortedMacros[0])) {
        return s_macros.Ptr(CGUIMacros::m_sortedMacros[index], HASHKEY_NONE());
    }

    return nullptr;
}

// VERIFY: 01debf71 auto
// OFFSET: 0x00566AA0
MACRONODE* CGUIMacros::GetMacroByID(uint32_t id) {
    return s_macros.Ptr(id, HASHKEY_NONE());
}

// VERIFY: 1c1a9d37 manual
// OFFSET: 0x00566AC0
void CGUIMacros::GetFullIconPath(uint32_t id, char* buffer, uint32_t size) {
    *buffer = 0;
    MACRONODE* macro = s_macros.Ptr(id, HASHKEY_NONE());

    if (!macro) {
        return;
    }

    if (macro->m_dynamic && !macro->m_actionBarCount) {
        macro->FindSpellID();
    }

    if (!SStrCmp(macro->m_icon, "INV_Misc_QuestionMark", STORM_MAX_STR)) {
        if (macro->m_itemID) {
            const char* path = ClientDBStringLookup(3);
            WOWGUID none = WOWGUID();
            ItemStats_C* stats = g_itemDBCache.GetRecord(macro->m_itemID, none, nullptr, nullptr, false);
            int32_t displayId = stats ? stats->m_displayId : 0;
            SStrPrintf(buffer, size, "%s%s%s", path, *path ? "\\" : "", CGItem_C::GetInventoryArt(displayId));
            return;
        }

        if (macro->m_spellID) {
            SpellRec spell;

            if (g_spellDB.GetRecord(macro->m_spellID, &spell)) {
                int32_t iconId = spell.m_spellIconID;

                if (spell.m_activeIconID) {
                    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(macro->m_spellFlags ? CGPetInfo::GetPet(0) : ClntObjMgrGetActivePlayer(), TYPEMASK_UNIT, __FILE__, __LINE__));

                    if (unit) {
                        uint32_t count = unit->GetAuraCount();

                        for (uint32_t i = 0; i < count; i++) {
                            if (unit->GetAuraSpellId(i) == spell.m_id && (unit->GetAuraFlags(i) & 0x10)) {
                                iconId = spell.m_activeIconID;
                                break;
                            }
                        }
                    }
                }

                SpellIconRec* icon = g_spellIconDB.GetRecord(iconId);

                if (icon) {
                    SStrCopy(buffer, icon->m_textureFilename, size);
                    return;
                }
            }
        }
    }

    SStrPrintf(buffer, size, "%s%s", s_macroIconPath, macro->m_icon);
}

// VERIFY: ee9d285c manual
// OFFSET: 0x00566330
MACRONODE* CGUIMacros::FindMacro(const char* name) {
    if (!name || !*name) {
        return nullptr;
    }

    for (MACRONODE* macro = s_macros.m_fulllist.Head(); macro; macro = s_macros.m_fulllist.Next(macro)) {
        if (!SStrCmpI(name, macro->m_name, STORM_MAX_STR)) {
            return macro;
        }
    }

    return nullptr;
}

// VERIFY: ebb01234 auto
// OFFSET: 0x0050F4D0
bool CGUIMacros::IsActionAllowed(uint32_t action) {
    return !CGUIMacros::m_macroButton || ((1 << action) & CGUIMacros::m_state);
}

// VERIFY: 2f0aa8c8 auto
// OFFSET: 0x00565AD0
void CGUIMacros::RemoveDynamicMacro(MACRONODE* macro) {
    s_dynamicMacros.UnlinkNode(macro);
}

// VERIFY: 4406f3d1 auto
// OFFSET: 0x00566140
void CGUIMacros::Initialize() {
    do {
        s_initCounter++;
    } while (!s_initCounter);

    InitializeMacroOptions();
    CGUIMacros::LoadIcons();
}

// VERIFY: 7d331447 auto
// OFFSET: 0x00567FD0
void CGUIMacros::LoadMacros() {
    s_pendingLoads = 0x30000;
    LoadAccountData(4, CGUIMacros::LoadMacrosCallback, reinterpret_cast<void*>(static_cast<uintptr_t>(s_initCounter | 0x10000)));
    LoadAccountData(5, CGUIMacros::LoadMacrosCallback, reinterpret_cast<void*>(static_cast<uintptr_t>(s_initCounter | 0x20000)));
}

// VERIFY: 1a2a9c35 auto
// OFFSET: 0x00564A20
static int32_t QSortIcons(const void* a, const void* b) {
    const char* nameB = *static_cast<char* const*>(b);
    const char* nameA = *static_cast<char* const*>(a);

    if (!SStrCmp(nameA, "INV_Misc_QuestionMark", STORM_MAX_STR)) {
        return -1;
    }

    if (!SStrCmp(nameB, "INV_Misc_QuestionMark", STORM_MAX_STR)) {
        return 1;
    }

    return SStrCmpI(nameA, nameB, STORM_MAX_STR);
}

// VERIFY: 2e63df8f auto
// OFFSET: 0x00564A80
static int32_t QSortMacrosByName(const void* a, const void* b) {
    return SStrCmpI((*static_cast<MACRONODE* const*>(a))->m_name, (*static_cast<MACRONODE* const*>(b))->m_name, STORM_MAX_STR);
}

// VERIFY: b423fecf manual
// OFFSET: 0x00565550
static void AddMacroIcon(const char* fileName) {
    char* ext = SStrChrR(const_cast<char*>(fileName), '.');

    if (!ext) {
        return;
    }

    if (!SStrCmpI(ext, ".blp", STORM_MAX_STR) || !SStrCmpI(ext, ".tga", STORM_MAX_STR)) {
        char* name = SStrDupA(fileName, __FILE__, __LINE__);
        name[ext - fileName] = '\0';
        *s_macroIcons.New() = name;
    }
}

// VERIFY: f420fc9d manual
// OFFSET: 0x005655C0
static void AddMacroItemIcon(const char* fileName) {
    char* ext = SStrChrR(const_cast<char*>(fileName), '.');

    if (!ext) {
        return;
    }

    if (!SStrCmpI(ext, ".blp", STORM_MAX_STR) || !SStrCmpI(ext, ".tga", STORM_MAX_STR)) {
        char* name = SStrDupA(fileName, __FILE__, __LINE__);
        name[ext - fileName] = '\0';
        *s_macroItemIcons.New() = name;
    }
}

// VERIFY: ae9495e2 auto
// OFFSET: 0x00565630
static int32_t IconListArchiveCallback(const char* fileName, void* param) {
    const char* name = fileName + SStrLen(s_macroIconPath);

    if (!SStrCmpI(name, "Ability_", SStrLen("Ability_")) || !SStrCmpI(name, "Spell_", SStrLen("Spell_"))) {
        AddMacroIcon(name);
    }

    if (!SStrCmpI(name, "INV_", SStrLen("INV_"))) {
        AddMacroItemIcon(name);
    }

    return 1;
}

// VERIFY: f321e4f2 auto
// OFFSET: 0x005656B0
static int32_t IconListDirectoryCallback(OS_FILE_DATA& data, void* param) {
    if (data.flags & OS_FILE_ATTRIBUTE_DIRECTORY) {
        return 0;
    }

    char name[260];

    if (!SStrCmpI(data.fileName, "Ability_", SStrLen("Ability_")) || !SStrCmpI(data.fileName, "Spell_", SStrLen("Spell_"))) {
        SStrCopy(name, data.fileName, sizeof(name));

        for (char* dot = SStrChr(name, '.'); dot && *dot; dot = SStrChr(dot + 1, '.')) {
            if (!SStrCmpI(dot, ".bz", STORM_MAX_STR)) {
                *dot = '\0';
                break;
            }
        }

        AddMacroIcon(name);
    }

    if (!SStrCmpI(data.fileName, "INV_", SStrLen("INV_"))) {
        SStrCopy(name, data.fileName, sizeof(name));

        for (char* dot = SStrChr(name, '.'); dot && *dot; dot = SStrChr(dot + 1, '.')) {
            if (!SStrCmpI(dot, ".bz", STORM_MAX_STR)) {
                *dot = '\0';
                break;
            }
        }

        AddMacroItemIcon(name);
    }

    return 0;
}

// VERIFY: 53ef5430 auto
// OFFSET: 0x00565810
static int32_t IconListCustomCallback(OS_FILE_DATA& data, void* param) {
    if (!(data.flags & OS_FILE_ATTRIBUTE_DIRECTORY)) {
        AddMacroIcon(data.fileName);
        AddMacroItemIcon(data.fileName);
    }

    return 0;
}

// VERIFY: d515d51e manual
// OFFSET: 0x00565840
void CGUIMacros::LoadIcons() {
    s_macroIcons.m_chunk = 256;
    s_macroItemIcons.m_chunk = 256;
    *s_macroIcons.New() = SStrDupA("INV_Misc_QuestionMark", __FILE__, __LINE__);
    *s_macroItemIcons.New() = SStrDupA("INV_Misc_QuestionMark", __FILE__, __LINE__);

    ListInterfaceArchive(s_macroIconPath, IconListArchiveCallback, nullptr);

    char path[260];
    SStrPrintf(path, sizeof(path), "%s%s", SFile::GetDataPath(), s_macroIconPath);
    OsFileList(path, "*", IconListDirectoryCallback, nullptr, 0);
    OsFileList(s_macroIconPath, "*", IconListCustomCallback, nullptr, 0);

    qsort(s_macroIcons.m_data, s_macroIcons.Count(), sizeof(char*), QSortIcons);
    qsort(s_macroItemIcons.m_data, s_macroItemIcons.Count(), sizeof(char*), QSortIcons);

    uint32_t count = s_macroIcons.Count();
    uint32_t i = count - 1;

    if (count != 1) {
        do {
            char* next = s_macroIcons.m_data[i];
            i--;

            if (!SStrCmpI(s_macroIcons.m_data[i], next, STORM_MAX_STR)) {
                SMemFree(s_macroIcons.m_data[i], __FILE__, __LINE__, 0);
                count--;

                if (i < count) {
                    memmove(&s_macroIcons.m_data[i], &s_macroIcons.m_data[i + 1], sizeof(char*) * (count - i));
                }
            }
        } while (i);

        if (count != s_macroIcons.Count()) {
            s_macroIcons.SetCount(count);
        }
    }

    count = s_macroItemIcons.Count();
    i = count - 1;

    if (count != 1) {
        do {
            char* next = s_macroItemIcons.m_data[i];
            i--;

            if (!SStrCmpI(s_macroItemIcons.m_data[i], next, STORM_MAX_STR)) {
                SMemFree(s_macroItemIcons.m_data[i], __FILE__, __LINE__, 0);
                count--;

                if (i < count) {
                    memmove(&s_macroItemIcons.m_data[i], &s_macroItemIcons.m_data[i + 1], sizeof(char*) * (count - i));
                }
            }
        } while (i);

        if (count != s_macroItemIcons.Count()) {
            s_macroItemIcons.SetCount(count);
        }
    }
}

struct MacroSlashCommand {
    const char* m_token;
    size_t m_length;
    int32_t m_type;
};

// VERIFY: e04b7c4d manual
// OFFSET: 0x00565C40
int32_t MACRONODE::Parse() {
    lua_State* L = FrameScript_GetContext();
    this->m_showTooltip = 0;
    this->m_dynamic = 0;
    this->m_commands.SetCount(0);

    const char* cursor = this->m_body;
    const char* end = this->m_body + SStrLen(this->m_body);
    MacroSubstring line = { nullptr, nullptr };
    MacroSubstring command = { nullptr, nullptr };
    MacroSubstring args = { nullptr, nullptr };

    while (cursor < end) {
        int32_t type = 0;
        line.SetText(cursor, end, '\n', &cursor);
        cursor++;
        command.SetText(line.m_start, line.m_end, ' ', &args.m_start);

        if (*command.m_start == '/') {
            // STATIC: 0x00ACE300
            static MacroSlashCommand s_slashCommands[] = {
                { "SLASH_CAST", SStrLen("SLASH_CAST"), 2 },
                { "SLASH_CASTRANDOM", SStrLen("SLASH_CASTRANDOM"), 3 },
                { "SLASH_CASTSEQUENCE", SStrLen("SLASH_CASTSEQUENCE"), 4 },
                { "SLASH_USE", SStrLen("SLASH_USE"), 2 },
                { "SLASH_USERANDOM", SStrLen("SLASH_USERANDOM"), 3 },
                { "SLASH_EQUIP", SStrLen("SLASH_EQUIP"), 5 },
                { "SLASH_EQUIP_TO_SLOT", SStrLen("SLASH_EQUIP_TO_SLOT"), 6 },
            };

            for (uint32_t i = 0; !type && i < sizeof(s_slashCommands) / sizeof(s_slashCommands[0]); i++) {
                MacroSlashCommand& slash = s_slashCommands[i];

                for (int32_t n = 1; n <= 9; n++) {
                    char name[256];
                    memcpy(name, slash.m_token, slash.m_length);
                    name[slash.m_length] = static_cast<char>('0' + n);
                    name[slash.m_length + 1] = '\0';

                    lua_pushstring(L, name);
                    lua_rawget(L, LUA_GLOBALSINDEX);
                    const char* text = lua_tostring(L, -1);
                    lua_pop(L, 1);

                    if (!text || !*text) {
                        break;
                    }

                    if (static_cast<size_t>(command.m_end - command.m_start) == SStrLen(text) && !SStrCmpUTF8I(command.m_start, text, SStrLenUTF8(text))) {
                        type = slash.m_type;
                        break;
                    }
                }
            }

            if (!type) {
                continue;
            }
        } else {
            if (*command.m_start != '#' || static_cast<size_t>(command.m_end - command.m_start) < 5 || SStrCmp(command.m_start, "#show", 5)) {
                continue;
            }

            if (static_cast<size_t>(command.m_end - command.m_start) >= 12 && !SStrCmp(command.m_start + 5, "tooltip", 7)) {
                this->m_showTooltip = 1;
            }

            type = 1;
        }

        args.SetText(args.m_start, line.m_end, '\0', nullptr);

        if (args.m_end == args.m_start) {
            continue;
        }

        if (args == "none") {
            break;
        }

        const char* c = args.m_start;

        if (type == 4 || *c == '[') {
            this->m_dynamic = 1;
        } else {
            if (type == 6 && c < args.m_end) {
                while (*c >= '0' && *c <= '9') {
                    if (++c >= args.m_end) {
                        break;
                    }
                }

                while (c < args.m_end && *c == ' ') {
                    c++;
                }
            }

            if (*c >= '0' && *c <= '9') {
                this->m_dynamic = 1;
            }
        }

        MacroCommand* entry = this->m_commands.New();
        entry->m_type = type;
        entry->m_text = args;

        if (type == 1 || !this->m_dynamic) {
            break;
        }
    }

    if (this->m_dynamic && static_cast<int32_t>(this->m_actionBarCount) > 0) {
        s_dynamicMacros.LinkToTail(this);
    } else {
        s_dynamicMacros.UnlinkNode(this);
    }

    return this->FindSpellID();
}

// VERIFY: f206167d manual
// OFFSET: 0x00566240
void CGUIMacros::SortMacros() {
    memset(CGUIMacros::m_sortedMacros, 0, sizeof(CGUIMacros::m_sortedMacros));
    MACRONODE** sorted = static_cast<MACRONODE**>(alloca(sizeof(MACRONODE*) * 36));

    for (uint32_t type = 0; type < 2; type++) {
        MACRONODE** out = sorted;

        for (MACRONODE* macro = s_macros.m_fulllist.Head(); macro; macro = s_macros.m_fulllist.Next(macro)) {
            if (macro->m_type == type) {
                *out++ = macro;
            }
        }

        qsort(sorted, CGUIMacros::m_numMacros[type], sizeof(MACRONODE*), QSortMacrosByName);

        for (uint32_t i = 0; i < CGUIMacros::m_numMacros[type]; i++) {
            CGUIMacros::m_sortedMacros[type * 36 + i] = sorted[i]->m_id;
        }
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_MACROS, nullptr);
}

// VERIFY: 18506b38 auto
// OFFSET: 0x005663A0
int32_t CGUIMacros::SetMacroBody(MACRONODE* macro, const char* body) {
    SStrCopy(macro->m_body, body, sizeof(macro->m_body));
    char* cursor = macro->m_body;
    int32_t chars;

    for (uint32_t i = 0; sgetu8(reinterpret_cast<const uint8_t*>(cursor), &chars) != 0xFFFFFFFF && i < 256; i++) {
        cursor += chars;
    }

    *cursor = '\0';

    return macro->Parse();
}

// VERIFY: 781ecbc4 manual
// OFFSET: 0x00567520
void CGUIMacros::CheckActionBar() {
    for (MACRONODE* macro = s_macros.m_fulllist.Head(); macro; macro = s_macros.m_fulllist.Next(macro)) {
        macro->m_actionBarCount = 0;
        s_dynamicMacros.UnlinkNode(macro);
    }

    for (uint32_t i = 0; i < 144; i++) {
        uint32_t action = CGActionBar::m_slotActions[i];

        if (action && (action & 0xF0000000) == 0x40000000) {
            uint32_t id = action & 0xBFFFFFFF;

            if (id) {
                MACRONODE* macro = s_macros.Ptr(id, HASHKEY_NONE());

                if (macro) {
                    macro->IncrementActionBarCount();
                }
            }
        }
    }
}

// VERIFY: 7e2f0778 manual
// OFFSET: 0x00567C10
void CGUIMacros::LoadMacrosCallback(HOSFILE localFile, HOSFILE cacheFile, void* param) {
    uint32_t value = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(param));

    if (static_cast<uint16_t>(value) != s_initCounter) {
        return;
    }

    uint32_t flag = value & 0xFFFF0000;

    if (!(flag & s_pendingLoads)) {
        return;
    }

    s_pendingLoads &= ~flag;
    uint32_t type = (flag >> 16) - 1;

    for (MACRONODE* macro = s_macros.m_fulllist.Head(); macro; ) {
        if (macro->m_type == type) {
            macro = s_macros.DeleteNode(macro);
        } else {
            macro = s_macros.m_fulllist.Next(macro);
        }
    }

    CGUIMacros::m_numMacros[type] = 0;
    s_maxMacroID[type] = s_macroIDBase[type];

    if (cacheFile != OS_INVALID_FILE) {
        uint32_t size = static_cast<uint32_t>(OsGetFileSize(cacheFile));

        if (size) {
            char* buffer = static_cast<char*>(alloca(size + 1));
            uint32_t bytesRead;

            if (OsReadFile(cacheFile, buffer, size, &bytesRead)) {
                buffer[bytesRead] = '\0';

                MACRONODE* current = nullptr;
                const char* cursor = buffer;
                char body[1024];
                char line[1024];
                char token[1024];

                do {
                    SStrTokenize(&cursor, line, sizeof(line), "\r\n", nullptr);

                    if (!line[0]) {
                        continue;
                    }

                    if (current) {
                        if (SStrCmpI(line, "END", STORM_MAX_STR)) {
                            if (SStrLen(line)) {
                                SStrPack(body, line, sizeof(body));
                                SStrPack(body, "\n", sizeof(body));
                            }
                        } else {
                            CGUIMacros::SetMacroBody(current, body);
                            current = nullptr;
                        }

                        continue;
                    }

                    const char* tokens = line;
                    SStrTokenize(&tokens, token, sizeof(token), " ", nullptr);

                    if (SStrCmpI(token, "MACRO", STORM_MAX_STR) || CGUIMacros::m_numMacros[type] >= 36) {
                        continue;
                    }

                    SStrTokenize(&tokens, token, sizeof(token), " ", nullptr);
                    uint32_t id = SStrToInt(token);

                    if (!id || s_macros.Ptr(id, HASHKEY_NONE())) {
                        continue;
                    }

                    if (id < s_macroIDBase[type]) {
                        id += s_macroIDBase[type];
                    }

                    if (id > s_maxMacroID[type]) {
                        s_maxMacroID[type] = id;
                    }

                    SStrTokenize(&tokens, token, sizeof(token), " \"\r\n", nullptr);

                    if (!token[0]) {
                        continue;
                    }

                    CGUIMacros::m_numMacros[type]++;
                    token[63] = '\0';

                    current = s_macros.New(id, HASHKEY_NONE(), 0, 0);
                    current->m_id = id;
                    current->m_type = type;
                    SStrCopy(current->m_name, token, sizeof(current->m_name));
                    body[0] = '\0';

                    SStrTokenize(&tokens, token, sizeof(token), " \"\r\n", nullptr);

                    if (token[0]) {
                        token[255] = '\0';
                        SStrCopy(current->m_icon, token, sizeof(current->m_icon));
                    }
                } while (cursor && *cursor);
            }
        }
    }

    CGUIMacros::SortMacros();
    CGUIMacros::UpdateMacroSpells();
    CGUIMacros::CheckActionBar();

    if (CGGameUI::s_inWorld) {
        FrameScript_SignalEvent(SCRIPT_EVENT_ACTIONBAR_SLOT_CHANGED, "%d", 0);
    }
}

// VERIFY: a7a6a7f6 manual
// OFFSET: 0x005650C0
int32_t MACRONODE::FindSpellID() {
    MacroSubstring text = { nullptr, nullptr };
    MacroSubstring target = { nullptr, nullptr };
    int32_t itemId = 0;
    int32_t spellId = 0;
    uint32_t count = this->m_commands.Count();

    if (count) {
        uint32_t index;

        for (index = 0; index < count; index++) {
            MacroCommand& command = this->m_commands.m_data[index];
            text.m_start = command.m_text.m_start;
            text.m_end = command.m_text.m_end;

            if (*text.m_start != '[') {
                break;
            }

            if (EvaluateMacroOptions(text.m_start, text.m_end, &text, &target) && text.m_end != text.m_start) {
                break;
            }
        }

        if (index < count) {
            int32_t type = this->m_commands.m_data[index].m_type;

            if (type == 3) {
                text.SetText(text.m_start, text.m_end, ',', nullptr);
            } else if (type == 4) {
                MacroQueryCastSequence(&text);
            } else if (type == 6 && text.m_start < text.m_end) {
                while (*text.m_start >= '0' && *text.m_start <= '9') {
                    if (++text.m_start >= text.m_end) {
                        break;
                    }
                }

                while (text.m_start < text.m_end && *text.m_start == ' ') {
                    text.m_start++;
                }
            }

            if (*text.m_start < '0' || *text.m_start > '9') {
                char name[1024];
                text.GetText(name, sizeof(name));
                itemId = GetItemIDByName(name);

                if (itemId) {
                    spellId = 0;
                } else {
                    int32_t commandType = this->m_commands.m_data[index].m_type;

                    if (commandType != 5 && commandType != 6) {
                        spellId = GetSpellIdByName(name, &this->m_spellFlags);
                    }
                }

                if (!itemId && !spellId) {
                    spellId = -1;
                }
            } else {
                MacroSubstring first = { nullptr, nullptr };
                MacroSubstring second = { nullptr, nullptr };
                first.SetText(text.m_start, text.m_end, ' ', nullptr);
                second.SetText(first.m_end, text.m_end, '\0', nullptr);
                CGBag_C* bag = nullptr;
                int32_t slot;
                int32_t maxSlot;

                if (second.m_end == second.m_start) {
                    slot = first.ToNumber() - 1;
                    maxSlot = 22;
                    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

                    if (player) {
                        bag = &player->m_bag;
                    }
                } else {
                    int32_t bagIndex = first.ToNumber() - 1;
                    slot = second.ToNumber() - 1;

                    if (bagIndex == -1) {
                        slot += 23;
                        maxSlot = 38;
                        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

                        if (player) {
                            bag = &player->m_bag;
                        }
                    } else if (static_cast<uint32_t>(bagIndex) <= 3) {
                        CGObject_C* container = ClntObjMgrObjectPtr(GetBagAtIndex(bagIndex), TYPEMASK_CONTAINER, __FILE__, __LINE__);

                        if (container) {
                            bag = container->GetBag();

                            if (bag) {
                                maxSlot = bag->m_numSlots - 1;
                            }
                        }
                    }
                }

                CGItem_C* item = (bag && slot >= 0 && slot <= maxSlot) ? bag->GetItemPointer(slot) : nullptr;

                if (item) {
                    itemId = item->m_obj->m_entryID;
                    spellId = 0;
                } else {
                    spellId = -1;
                }
            }
        }
    }

    WOWGUID guid = WOWGUID();

    if (target.m_end != target.m_start) {
        char token[256];
        target.GetText(token, sizeof(token));
        Script_GetGUIDFromToken(token, guid, false);
    }

    if (itemId != this->m_itemID || spellId != this->m_spellID || guid != this->m_target) {
        this->m_spellID = spellId;
        this->m_itemID = itemId;
        this->m_target = guid;
        return 1;
    }

    return 0;
}

// VERIFY: 4415731b manual
// OFFSET: 0x00566530
void CGUIMacros::Shutdown() {
    s_initCounter++;
    s_dynamicMacros.UnlinkAll();
    s_macros.Clear();

    for (uint32_t i = s_macroIcons.Count(); i; i--) {
        SMemFree(s_macroIcons.m_data[i - 1], __FILE__, __LINE__, 0);
    }

    s_macroIcons.Clear();

    for (uint32_t i = s_macroItemIcons.Count(); i; i--) {
        SMemFree(s_macroItemIcons.m_data[i - 1], __FILE__, __LINE__, 0);
    }

    s_macroItemIcons.Clear();
    CGUIMacros::m_numMacros[0] = 0;
    CGUIMacros::m_numMacros[1] = 0;
    s_macroConditions.Clear();
}

// VERIFY: 55fecf8f auto
// OFFSET: 0x005668F0
void CGUIMacros::SaveMacrosCallback(HOSFILE localFile, HOSFILE cacheFile, void* param) {
    if (cacheFile == OS_INVALID_FILE) {
        return;
    }

    uint32_t set = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(param));
    uint32_t written = 0;

    for (uint32_t i = 0; i < CGUIMacros::m_numMacros[set]; i++) {
        MACRONODE* macro = s_macros.Ptr(CGUIMacros::m_sortedMacros[36 * set + i], HASHKEY_NONE());
        char text[1536];
        SStrPrintf(text, sizeof(text), "MACRO %d \"%s\" %s\r\n", macro->m_id, macro->m_name, macro->m_icon);
        const char* body = macro->m_body;

        while (body && *body) {
            char line[1024];
            SStrTokenize(&body, line, sizeof(line), "\r\n", nullptr);

            if (line[0]) {
                SStrPack(text, line, sizeof(text));
                SStrPack(text, "\r\n", sizeof(text));
            }
        }

        SStrPack(text, "END\r\n", sizeof(text));
        OsWriteFile(cacheFile, text, SStrLen(text), &written);
    }
}

// VERIFY: 67e45d3d auto
// OFFSET: 0x00566A30
void CGUIMacros::SaveMacros() {
    if (!CGUIMacros::m_saveMacros) {
        return;
    }

    CGUIMacros::m_saveMacros = 0;
    SaveAccountData(4, CGUIMacros::SaveMacrosCallback, nullptr);
    SaveAccountData(5, CGUIMacros::SaveMacrosCallback, reinterpret_cast<void*>(1));
}
