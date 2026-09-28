#ifndef WOW_SOURCE_GLUE_GLUEMGR_H
#define WOW_SOURCE_GLUE_GLUEMGR_H

extern const char* g_glueScriptEvents[41];

void GlueScriptEventsInitialize();
void GlueScriptEventsRegisterFunctions();
void GlueScriptEventsUnregisterFunctions();

#endif
