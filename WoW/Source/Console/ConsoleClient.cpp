#include <cstdint>
#include <new>
#include "ConsoleClient.h"
#include "Engine/Source/ConsoleBase/ConsoleCommand.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Gxu/GxuFont.h"
#include "Engine/Source/Os/System.h"
#include "Engine/Source/Services/TextBlock.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/Source/SLock.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "WoW/Source/ConsoleDevice/ConsoleDevice.h"

// STATIC: 0x00ADBACC
static float s_fontHeight = 0.02f;

// STATIC: 0x00ADBAE4
static uint32_t s_baseTextFlags = 8;

// STATIC: 0x00ADBBB0
static TSList<CONSOLELINE, TSGetLink<CONSOLELINE> > s_linelist;

// STATIC: 0x00ADBBBC
static uint32_t s_colorArray[NUM_COLORTYPES] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFF808080, 0xFFFF0000, 0xFFFFFF00, 0xFFFFFFFF, 0xFFFFFFFF, 0x80FFFFFF, 0xC0000000 };

// STATIC: 0x00CA1720
static CONSOLELINE* s_currlineptr;

// STATIC: 0x00CA197C
static uint32_t s_NumLines;

// STATIC: 0x00CA198C
static HTEXTFONT s_textFont;

// STATIC: 0x00CA19A8
static float s_charSpacing;

// STATIC: 0x00CA19B4
static SCritSect s_critsect;

// OFFSET: 0x00763680
static void GenerateNodeString(CONSOLELINE* node) {
    CGxFont* font = TextBlockGetFontPtr(s_textFont);

    if (!font || !node || !node->m_buffer || !node->m_buffer[0]) {
        return;
    }

    if (node->m_fontPointer) {
        GxuFontDestroyString(node->m_fontPointer);
    }

    C3Vector position(0.0f, 0.0f, 1.0f);
    GxuFontCreateString(font, reinterpret_cast<const char*>(node->m_buffer), s_fontHeight, position, 1.0f, s_fontHeight, 0.0f, node->m_fontPointer, GxVJ_Middle, GxHJ_Left, s_baseTextFlags, *reinterpret_cast<const CImVector*>(&s_colorArray[node->m_colorType]), s_charSpacing, 1.0f);
}

// OFFSET: 0x00763D80
CONSOLELINE::~CONSOLELINE() {
    if (this->m_buffer) {
        SMemFree(this->m_buffer, __FILE__, __LINE__, 0);
    }

    if (this->m_fontPointer) {
        GxuFontDestroyString(this->m_fontPointer);
    }
}

// OFFSET: 0x007644B0
static void EnforceMaxLines() {
    if (s_NumLines <= 256) {
        return;
    }

    CONSOLELINE* node = s_linelist.Tail();

    if (node == s_currlineptr) {
        s_currlineptr = s_linelist.Prev(node);
    }

    node->~CONSOLELINE();
    SMemFree(node, __FILE__, __LINE__, 0);
    s_NumLines--;
}

// OFFSET: 0x00765270
void ConsoleWrite(const char* str, COLOR_T color) {
    if (!str || !*str || !ConsoleDeviceExists() || !s_textFont) {
        return;
    }

    s_critsect.Enter();

    CONSOLELINE* node = new (SMemAlloc(sizeof(CONSOLELINE), __FILE__, __LINE__, SMEM_FLAG_ZEROMEMORY)) CONSOLELINE();
    CONSOLELINE* head = s_linelist.Head();

    if (head && head->m_inputpos) {
        s_linelist.LinkNode(node, 1, head);
    } else {
        s_linelist.LinkToHead(node);
    }

    size_t length = SStrLen(str) + 1;
    node->m_chars = length;
    node->m_charsalloc = length;
    node->m_buffer = static_cast<uint8_t*>(SMemAlloc(length, __FILE__, __LINE__, 0));
    SStrCopy(reinterpret_cast<char*>(node->m_buffer), str, STORM_MAX_STR);
    node->m_colorType = color;

    GenerateNodeString(node);
    s_NumLines++;
    EnforceMaxLines();

    s_critsect.Leave();
}

// OFFSET: 0x007658A0
void ConsoleCommandExecute(const char* commandLine, int32_t addToHistory) {
    if (s_execState == 1 || s_execState == 0 || s_execState == 2) {
        if (!AddLineToExecFile(commandLine)) {
            return;
        }
    }

    while (*commandLine == ' ') {
        commandLine++;
    }

    if (addToHistory) {
        const char* last = ConsoleCommandHistory(0);

        if (!last || SStrCmp(commandLine, last, STORM_MAX_STR)) {
            AddToHistory(commandLine);
        }
    }

    char* arguments = static_cast<char*>(SMemAlloc(1024, __FILE__, __LINE__, 0));
    const char* command;
    CONSOLECOMMAND* entry = ParseCommand(commandLine, &command, arguments, 1024);

    if (!entry) {
        if (g_defaultCommand) {
            if (g_defaultCommand(command, arguments)) {
                goto done;
            }
        } else {
            entry = g_consoleCommandHash.Ptr("run");

            if (!entry) {
                ConsoleWrite("Unknown command", DEFAULT_COLOR);
                goto done;
            }

            command = "";
            SStrCopy(arguments, commandLine, STORM_MAX_STR);
        }
    }

    OsValidateFunctionPointer(reinterpret_cast<const void*>(entry->m_handler));
    entry->m_handler(command, arguments);

done:
    if (arguments) {
        delete[] arguments;
    }
}
