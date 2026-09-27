#include "SurveyDownloadGlue.h"
#include "WoW/Source/Glue/CGlueMgr.h"

// STATIC: 0x00B6B248
uint32_t CGlueMgr::m_surveyTimer;

// STATIC: 0x00B6B24C
uint8_t CGlueMgr::m_executedSurvey;

// STATIC: 0x00B6B250
FileDownload* CGlueMgr::m_surveyDownload;
