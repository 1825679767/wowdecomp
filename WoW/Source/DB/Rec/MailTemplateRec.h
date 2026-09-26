#ifndef WOW_SOURCE_DB_REC_MAILTEMPLATEREC_H
#define WOW_SOURCE_DB_REC_MAILTEMPLATEREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "WoW/Source/Client.h"

struct MailTemplateRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_subject;
    /* 0008 */ const char* m_body;

// VERIFY: 0b0e721a auto
    // OFFSET: 0x008B3580
    static const char* GetFilename() {
        return "DBFilesClient\\MailTemplate.dbc";
    }

// VERIFY: 0e07fb81 auto
    // OFFSET: 0x008B3590
    bool Read(SFileImpl* file, const char* stringBuffer) {
        uint32_t subjectOffset[16];
        uint32_t subjectFlags;
        uint32_t bodyOffset[16];
        uint32_t bodyFlags;

        int32_t idRead = SFileReadFile(file, &this->m_id, sizeof(this->m_id), nullptr, nullptr, 0);
        int32_t subjectRead0 = SFileReadFile(file, &subjectOffset[0], sizeof(subjectOffset[0]), nullptr, nullptr, 0);
        int32_t subjectRead1 = SFileReadFile(file, &subjectOffset[1], sizeof(subjectOffset[1]), nullptr, nullptr, 0);
        int32_t subjectRead2 = SFileReadFile(file, &subjectOffset[2], sizeof(subjectOffset[2]), nullptr, nullptr, 0);
        int32_t subjectRead3 = SFileReadFile(file, &subjectOffset[3], sizeof(subjectOffset[3]), nullptr, nullptr, 0);
        int32_t subjectRead4 = SFileReadFile(file, &subjectOffset[4], sizeof(subjectOffset[4]), nullptr, nullptr, 0);
        int32_t subjectRead5 = SFileReadFile(file, &subjectOffset[5], sizeof(subjectOffset[5]), nullptr, nullptr, 0);
        int32_t subjectRead6 = SFileReadFile(file, &subjectOffset[6], sizeof(subjectOffset[6]), nullptr, nullptr, 0);
        int32_t subjectRead7 = SFileReadFile(file, &subjectOffset[7], sizeof(subjectOffset[7]), nullptr, nullptr, 0);
        int32_t subjectRead8 = SFileReadFile(file, &subjectOffset[8], sizeof(subjectOffset[8]), nullptr, nullptr, 0);
        int32_t subjectRead9 = SFileReadFile(file, &subjectOffset[9], sizeof(subjectOffset[9]), nullptr, nullptr, 0);
        int32_t subjectRead10 = SFileReadFile(file, &subjectOffset[10], sizeof(subjectOffset[10]), nullptr, nullptr, 0);
        int32_t subjectRead11 = SFileReadFile(file, &subjectOffset[11], sizeof(subjectOffset[11]), nullptr, nullptr, 0);
        int32_t subjectRead12 = SFileReadFile(file, &subjectOffset[12], sizeof(subjectOffset[12]), nullptr, nullptr, 0);
        int32_t subjectRead13 = SFileReadFile(file, &subjectOffset[13], sizeof(subjectOffset[13]), nullptr, nullptr, 0);
        int32_t subjectRead14 = SFileReadFile(file, &subjectOffset[14], sizeof(subjectOffset[14]), nullptr, nullptr, 0);
        int32_t subjectRead15 = SFileReadFile(file, &subjectOffset[15], sizeof(subjectOffset[15]), nullptr, nullptr, 0);
        int32_t subjectFlagsRead = SFileReadFile(file, &subjectFlags, sizeof(subjectFlags), nullptr, nullptr, 0);
        int32_t bodyRead0 = SFileReadFile(file, &bodyOffset[0], sizeof(bodyOffset[0]), nullptr, nullptr, 0);
        int32_t bodyRead1 = SFileReadFile(file, &bodyOffset[1], sizeof(bodyOffset[1]), nullptr, nullptr, 0);
        int32_t bodyRead2 = SFileReadFile(file, &bodyOffset[2], sizeof(bodyOffset[2]), nullptr, nullptr, 0);
        int32_t bodyRead3 = SFileReadFile(file, &bodyOffset[3], sizeof(bodyOffset[3]), nullptr, nullptr, 0);
        int32_t bodyRead4 = SFileReadFile(file, &bodyOffset[4], sizeof(bodyOffset[4]), nullptr, nullptr, 0);
        int32_t bodyRead5 = SFileReadFile(file, &bodyOffset[5], sizeof(bodyOffset[5]), nullptr, nullptr, 0);
        int32_t bodyRead6 = SFileReadFile(file, &bodyOffset[6], sizeof(bodyOffset[6]), nullptr, nullptr, 0);
        int32_t bodyRead7 = SFileReadFile(file, &bodyOffset[7], sizeof(bodyOffset[7]), nullptr, nullptr, 0);
        int32_t bodyRead8 = SFileReadFile(file, &bodyOffset[8], sizeof(bodyOffset[8]), nullptr, nullptr, 0);
        int32_t bodyRead9 = SFileReadFile(file, &bodyOffset[9], sizeof(bodyOffset[9]), nullptr, nullptr, 0);
        int32_t bodyRead10 = SFileReadFile(file, &bodyOffset[10], sizeof(bodyOffset[10]), nullptr, nullptr, 0);
        int32_t bodyRead11 = SFileReadFile(file, &bodyOffset[11], sizeof(bodyOffset[11]), nullptr, nullptr, 0);
        int32_t bodyRead12 = SFileReadFile(file, &bodyOffset[12], sizeof(bodyOffset[12]), nullptr, nullptr, 0);
        int32_t bodyRead13 = SFileReadFile(file, &bodyOffset[13], sizeof(bodyOffset[13]), nullptr, nullptr, 0);
        int32_t bodyRead14 = SFileReadFile(file, &bodyOffset[14], sizeof(bodyOffset[14]), nullptr, nullptr, 0);
        int32_t bodyRead15 = SFileReadFile(file, &bodyOffset[15], sizeof(bodyOffset[15]), nullptr, nullptr, 0);
        int32_t bodyFlagsRead = SFileReadFile(file, &bodyFlags, sizeof(bodyFlags), nullptr, nullptr, 0);

        if (!idRead
            | !subjectRead0
            | !subjectRead1
            | !subjectRead2
            | !subjectRead3
            | !subjectRead4
            | !subjectRead5
            | !subjectRead6
            | !subjectRead7
            | !subjectRead8
            | !subjectRead9
            | !subjectRead10
            | !subjectRead11
            | !subjectRead12
            | !subjectRead13
            | !subjectRead14
            | !subjectRead15
            | !subjectFlagsRead
            | !bodyRead0
            | !bodyRead1
            | !bodyRead2
            | !bodyRead3
            | !bodyRead4
            | !bodyRead5
            | !bodyRead6
            | !bodyRead7
            | !bodyRead8
            | !bodyRead9
            | !bodyRead10
            | !bodyRead11
            | !bodyRead12
            | !bodyRead13
            | !bodyRead14
            | !bodyRead15
            | !bodyFlagsRead) {
            return false;
        }

        if (stringBuffer) {
            this->m_subject = &stringBuffer[subjectOffset[s_locale]];
            this->m_body = &stringBuffer[bodyOffset[s_locale]];
        } else {
            this->m_subject = "";
            this->m_body = "";
        }

        return true;
    }
};

#endif
