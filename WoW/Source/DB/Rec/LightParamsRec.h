#ifndef WOW_SOURCE_DB_REC_LIGHTPARAMSREC_H
#define WOW_SOURCE_DB_REC_LIGHTPARAMSREC_H

#include <cstddef>
#include <cstdint>

class SFileImpl;

class LightParamsRec {
    public:
    static const int32_t s_numColumns = 9;
    static const int32_t s_rowSize = 36;
    static const bool s_generatedID = false;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_highlightSky;
    /* 0008 */ int32_t m_lightSkyboxID;
    /* 000C */ float m_glow;
    /* 0010 */ float m_waterShallowAlpha;
    /* 0014 */ float m_waterDeepAlpha;
    /* 0018 */ float m_oceanShallowAlpha;
    /* 001C */ float m_oceanDeepAlpha;
    /* 0020 */ int32_t m_flags;

    LightParamsRec();
    ~LightParamsRec();
    static const char* GetFilename();
    bool Read(SFileImpl* file, const char* stringBuffer);
};

#endif
