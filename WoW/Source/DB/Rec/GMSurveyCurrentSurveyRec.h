#ifndef WOW_SOURCE_DB_REC_GMSURVEYCURRENTSURVEYREC_H
#define WOW_SOURCE_DB_REC_GMSURVEYCURRENTSURVEYREC_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"

struct GMSurveyCurrentSurveyRec {
    /* 0000 */ int32_t m_langid;
    /* 0004 */ int32_t m_gmsurveyID;

// VERIFY: 8abfddeb auto
    // OFFSET: 0x008AFE20
    static const char* GetFilename() {
        return "DBFilesClient\\GMSurveyCurrentSurvey.dbc";
    }

    bool Read(SFileImpl* file, const char* stringBuffer) {
        int32_t langidRead = SFileReadFile(file, &this->m_langid, sizeof(this->m_langid), nullptr, nullptr, 0);
        int32_t gmsurveyIDRead = SFileReadFile(file, &this->m_gmsurveyID, sizeof(this->m_gmsurveyID), nullptr, nullptr, 0);

        if (!langidRead
            | !gmsurveyIDRead) {
            return false;
        }

        return true;
    }
};

#endif
