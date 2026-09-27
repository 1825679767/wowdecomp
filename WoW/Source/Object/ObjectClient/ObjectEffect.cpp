#include "ObjectEffect.h"
#include <new>
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Object/ObjectClient/Types.h"

// STATIC: 0x00C9ECD0
static ObjectEffectAnimState s_objectEffectAnimStateMap[ANIM_COUNT];

// STATIC: 0x00CA0488
static HASHKEY_NONE s_hashKeyNone;

// STATIC: 0x00CA048C
static TSHashTable<OBJ_EFFECT_DEF_LOOKUP, HASHKEY_NONE> s_effectDefLookup;

// STATIC: 0x00CA04B4
static TSHashTable<OBJ_EFFECT_GROUP_DEF_LOOKUP, HASHKEY_NONE> s_groupDefLookup;

// STATIC: 0x00CA04DC
static TSHashTable<OBJ_EFFECT_PACKAGE_DEF_LOOKUP, HASHKEY_NONE> s_packageDefLookup;

// INLINED
CObjectEffectModifier::CObjectEffectModifier() {
    this->m_inputType = 0;
    this->m_mapType = 0;
    this->m_outputType = 0;
    this->m_params = nullptr;
    this->m_numParams = 0;
}

// INLINED
void CObjectEffectModifier::Init(const ObjectEffectModifierRec* rec) {
    this->m_inputType = rec->m_inputType;
    this->m_outputType = rec->m_outputType;
    this->m_mapType = rec->m_mapType;
    this->m_id = rec->m_id;
    this->m_numParams = this->m_mapType == 1 ? 4 : 0;

    if (this->m_numParams) {
        this->m_params = new (__FILE__, __LINE__) float[this->m_numParams];
    }
}

// INLINED
void CObjectEffectModifier::SetParam(int32_t index, float value) {
    this->m_params[index] = value;
}

// INLINED
CObjectEffectDefRef::CObjectEffectDefRef() {
    this->m_effectDef = nullptr;
}

// INLINED
CObjectEffectGroupDefRef::CObjectEffectGroupDefRef() {
    this->m_groupDef = nullptr;
}

