#ifndef _TRIAGE_STARTUPREPORT_H
#define _TRIAGE_STARTUPREPORT_H

bool RegisterOggDecompressor();
bool ShutdownFileCache();
void StartupReportSendTelemetry(bool flag, bool unique);

#endif
