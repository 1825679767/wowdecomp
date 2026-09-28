#include "Tutorial.h"
#include "Engine/Source/Base/CDataStore.h"

// STATIC: 0x00BD1AE0
TSBitField<TSFixedArray<uint32_t> > CGTutorial::m_tutorialShown;

// STATIC: 0x00BD1AF0
TSBitField<TSFixedArray<uint32_t> > CGTutorial::m_tutorialFlags;

// VERIFY: 101b433c manual
// OFFSET: 0x00530920
int32_t CGTutorial::OnTutorialFlags(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t bytes = msg->m_size - msg->m_read;
    void* data;
    msg->GetDataInSitu(data, bytes);
    CGTutorial::m_tutorialShown.Load(data, bytes, -1);
    CGTutorial::m_tutorialFlags.Load(data, bytes, -1);

    return 1;
}
