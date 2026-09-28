#include "Engine/Source/Gxu/GxuScene.h"

// VERIFY: 900a5aaa manual
// OFFSET: 0x009A8180
GxuScene::~GxuScene() {
}

// VERIFY: b534d669 auto
// OFFSET: 0x009A81C0
void GxuScene::Clear() {
    for (uint32_t i = 0; i < this->m_objects.Count(); i++) {
        this->m_objects[i]->Unk04();
    }

    this->m_objects.SetCount(0);
}
