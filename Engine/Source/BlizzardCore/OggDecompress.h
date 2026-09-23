#ifndef ENGINE_SOURCE_BLIZZARDCORE_OGGDECOMPRESS_H
#define ENGINE_SOURCE_BLIZZARDCORE_OGGDECOMPRESS_H

#include <cstddef>
#include <cstdint>

struct vorbis_comment;
struct vorbis_info;

struct OggVorbis_File {
    /* 0000 */ void* m_datasource;
    /* 0004 */ int32_t m_seekable;
    /* 0008 */ uint8_t m_offset[8];
    /* 0010 */ uint8_t m_end[8];
    /* 0018 */ uint8_t m_oy[28];
    /* 0034 */ int32_t m_links;
    /* 0038 */ int64_t* m_offsets;
    /* 003C */ int64_t* m_dataoffsets;
    /* 0040 */ int32_t* m_serialnos;
    /* 0044 */ int64_t* m_pcmlengths;
    /* 0048 */ vorbis_info* m_vi;
    /* 004C */ vorbis_comment* m_vc;
    /* 0050 */ uint8_t m_pcmOffset[8];
    /* 0058 */ int32_t m_readyState;
    /* 005C */ int32_t m_currentSerialno;
    /* 0060 */ int32_t m_currentLink;
    /* 0064 */ float m_bittrack;
    /* 0068 */ float m_samptrack;
    /* 006C */ uint8_t unk_006C[4];
    /* 0070 */ uint8_t m_os[360];
    /* 01D8 */ uint8_t m_vd[112];
    /* 0248 */ uint8_t m_vb[112];
    /* 02B8 */ uint8_t m_callbacks[16];
};

#endif
