#ifndef WOW_SOURCE_DB_REC_DECLINEDWORDCASESREC_H
#define WOW_SOURCE_DB_REC_DECLINEDWORDCASESREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct DeclinedWordCasesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_declinedWordID;
    /* 0008 */ int32_t m_caseIndex;
    /* 000C */ const char* m_declinedWord;

// VERIFY: 56bb3cc7 auto
    // OFFSET: 0x008AE690
    static const char* GetFilename() {
        return "DBFilesClient\\DeclinedWordCases.dbc";
    }

// VERIFY: 78be54e7 manual
    // OFFSET: 0x008AE6A0
    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t declinedWordOffset;

        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t declinedWordIDRead = SFileReadFile(file, &this->m_declinedWordID, sizeof(this->m_declinedWordID), nullptr, nullptr, 0);
        int32_t caseIndexRead = SFileReadFile(file, &this->m_caseIndex, sizeof(this->m_caseIndex), nullptr, nullptr, 0);
        int32_t declinedWordRead = SFileReadFile(file, &declinedWordOffset, sizeof(declinedWordOffset), nullptr, nullptr, 0);

        if (!idRead
            | !declinedWordIDRead
            | !caseIndexRead
            | !declinedWordRead) {
            return false;
        }

        if (stringBuffer) {
            this->m_declinedWord = &stringBuffer[declinedWordOffset];
        } else {
            this->m_declinedWord = "";
        }

        return true;
    }
};

#endif
