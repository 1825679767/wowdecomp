#ifndef _TRIAGE_FILEDOWNLOAD_H
#define _TRIAGE_FILEDOWNLOAD_H

#include <cstdint>

class FileDownload {
    public:
    virtual ~FileDownload();
    virtual bool Initialize(const char* fileName, const uint8_t* md5, uint64_t size, uint64_t& offset);
    virtual void Destroy(bool deleteFile);
    virtual int32_t GetStatus();
    virtual int32_t GetReason(uint32_t* reason);
    virtual void Proceed(const uint8_t* data, uint32_t size);
    virtual float GetProgress();
    virtual void StopDownload();
    virtual void CompleteDownload();
    virtual void Accepted();
};

#endif