// VERIFY: 918fad2d auto
// OFFSET: 0x006F1820
void InitObjectEffectStateMap() {
    for (int32_t i = 0; i < ANIM_COUNT; i++) {
        s_objectEffectAnimStateMap[i].m_animID = ANIM_COUNT;
        s_objectEffectAnimStateMap[i].m_stateID = 0;
    }

    s_objectEffectAnimStateMap[ANIM_ATTACK_THROWN].m_animID = ANIM_ATTACK_THROWN;
    s_objectEffectAnimStateMap[ANIM_ATTACK_THROWN].m_stateID = 8;
    s_objectEffectAnimStateMap[ANIM_ATTACK_THROWN].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_HOLD_THROWN].m_animID = ANIM_HOLD_THROWN;
    s_objectEffectAnimStateMap[ANIM_HOLD_THROWN].m_stateID = 9;
    s_objectEffectAnimStateMap[ANIM_HOLD_THROWN].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_LOAD_THROWN].m_animID = ANIM_LOAD_THROWN;
    s_objectEffectAnimStateMap[ANIM_LOAD_THROWN].m_stateID = 10;
    s_objectEffectAnimStateMap[ANIM_LOAD_THROWN].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_READY_THROWN].m_animID = ANIM_READY_THROWN;
    s_objectEffectAnimStateMap[ANIM_READY_THROWN].m_stateID = 11;
    s_objectEffectAnimStateMap[ANIM_READY_THROWN].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_RUN].m_animID = ANIM_RUN;
    s_objectEffectAnimStateMap[ANIM_RUN].m_stateID = 12;
    s_objectEffectAnimStateMap[ANIM_RUN].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_WALK].m_animID = ANIM_WALK;
    s_objectEffectAnimStateMap[ANIM_WALK].m_stateID = 13;
    s_objectEffectAnimStateMap[ANIM_WALK].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_COMBAT_WOUND].m_animID = ANIM_COMBAT_WOUND;
    s_objectEffectAnimStateMap[ANIM_COMBAT_WOUND].m_stateID = 14;
    s_objectEffectAnimStateMap[ANIM_COMBAT_WOUND].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_DEATH].m_animID = ANIM_DEATH;
    s_objectEffectAnimStateMap[ANIM_DEATH].m_stateID = 15;
    s_objectEffectAnimStateMap[ANIM_DEATH].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_DEAD].m_animID = ANIM_DEAD;
    s_objectEffectAnimStateMap[ANIM_DEAD].m_stateID = 81;
    s_objectEffectAnimStateMap[ANIM_DEAD].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_SPELL_CAST_DIRECTED].m_animID = ANIM_SPELL_CAST_DIRECTED;
    s_objectEffectAnimStateMap[ANIM_SPELL_CAST_DIRECTED].m_stateID = 16;
    s_objectEffectAnimStateMap[ANIM_SPELL_CAST_DIRECTED].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_STAND_WOUND].m_animID = ANIM_STAND_WOUND;
    s_objectEffectAnimStateMap[ANIM_STAND_WOUND].m_stateID = 17;
    s_objectEffectAnimStateMap[ANIM_STAND_WOUND].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_READY_UNARMED].m_animID = ANIM_READY_UNARMED;
    s_objectEffectAnimStateMap[ANIM_READY_UNARMED].m_stateID = 18;
    s_objectEffectAnimStateMap[ANIM_READY_UNARMED].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_STAND].m_animID = ANIM_STAND;
    s_objectEffectAnimStateMap[ANIM_STAND].m_stateID = 19;
    s_objectEffectAnimStateMap[ANIM_STAND].m_altStateID = 41;
    s_objectEffectAnimStateMap[ANIM_SHUFFLE_LEFT].m_animID = ANIM_SHUFFLE_LEFT;
    s_objectEffectAnimStateMap[ANIM_SHUFFLE_LEFT].m_stateID = 20;
    s_objectEffectAnimStateMap[ANIM_SHUFFLE_LEFT].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_SHUFFLE_RIGHT].m_animID = ANIM_SHUFFLE_RIGHT;
    s_objectEffectAnimStateMap[ANIM_SHUFFLE_RIGHT].m_stateID = 21;
    s_objectEffectAnimStateMap[ANIM_SHUFFLE_RIGHT].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_WALKBACKWARDS].m_animID = ANIM_WALKBACKWARDS;
    s_objectEffectAnimStateMap[ANIM_WALKBACKWARDS].m_stateID = 22;
    s_objectEffectAnimStateMap[ANIM_WALKBACKWARDS].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_JUMP_START].m_animID = ANIM_JUMP_START;
    s_objectEffectAnimStateMap[ANIM_JUMP_START].m_stateID = 23;
    s_objectEffectAnimStateMap[ANIM_JUMP_START].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_JUMP_END].m_animID = ANIM_JUMP_END;
    s_objectEffectAnimStateMap[ANIM_JUMP_END].m_stateID = 24;
    s_objectEffectAnimStateMap[ANIM_JUMP_END].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_FALL].m_animID = ANIM_FALL;
    s_objectEffectAnimStateMap[ANIM_FALL].m_stateID = 25;
    s_objectEffectAnimStateMap[ANIM_FALL].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_SWIM_IDLE].m_animID = ANIM_SWIM_IDLE;
    s_objectEffectAnimStateMap[ANIM_SWIM_IDLE].m_stateID = 26;
    s_objectEffectAnimStateMap[ANIM_SWIM_IDLE].m_altStateID = 42;
    s_objectEffectAnimStateMap[ANIM_SWIM].m_animID = ANIM_SWIM;
    s_objectEffectAnimStateMap[ANIM_SWIM].m_stateID = 27;
    s_objectEffectAnimStateMap[ANIM_SWIM].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_SWIM_LEFT].m_animID = ANIM_SWIM_LEFT;
    s_objectEffectAnimStateMap[ANIM_SWIM_LEFT].m_stateID = 28;
    s_objectEffectAnimStateMap[ANIM_SWIM_LEFT].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_SWIM_RIGHT].m_animID = ANIM_SWIM_RIGHT;
    s_objectEffectAnimStateMap[ANIM_SWIM_RIGHT].m_stateID = 29;
    s_objectEffectAnimStateMap[ANIM_SWIM_RIGHT].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_SWIM_BACKWARDS].m_animID = ANIM_SWIM_BACKWARDS;
    s_objectEffectAnimStateMap[ANIM_SWIM_BACKWARDS].m_stateID = 30;
    s_objectEffectAnimStateMap[ANIM_SWIM_BACKWARDS].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_RUN_LEFT].m_animID = ANIM_RUN_LEFT;
    s_objectEffectAnimStateMap[ANIM_RUN_LEFT].m_stateID = 31;
    s_objectEffectAnimStateMap[ANIM_RUN_LEFT].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_RUN_RIGHT].m_animID = ANIM_RUN_RIGHT;
    s_objectEffectAnimStateMap[ANIM_RUN_RIGHT].m_stateID = 32;
    s_objectEffectAnimStateMap[ANIM_RUN_RIGHT].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_FLY].m_animID = ANIM_FLY;
    s_objectEffectAnimStateMap[ANIM_FLY].m_stateID = 33;
    s_objectEffectAnimStateMap[ANIM_FLY].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_SPRINT].m_animID = ANIM_SPRINT;
    s_objectEffectAnimStateMap[ANIM_SPRINT].m_stateID = 34;
    s_objectEffectAnimStateMap[ANIM_SPRINT].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_JUMP_LAND_RUN].m_animID = ANIM_JUMP_LAND_RUN;
    s_objectEffectAnimStateMap[ANIM_JUMP_LAND_RUN].m_stateID = 35;
    s_objectEffectAnimStateMap[ANIM_JUMP_LAND_RUN].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_JUMP].m_animID = ANIM_JUMP;
    s_objectEffectAnimStateMap[ANIM_JUMP].m_stateID = 36;
    s_objectEffectAnimStateMap[ANIM_JUMP].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_COMBAT_CRITICAL].m_animID = ANIM_COMBAT_CRITICAL;
    s_objectEffectAnimStateMap[ANIM_COMBAT_CRITICAL].m_stateID = 39;
    s_objectEffectAnimStateMap[ANIM_COMBAT_CRITICAL].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_ATTACK_UNARMED].m_animID = ANIM_ATTACK_UNARMED;
    s_objectEffectAnimStateMap[ANIM_ATTACK_UNARMED].m_stateID = 40;
    s_objectEffectAnimStateMap[ANIM_ATTACK_UNARMED].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_SIT_GROUND].m_animID = ANIM_SIT_GROUND;
    s_objectEffectAnimStateMap[ANIM_SIT_GROUND].m_stateID = 43;
    s_objectEffectAnimStateMap[ANIM_SIT_GROUND].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_SIT_GROUND_UP].m_animID = ANIM_SIT_GROUND_UP;
    s_objectEffectAnimStateMap[ANIM_SIT_GROUND_UP].m_stateID = 44;
    s_objectEffectAnimStateMap[ANIM_SIT_GROUND_UP].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_SIT_GROUND_DOWN].m_animID = ANIM_SIT_GROUND_DOWN;
    s_objectEffectAnimStateMap[ANIM_SIT_GROUND_DOWN].m_stateID = 45;
    s_objectEffectAnimStateMap[ANIM_SIT_GROUND_DOWN].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_HOVER].m_animID = ANIM_HOVER;
    s_objectEffectAnimStateMap[ANIM_HOVER].m_stateID = 46;
    s_objectEffectAnimStateMap[ANIM_HOVER].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_GRAB].m_animID = ANIM_GRAB;
    s_objectEffectAnimStateMap[ANIM_GRAB].m_stateID = 67;
    s_objectEffectAnimStateMap[ANIM_GRAB].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_SHIP_START].m_animID = ANIM_SHIP_START;
    s_objectEffectAnimStateMap[ANIM_SHIP_START].m_stateID = 68;
    s_objectEffectAnimStateMap[ANIM_SHIP_START].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_SHIP_MOVING].m_animID = ANIM_SHIP_MOVING;
    s_objectEffectAnimStateMap[ANIM_SHIP_MOVING].m_stateID = 69;
    s_objectEffectAnimStateMap[ANIM_SHIP_MOVING].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_SHIP_STOP].m_animID = ANIM_SHIP_STOP;
    s_objectEffectAnimStateMap[ANIM_SHIP_STOP].m_stateID = 70;
    s_objectEffectAnimStateMap[ANIM_SHIP_STOP].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_OPEN].m_animID = ANIM_OPEN;
    s_objectEffectAnimStateMap[ANIM_OPEN].m_stateID = 71;
    s_objectEffectAnimStateMap[ANIM_OPEN].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_OPENED].m_animID = ANIM_OPENED;
    s_objectEffectAnimStateMap[ANIM_OPENED].m_stateID = 72;
    s_objectEffectAnimStateMap[ANIM_OPENED].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_CLOSE].m_animID = ANIM_CLOSE;
    s_objectEffectAnimStateMap[ANIM_CLOSE].m_stateID = 73;
    s_objectEffectAnimStateMap[ANIM_CLOSE].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_CLOSED].m_animID = ANIM_CLOSED;
    s_objectEffectAnimStateMap[ANIM_CLOSED].m_stateID = 74;
    s_objectEffectAnimStateMap[ANIM_CLOSED].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_DESTROY].m_animID = ANIM_DESTROY;
    s_objectEffectAnimStateMap[ANIM_DESTROY].m_stateID = 75;
    s_objectEffectAnimStateMap[ANIM_DESTROY].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_DESTROYED].m_animID = ANIM_DESTROYED;
    s_objectEffectAnimStateMap[ANIM_DESTROYED].m_stateID = 76;
    s_objectEffectAnimStateMap[ANIM_DESTROYED].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_CUSTOM_0].m_animID = ANIM_CUSTOM_0;
    s_objectEffectAnimStateMap[ANIM_CUSTOM_0].m_stateID = 77;
    s_objectEffectAnimStateMap[ANIM_CUSTOM_0].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_CUSTOM_1].m_animID = ANIM_CUSTOM_1;
    s_objectEffectAnimStateMap[ANIM_CUSTOM_1].m_stateID = 78;
    s_objectEffectAnimStateMap[ANIM_CUSTOM_1].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_CUSTOM_2].m_animID = ANIM_CUSTOM_2;
    s_objectEffectAnimStateMap[ANIM_CUSTOM_2].m_stateID = 79;
    s_objectEffectAnimStateMap[ANIM_CUSTOM_2].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_CUSTOM_3].m_animID = ANIM_CUSTOM_3;
    s_objectEffectAnimStateMap[ANIM_CUSTOM_3].m_stateID = 80;
    s_objectEffectAnimStateMap[ANIM_CUSTOM_3].m_altStateID = 0;
}

