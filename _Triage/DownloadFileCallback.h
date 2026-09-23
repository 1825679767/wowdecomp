#ifndef _TRIAGE_DOWNLOADFILECALLBACK_H
#define _TRIAGE_DOWNLOADFILECALLBACK_H

#include <cstddef>
#include <cstdint>

class HttpDownload {
    public:
    /* 0000 */ uint8_t unk_0000 [8];
    /* 0008 */ int32_t unk_0008;
    /* 000C */ uint8_t m_gapC [4];
    /* 0010 */ int32_t unk_0010;
};

class DownloadFileCallback {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint8_t unk_0004[4];
    /* 0008 */ uint32_t unk_0008;
};

#endif
