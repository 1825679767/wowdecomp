#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_OBJECTEFFECT_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_OBJECTEFFECT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"

class CObjectEffectDef;
class CObjectEffectGroupDef;
class ObjectEffectModifierRec;

class CObjectEffect {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint8_t unk_0008[8];
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint8_t unk_0018[4];
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ uint32_t unk_0020;
    /* 0024 */ uint32_t unk_0024;
};

class CObjectEffectManager {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint8_t unk_0004[12];
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint8_t unk_0018[84];
    /* 006C */ uint32_t unk_006C;
    /* 0070 */ uint32_t unk_0070;
    /* 0074 */ uint8_t unk_0074[4];
    /* 0078 */ uint32_t unk_0078;
    /* 007C */ uint32_t unk_007C;

    void EnterState(uint32_t state, int32_t a2, int32_t a3);
    void ExitState(uint32_t state, int32_t a2);
};

struct ObjectEffectAnimState {
    /* 0000 */ int32_t m_animID;
    /* 0004 */ int32_t m_stateID;
    /* 0008 */ int32_t m_altStateID;
};

class CObjectEffectModifier {
    public:
    /* 0000 */ int32_t m_inputType;
    /* 0004 */ int32_t m_mapType;
    /* 0008 */ int32_t m_outputType;
    /* 000C */ float* m_params;
    /* 0010 */ int32_t m_numParams;
    /* 0014 */ int32_t m_id;

    CObjectEffectModifier();
    void Init(const ObjectEffectModifierRec* rec);
    void SetParam(int32_t index, float value);
};

class CObjectEffectDefRef : public TSLinkedNode<CObjectEffectDefRef> {
    public:
    /* 0008 */ CObjectEffectDef* m_effectDef;

    CObjectEffectDefRef();
};

class CObjectEffectGroupDefRef : public TSLinkedNode<CObjectEffectGroupDefRef> {
    public:
    /* 0008 */ CObjectEffectGroupDef* m_groupDef;

    CObjectEffectGroupDefRef();
};

class CObjectEffectDef : public TSLinkedNode<CObjectEffectDef> {
    public:
    /* 0008 */ int32_t m_id;
    /* 000C */ int32_t m_effectRecType;
    /* 0010 */ int32_t m_effectRecID;
    /* 0014 */ int32_t m_attachment;
    /* 0018 */ C3Vector m_offset;
    /* 0024 */ CObjectEffectModifier* m_modifier;
};

class CObjectEffectGroupDef : public TSLinkedNode<CObjectEffectGroupDef> {
    public:
    /* 0008 */ int32_t m_id;
    /* 000C */ TSList<CObjectEffectDefRef> unk_000C;
    /* 0018 */ TSList<CObjectEffectDefRef> unk_0018;
    /* 0024 */ TSList<CObjectEffectDefRef> unk_0024;
    /* 0030 */ TSList<CObjectEffectDefRef> unk_0030;
};

struct OBJ_EFFECT_GROUP_DEF_REF_LIST_LOOKUP : public TSHashObject<OBJ_EFFECT_GROUP_DEF_REF_LIST_LOOKUP, HASHKEY_NONE> {
    /* 0018 */ TSList<CObjectEffectGroupDefRef> m_groupDefRefs;
};

class CObjectEffectPackageDef : public TSLinkedNode<CObjectEffectPackageDef> {
    public:
    /* 0008 */ int32_t m_id;
    /* 000C */ TSHashTable<OBJ_EFFECT_GROUP_DEF_REF_LIST_LOOKUP, HASHKEY_NONE> m_stateGroupRefs;
    /* 0034 */ TSHashTable<OBJ_EFFECT_GROUP_DEF_REF_LIST_LOOKUP, HASHKEY_NONE> m_eventGroupRefs;
};

struct OBJ_EFFECT_DEF_LOOKUP : public TSHashObject<OBJ_EFFECT_DEF_LOOKUP, HASHKEY_NONE> {
    /* 0018 */ CObjectEffectDef m_objectEffectDef;
};

struct OBJ_EFFECT_GROUP_DEF_LOOKUP : public TSHashObject<OBJ_EFFECT_GROUP_DEF_LOOKUP, HASHKEY_NONE> {
    /* 0018 */ CObjectEffectGroupDef m_objectEffectGroupDef;
};

struct OBJ_EFFECT_PACKAGE_DEF_LOOKUP : public TSHashObject<OBJ_EFFECT_PACKAGE_DEF_LOOKUP, HASHKEY_NONE> {
    /* 0018 */ CObjectEffectPackageDef m_objectEffectPackageDef;
};

void InitObjectEffectStateMap();
void ObjectEffectSystem_ShutDown();
void ObjectEffectSystem_Initialize();

#endif
