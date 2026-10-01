#ifndef WOW_SOURCE_UI_GMSURVEYINFO_H
#define WOW_SOURCE_UI_GMSURVEYINFO_H

#include <cstddef>
#include <cstdint>

class CGGMSurveyInfo {
    public:
    static uint8_t m_surveyAnswers[10];
    static char* m_surveyAnswerComments[10];
    static char* m_surveyComment;
    static void ShutdownGame();
    static void FreeAnswers();
    static void InitializeGame();
};

#endif
