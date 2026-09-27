#ifndef WOW_SOURCE_WORLDCLIENT_CDETAILDOODADDATA_H
#define WOW_SOURCE_WORLDCLIENT_CDETAILDOODADDATA_H

#include <cstddef>
#include <cstdint>

class CM2Model;
class GroundEffectDoodadRec;

class CDetailDoodadData {
    public:
    /* 0000 */ GroundEffectDoodadRec* m_record;
    /* 0004 */ CM2Model* m_model;
    /* 0008 */ void* unk_0008;
};

#endif
