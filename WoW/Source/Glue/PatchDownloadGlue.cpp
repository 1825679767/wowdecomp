#include "PatchDownloadGlue.h"
#include "WoW/Source/Glue/CGlueMgr.h"

// STATIC: 0x00B6B25C
FileDownload* CGlueMgr::m_patchDownload;

// STATIC: 0x00B6A990
float CGlueMgr::m_patchDownloadProgress;
