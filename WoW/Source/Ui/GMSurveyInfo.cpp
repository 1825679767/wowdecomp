#include "GMSurveyInfo.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00C1E8C4
uint8_t CGGMSurveyInfo::m_surveyAnswers[10];

// STATIC: 0x00C1E8D8
char* CGGMSurveyInfo::m_surveyAnswerComments[10];

// STATIC: 0x00C1E900
char* CGGMSurveyInfo::m_surveyComment;

// VERIFY: cd4c7b4d auto
// OFFSET: 0x005EEB70
void CGGMSurveyInfo::InitializeGame() {
}

// VERIFY: 849050ba manual
// OFFSET: 0x005AC1F0
void CGGMSurveyInfo::FreeAnswers() {
    for (uint32_t i = 0; i < 10; i++) {
        CGGMSurveyInfo::m_surveyAnswers[i] = 0;

        if (CGGMSurveyInfo::m_surveyAnswerComments[i]) {
            SMemFree(CGGMSurveyInfo::m_surveyAnswerComments[i], __FILE__, __LINE__, 0);
        }

        CGGMSurveyInfo::m_surveyAnswerComments[i] = nullptr;
    }

    if (CGGMSurveyInfo::m_surveyComment) {
        SMemFree(CGGMSurveyInfo::m_surveyComment, __FILE__, __LINE__, 0);
    }

    CGGMSurveyInfo::m_surveyComment = nullptr;
}

// VERIFY: df97a125 auto
// OFFSET: 0x005AC520
void CGGMSurveyInfo::ShutdownGame() {
    CGGMSurveyInfo::FreeAnswers();
}
