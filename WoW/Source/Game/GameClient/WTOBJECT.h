#ifndef WOW_SOURCE_GAME_GAMECLIENT_WTOBJECT_H
#define WOW_SOURCE_GAME_GAMECLIENT_WTOBJECT_H

#include <cstddef>
#include <cstdint>

class CM2Model;
struct SWING {
    /* 0000 */ uint8_t unk_0000[1560];
};

class WTOBJECT {
    public:
    /* 0000 */ SWING* m_swing;
    /* 0004 */ CM2Model* m_model;

    ~WTOBJECT();
};

WTOBJECT* WeaponTrailCreate(CM2Model* model);
void WeaponTrailClose(WTOBJECT* trail);
void WeaponTrailsShutdown();
void WeaponTrailSetDrawTrail(WTOBJECT* trail, const uint32_t* a2, uint32_t a3);

void WorldTextInitialize();
void WorldTextShutdown();

#endif