// VERIFY: 56342ab3 auto
// OFFSET: 0x006F2B40
void ObjectEffectSystem_ShutDown() {
    s_effectDefLookup.Clear();
    s_groupDefLookup.Clear();
    s_packageDefLookup.Clear();
}

// VERIFY: 320e76a7 manual
// OFFSET: 0x006F66B0
void ObjectEffectSystem_Initialize() {
    InitObjectEffectStateMap();

    int32_t packageCount = g_objectEffectPackageDB.m_numRecords;

    for (int32_t i = 0; i < packageCount; i++) {
        ObjectEffectPackageRec* rec = (i < 0 || i >= g_objectEffectPackageDB.m_numRecords) ? nullptr : &g_objectEffectPackageDB.m_records[i];
        OBJ_EFFECT_PACKAGE_DEF_LOOKUP* packageLookup = s_packageDefLookup.New(rec->m_id, s_hashKeyNone, 0, 0);
        packageLookup->m_objectEffectPackageDef.m_id = rec->m_id;
    }

    int32_t groupCount = g_objectEffectGroupDB.m_numRecords;

    for (int32_t i = 0; i < groupCount; i++) {
        ObjectEffectGroupRec* rec = (i < 0 || i >= g_objectEffectGroupDB.m_numRecords) ? nullptr : &g_objectEffectGroupDB.m_records[i];
        OBJ_EFFECT_GROUP_DEF_LOOKUP* groupLookup = s_groupDefLookup.New(rec->m_id, s_hashKeyNone, 0, 0);
        groupLookup->m_objectEffectGroupDef.m_id = rec->m_id;
    }

    int32_t elemCount = g_objectEffectPackageElemDB.m_numRecords;

    for (int32_t i = 0; i < elemCount; i++) {
        ObjectEffectPackageElemRec* rec = (i < 0 || i >= g_objectEffectPackageElemDB.m_numRecords) ? nullptr : &g_objectEffectPackageElemDB.m_records[i];
        OBJ_EFFECT_PACKAGE_DEF_LOOKUP* packageLookup = s_packageDefLookup.Ptr(rec->m_objectEffectPackageID, s_hashKeyNone);

        if (!packageLookup) {
            continue;
        }

        OBJ_EFFECT_GROUP_DEF_LOOKUP* groupLookup = s_groupDefLookup.Ptr(rec->m_objectEffectGroupID, s_hashKeyNone);
        OBJ_EFFECT_GROUP_DEF_REF_LIST_LOOKUP* refList = packageLookup->m_objectEffectPackageDef.m_stateGroupRefs.Ptr(rec->m_stateType, s_hashKeyNone);

        if (!refList) {
            refList = packageLookup->m_objectEffectPackageDef.m_stateGroupRefs.New(rec->m_stateType, s_hashKeyNone, 0, 0);
        }

        CObjectEffectGroupDefRef* groupRef = new (SMemAlloc(sizeof(CObjectEffectGroupDefRef), __FILE__, __LINE__, 0)) CObjectEffectGroupDefRef();
        groupRef->m_groupDef = &groupLookup->m_objectEffectGroupDef;
        refList->m_groupDefRefs.LinkToTail(groupRef);
    }

    int32_t effectCount = g_objectEffectDB.m_numRecords;

    for (int32_t i = 0; i < effectCount; i++) {
        ObjectEffectRec* rec = (i < 0 || i >= g_objectEffectDB.m_numRecords) ? nullptr : &g_objectEffectDB.m_records[i];
        OBJ_EFFECT_DEF_LOOKUP* effectLookup = s_effectDefLookup.New(rec->m_id, s_hashKeyNone, 0, 0);
        effectLookup->m_objectEffectDef.m_id = rec->m_id;
        effectLookup->m_objectEffectDef.m_effectRecType = rec->m_effectRecType;
        effectLookup->m_objectEffectDef.m_effectRecID = rec->m_effectRecID;
        effectLookup->m_objectEffectDef.m_attachment = rec->m_attachment;
        effectLookup->m_objectEffectDef.m_offset.x = rec->m_offset[0];
        effectLookup->m_objectEffectDef.m_offset.y = rec->m_offset[1];
        effectLookup->m_objectEffectDef.m_offset.z = rec->m_offset[2];

        ObjectEffectModifierRec* modifierRec = rec->m_objectEffectModifierID ? g_objectEffectModifierDB.GetRecord(rec->m_objectEffectModifierID) : nullptr;

        if (rec->m_objectEffectModifierID && modifierRec->m_inputType > 0 && modifierRec->m_mapType > 0 && modifierRec->m_outputType > 0) {
            effectLookup->m_objectEffectDef.m_modifier = new (SMemAlloc(sizeof(CObjectEffectModifier), __FILE__, __LINE__, 0)) CObjectEffectModifier();
            effectLookup->m_objectEffectDef.m_modifier->Init(modifierRec);

            for (int32_t param = 0; param < effectLookup->m_objectEffectDef.m_modifier->m_numParams; param++) {
                effectLookup->m_objectEffectDef.m_modifier->SetParam(param, modifierRec->m_param[param]);
            }
        } else {
            effectLookup->m_objectEffectDef.m_modifier = nullptr;
        }

        OBJ_EFFECT_GROUP_DEF_LOOKUP* groupLookup = s_groupDefLookup.Ptr(rec->m_objectEffectGroupID, s_hashKeyNone);
        CObjectEffectDefRef* effectRef = new (SMemAlloc(sizeof(CObjectEffectDefRef), __FILE__, __LINE__, 0)) CObjectEffectDefRef();
        effectRef->m_effectDef = &effectLookup->m_objectEffectDef;

        switch (rec->m_triggerType) {
            case 1:
                groupLookup->m_objectEffectGroupDef.unk_000C.LinkToTail(effectRef);
                break;

            case 2:
                groupLookup->m_objectEffectGroupDef.unk_0018.LinkToTail(effectRef);
                break;

            case 3:
                groupLookup->m_objectEffectGroupDef.unk_0024.LinkToTail(effectRef);
                break;

            case 4:
                groupLookup->m_objectEffectGroupDef.unk_0030.LinkToTail(effectRef);
                break;
        }
    }

    for (OBJ_EFFECT_PACKAGE_DEF_LOOKUP* packageLookup = s_packageDefLookup.m_fulllist.Head(); packageLookup; packageLookup = s_packageDefLookup.Next(packageLookup)) {
        CObjectEffectPackageDef& packageDef = packageLookup->m_objectEffectPackageDef;

        for (OBJ_EFFECT_GROUP_DEF_REF_LIST_LOOKUP* stateRefList = packageDef.m_stateGroupRefs.m_fulllist.Head(); stateRefList; stateRefList = packageDef.m_eventGroupRefs.Next(stateRefList)) {
            for (CObjectEffectGroupDefRef* groupRef = stateRefList->m_groupDefRefs.Head(); groupRef; groupRef = stateRefList->m_groupDefRefs.Next(groupRef)) {
                for (CObjectEffectDefRef* effectRef = groupRef->m_groupDef->unk_0030.Head(); effectRef; effectRef = groupRef->m_groupDef->unk_0030.Next(effectRef)) {
                    ObjectEffectRec* effectRec = g_objectEffectDB.GetRecord(effectRef->m_effectDef->m_id);
                    OBJ_EFFECT_GROUP_DEF_REF_LIST_LOOKUP* eventRefList = packageDef.m_eventGroupRefs.Ptr(effectRec->m_eventType, s_hashKeyNone);
                    bool found = false;

                    if (eventRefList) {
                        for (CObjectEffectGroupDefRef* eventRef = eventRefList->m_groupDefRefs.Head(); eventRef; eventRef = eventRefList->m_groupDefRefs.Next(eventRef)) {
                            if (eventRef->m_groupDef == groupRef->m_groupDef) {
                                found = true;
                                break;
                            }
                        }
                    }

                    if (found) {
                        continue;
                    }

                    eventRefList = packageDef.m_eventGroupRefs.New(effectRec->m_eventType, s_hashKeyNone, 0, 0);

                    CObjectEffectGroupDefRef* eventRef = new (SMemAlloc(sizeof(CObjectEffectGroupDefRef), __FILE__, __LINE__, 0)) CObjectEffectGroupDefRef();
                    eventRef->m_groupDef = groupRef->m_groupDef;
                    eventRefList->m_groupDefRefs.LinkToTail(eventRef);
                }
            }
        }
    }
}
