#include "MacroSubstring.h"

// VERIFY: 8f25a363 manual
// OFFSET: 0x005EEA40
void MacroSubstring::SetText(const char* start, const char* end, char delimiter, const char** next) {
    this->m_start = start;

    while (this->m_start < end && *this->m_start == ' ') {
        this->m_start++;
    }

    if (delimiter) {
        this->m_end = this->m_start;

        while (this->m_end < end && *this->m_end != delimiter) {
            this->m_end++;
        }

        if (next) {
            *next = this->m_end;
        }
    } else {
        this->m_end = end;
    }

    while (this->m_end > this->m_start && *(this->m_end - 1) == ' ') {
        this->m_end--;
    }
}

// VERIFY: c0aa335f manual
// OFFSET: 0x005EEB30
bool MacroSubstring::operator==(const char* str) {
    const char* cursor = this->m_start;

    while (*str) {
        if (cursor == this->m_end || *str != *cursor) {
            return false;
        }

        str++;
        cursor++;
    }

    return cursor == this->m_end;
}
