#ifndef WOW_SOURCE_DB_DBCLIENT_DBCLIENT_H
#define WOW_SOURCE_DB_DBCLIENT_DBCLIENT_H

#include <cstddef>
#include <cstdint>

class CM2Model;

struct AchievementRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_faction;
    /* 0008 */ int32_t m_instanceID;
    /* 000C */ int32_t m_supercedes;
    /* 0010 */ const char* m_title;
    /* 0014 */ const char* m_description;
    /* 0018 */ int32_t m_category;
    /* 001C */ int32_t m_points;
    /* 0020 */ int32_t m_uiOrder;
    /* 0024 */ int32_t m_flags;
    /* 0028 */ int32_t m_iconID;
    /* 002C */ const char* m_reward;
    /* 0030 */ int32_t m_minimumCriteria;
    /* 0034 */ int32_t m_sharesCriteria;
};

struct AnimKitBoneSetAliasRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_boneDataID;
    /* 0008 */ int32_t m_animKitBoneSetID;
};

struct AnimKitBoneSetRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_boneDataID;
    /* 000C */ int32_t m_parentAnimKitBoneSetID;
    /* 0010 */ int32_t m_extraBoneCount;
    /* 0014 */ int32_t m_altAnimKitBoneSetID;
};

struct AnimKitConfigBoneSetRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_parentAnimKitConfigID;
    /* 0008 */ int32_t m_animKitBoneSetID;
    /* 000C */ int32_t m_animKitPriorityID;
};

struct AnimKitConfigRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_configFlags;
};

struct AnimKitPriorityRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_priority;
};

struct AnimKitRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_oneShotDuration;
    /* 0008 */ int32_t m_oneShotStopAnimKitID;
};

struct AnimKitSegmentRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_parentAnimKitID;
    /* 0008 */ int32_t m_animID;
    /* 000C */ int32_t m_animStartTime;
    /* 0010 */ int32_t m_animKitConfigID;
    /* 0014 */ int32_t m_startCondition;
    /* 0018 */ int32_t m_startConditionParam;
    /* 001C */ int32_t m_startConditionDelay;
    /* 0020 */ int32_t m_endCondition;
    /* 0024 */ int32_t m_endConditionParam;
    /* 0028 */ int32_t m_endConditionDelay;
    /* 002C */ float m_speed;
    /* 0030 */ int32_t m_segmentFlags;
    /* 0034 */ int32_t m_forcedVariation;
    /* 0038 */ int32_t m_overrideConfigFlags;
    /* 003C */ int32_t m_loopToSegmentIndex;
};

struct AnimReplacementRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_srcAnimID;
    /* 0008 */ int32_t m_dstAnimID;
    /* 000C */ int32_t m_parentAnimReplacementSetID;
};

struct AnimReplacementSetRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_execOrder;
};

struct AnimationDataRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_weaponflags;
    /* 000C */ int32_t m_bodyflags;
    /* 0010 */ int32_t m_flags;
    /* 0014 */ int32_t m_fallback;
    /* 0018 */ int32_t m_behaviorID;
    /* 001C */ int32_t m_behaviorTier;
};

struct AreaAssignmentRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_mapID;
    /* 0008 */ int32_t m_areaID;
    /* 000C */ int32_t m_chunkX;
    /* 0010 */ int32_t m_chunkY;
};

struct AreaGroupRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_areaID [6];
    /* 001C */ int32_t m_nextAreaID;
};

struct AreaPOIRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_importance;
    /* 0008 */ int32_t m_icon [9];
    /* 002C */ int32_t m_factionID;
    /* 0030 */ float m_pos [3];
    /* 003C */ int32_t m_continentID;
    /* 0040 */ int32_t m_flags;
    /* 0044 */ int32_t m_areaID;
    /* 0048 */ const char* m_name;
    /* 004C */ const char* m_description;
    /* 0050 */ int32_t m_worldStateID;
    /* 0054 */ int32_t m_worldMapLink;
};

struct AreaTableRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_continentID;
    /* 0008 */ int32_t m_parentAreaID;
    /* 000C */ int32_t m_areaBit;
    /* 0010 */ int32_t m_flags;
    /* 0014 */ int32_t m_soundProviderPref;
    /* 0018 */ int32_t m_soundProviderPrefUnderwater;
    /* 001C */ int32_t m_ambienceID;
    /* 0020 */ int32_t m_zoneMusic;
    /* 0024 */ int32_t m_introSound;
    /* 0028 */ int32_t m_explorationLevel;
    /* 002C */ const char* m_areaName;
    /* 0030 */ int32_t m_factionGroupMask;
    /* 0034 */ int32_t m_liquidTypeID [4];
    /* 0044 */ float m_minElevation;
    /* 0048 */ float m_ambientMultiplier;
    /* 004C */ int32_t m_lightID;
};

struct AreaTriggerRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_continentID;
    /* 0008 */ float m_pos [3];
    /* 0014 */ float m_radius;
    /* 0018 */ float m_boxLength;
    /* 001C */ float m_boxWidth;
    /* 0020 */ float m_boxHeight;
    /* 0024 */ float m_boxYaw;
};

struct ArmorLocationRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ float m_clothmodifier;
    /* 0008 */ float m_leathermodifier;
    /* 000C */ float m_chainmodifier;
    /* 0010 */ float m_platemodifier;
    /* 0014 */ float m_modifier;
};

struct AttackAnimKitsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_itemSubclassID;
    /* 0008 */ int32_t m_animTypeID;
    /* 000C */ int32_t m_animFrequency;
    /* 0010 */ int32_t m_whichHand;
};

struct AttackAnimTypesRec {
    /* 0000 */ int32_t m_animID;
    /* 0004 */ const char* m_animName;
    /* 0008 */ int32_t m_generatedID;
};

struct AuctionHouseRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_factionID;
    /* 0008 */ int32_t m_depositRate;
    /* 000C */ int32_t m_consignmentRate;
    /* 0010 */ const char* m_name;
};

struct BankBagSlotPricesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_cost;
};

struct BannedAddOnsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_nameMd5 [4];
    /* 0014 */ int32_t m_versionMd5 [4];
    /* 0024 */ int32_t m_lastModified;
    /* 0028 */ int32_t m_flags;
};

struct BarberShopStyleRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_type;
    /* 0008 */ const char* m_displayName;
    /* 000C */ const char* m_description;
    /* 0010 */ float m_costModifier;
    /* 0014 */ int32_t m_race;
    /* 0018 */ int32_t m_sex;
    /* 001C */ int32_t m_data;
};

struct BattlemasterListRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_mapID [8];
    /* 0024 */ int32_t m_instanceType;
    /* 0028 */ int32_t m_groupsAllowed;
    /* 002C */ const char* m_name;
    /* 0030 */ int32_t m_maxGroupSize;
    /* 0034 */ int32_t m_holidayWorldState;
    /* 0038 */ int32_t m_minLevel;
    /* 003C */ int32_t m_maxLevel;
};

struct CameraModeRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_type;
    /* 000C */ int32_t m_flags;
    /* 0010 */ float m_positionOffset [3];
    /* 001C */ float m_targetOffset [3];
    /* 0028 */ float m_positionSmoothing;
    /* 002C */ float m_rotationSmoothing;
    /* 0030 */ float m_fieldOfView;
    /* 0034 */ int32_t m_lockedPositionOffsetBase;
    /* 0038 */ int32_t m_lockedPositionOffsetDirection;
    /* 003C */ int32_t m_lockedTargetOffsetBase;
    /* 0040 */ int32_t m_lockedTargetOffsetDirection;
};

struct CameraShakesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_shakeType;
    /* 0008 */ int32_t m_direction;
    /* 000C */ float m_amplitude;
    /* 0010 */ float m_frequency;
    /* 0014 */ float m_duration;
    /* 0018 */ float m_phase;
    /* 001C */ float m_coefficient;
};

struct CharBaseInfoRec {
    /* 0000 */ int8_t m_raceID;
    /* 0001 */ int8_t m_classID;
    /* 0002 */ uint8_t unk_0002[2];
    /* 0004 */ int32_t m_generatedID;
};

struct CharHairGeosetsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_raceID;
    /* 0008 */ int32_t m_sexID;
    /* 000C */ int32_t m_variationID;
    /* 0010 */ int32_t m_geosetID;
    /* 0014 */ int32_t m_showscalp;
};

struct CharHairTexturesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_field0533368001Race;
    /* 0008 */ int32_t m_field0533368002Gender;
    /* 000C */ int32_t m_field0533368003;
    /* 0010 */ int32_t m_field0533368004Mayberacemask;
    /* 0014 */ int32_t m_field0533368005TheXInHairXyBlp;
    /* 0018 */ int32_t m_field0533368006;
    /* 001C */ int32_t m_field0533368007;
};

struct CharSectionsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_raceID;
    /* 0008 */ int32_t m_sexID;
    /* 000C */ int32_t m_baseSection;
    /* 0010 */ const char* m_textureName [3];
    /* 001C */ int32_t m_flags;
    /* 0020 */ int32_t m_variationIndex;
    /* 0024 */ int32_t m_colorIndex;
};

struct CharStartOutfitRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int8_t m_raceID;
    /* 0005 */ int8_t m_classID;
    /* 0006 */ int8_t m_sexID;
    /* 0007 */ int8_t m_outfitID;
    /* 0008 */ int32_t m_itemID [24];
    /* 0068 */ int32_t m_displayItemID [24];
    /* 00C8 */ int32_t m_inventoryType [24];
};

struct CharTitlesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_conditionID;
    /* 0008 */ const char* m_name;
    /* 000C */ const char* m_name1;
    /* 0010 */ int32_t m_maskID;
};

struct CharVariationsRec {
    /* 0000 */ int32_t m_raceID;
    /* 0004 */ int32_t m_sexID;
    /* 0008 */ int32_t m_textureHoldLayer [4];
    /* 0018 */ int32_t m_generatedID;
};

struct CharacterFacialHairStylesRec {
    /* 0000 */ int32_t m_raceID;
    /* 0004 */ int32_t m_sexID;
    /* 0008 */ int32_t m_variationID;
    /* 000C */ int32_t m_geoset [5];
    /* 0020 */ int32_t m_generatedID;
};

struct ChatChannelsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_flags;
    /* 0008 */ int32_t m_factionGroup;
    /* 000C */ const char* m_name;
    /* 0010 */ const char* m_shortcut;
};

struct ChatProfanityRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_text;
    /* 0008 */ int32_t m_language;
};

struct ChrClassesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_damageBonusStat;
    /* 0008 */ int32_t m_displayPower;
    /* 000C */ const char* m_petNameToken;
    /* 0010 */ const char* m_name;
    /* 0014 */ const char* m_nameFemale;
    /* 0018 */ const char* m_nameMale;
    /* 001C */ const char* m_filename;
    /* 0020 */ int32_t m_spellClassSet;
    /* 0024 */ int32_t m_flags;
    /* 0028 */ int32_t m_cinematicSequenceID;
    /* 002C */ int32_t m_requiredExpansion;
};

struct ChrRacesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_flags;
    /* 0008 */ int32_t m_factionID;
    /* 000C */ int32_t m_explorationSoundID;
    /* 0010 */ int32_t m_maleDisplayID;
    /* 0014 */ int32_t m_femaleDisplayID;
    /* 0018 */ const char* m_clientPrefix;
    /* 001C */ int32_t m_baseLanguage;
    /* 0020 */ int32_t m_creatureType;
    /* 0024 */ int32_t m_resSicknessSpellID;
    /* 0028 */ int32_t m_splashSoundID;
    /* 002C */ const char* m_clientFileString;
    /* 0030 */ int32_t m_cinematicSequenceID;
    /* 0034 */ int32_t m_alliance;
    /* 0038 */ const char* m_name;
    /* 003C */ const char* m_nameFemale;
    /* 0040 */ const char* m_nameMale;
    /* 0044 */ const char* m_facialHairCustomization [2];
    /* 004C */ const char* m_hairCustomization;
    /* 0050 */ int32_t m_requiredExpansion;
};

struct CinematicCameraRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_model;
    /* 0008 */ int32_t m_soundID;
    /* 000C */ float m_origin [3];
    /* 0018 */ float m_originFacing;
};

struct CinematicSequencesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_soundID;
    /* 0008 */ int32_t m_camera [8];
};

struct CreatureDisplayInfoExtraRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_displayRaceID;
    /* 0008 */ int32_t m_displaySexID;
    /* 000C */ int32_t m_skinID;
    /* 0010 */ int32_t m_faceID;
    /* 0014 */ int32_t m_hairStyleID;
    /* 0018 */ int32_t m_hairColorID;
    /* 001C */ int32_t m_facialHairID;
    /* 0020 */ int32_t m_npcitemDisplay [11];
    /* 004C */ int32_t m_flags;
    /* 0050 */ const char* m_bakeName;
};

struct CreatureDisplayInfoRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_modelID;
    /* 0008 */ int32_t m_soundID;
    /* 000C */ int32_t m_extendedDisplayInfoID;
    /* 0010 */ float m_creatureModelScale;
    /* 0014 */ int32_t m_creatureModelAlpha;
    /* 0018 */ const char* m_textureVariation [3];
    /* 0024 */ const char* m_portraitTextureName;
    /* 0028 */ int32_t m_sizeClass;
    /* 002C */ int32_t m_bloodID;
    /* 0030 */ int32_t m_npcsoundID;
    /* 0034 */ int32_t m_particleColorID;
    /* 0038 */ int32_t m_creatureGeosetData;
    /* 003C */ int32_t m_objectEffectPackageID;
};

struct CreatureFamilyRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ float m_minScale;
    /* 0008 */ int32_t m_minScaleLevel;
    /* 000C */ float m_maxScale;
    /* 0010 */ int32_t m_maxScaleLevel;
    /* 0014 */ int32_t m_skillLine [2];
    /* 001C */ int32_t m_petFoodMask;
    /* 0020 */ int32_t m_petTalentType;
    /* 0024 */ int32_t m_categoryEnumID;
    /* 0028 */ const char* m_name;
    /* 002C */ const char* m_iconFile;
};

struct CreatureModelDataRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_flags;
    /* 0008 */ const char* m_modelName;
    /* 000C */ int32_t m_sizeClass;
    /* 0010 */ float m_modelScale;
    /* 0014 */ int32_t m_bloodID;
    /* 0018 */ int32_t m_footprintTextureID;
    /* 001C */ float m_footprintTextureLength;
    /* 0020 */ float m_footprintTextureWidth;
    /* 0024 */ float m_footprintParticleScale;
    /* 0028 */ int32_t m_foleyMaterialID;
    /* 002C */ int32_t m_footstepShakeSize;
    /* 0030 */ int32_t m_deathThudShakeSize;
    /* 0034 */ int32_t m_soundID;
    /* 0038 */ float m_collisionWidth;
    /* 003C */ float m_collisionHeight;
    /* 0040 */ float m_mountHeight;
    /* 0044 */ float m_geoBoxMinX;
    /* 0048 */ float m_geoBoxMinY;
    /* 004C */ float m_geoBoxMinZ;
    /* 0050 */ float m_geoBoxMaxX;
    /* 0054 */ float m_geoBoxMaxY;
    /* 0058 */ float m_geoBoxMaxZ;
    /* 005C */ float m_worldEffectScale;
    /* 0060 */ float m_attachedEffectScale;
    /* 0064 */ float m_missileCollisionRadius;
    /* 0068 */ float m_missileCollisionPush;
    /* 006C */ float m_missileCollisionRaise;
};

struct CreatureMovementInfoRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ float m_smoothFacingChaseRate;
};

struct CreatureSoundDataRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_soundExertionID;
    /* 0008 */ int32_t m_soundExertionCriticalID;
    /* 000C */ int32_t m_soundInjuryID;
    /* 0010 */ int32_t m_soundInjuryCriticalID;
    /* 0014 */ int32_t m_soundInjuryCrushingBlowID;
    /* 0018 */ int32_t m_soundDeathID;
    /* 001C */ int32_t m_soundStunID;
    /* 0020 */ int32_t m_soundStandID;
    /* 0024 */ int32_t m_soundFootstepID;
    /* 0028 */ int32_t m_soundAggroID;
    /* 002C */ int32_t m_soundWingFlapID;
    /* 0030 */ int32_t m_soundWingGlideID;
    /* 0034 */ int32_t m_soundAlertID;
    /* 0038 */ int32_t m_soundFidget [5];
    /* 004C */ int32_t m_customAttack [4];
    /* 005C */ int32_t m_npcsoundID;
    /* 0060 */ int32_t m_loopSoundID;
    /* 0064 */ int32_t m_creatureImpactType;
    /* 0068 */ int32_t m_soundJumpStartID;
    /* 006C */ int32_t m_soundJumpEndID;
    /* 0070 */ int32_t m_soundPetAttackID;
    /* 0074 */ int32_t m_soundPetOrderID;
    /* 0078 */ int32_t m_soundPetDismissID;
    /* 007C */ float m_fidgetDelaySecondsMin;
    /* 0080 */ float m_fidgetDelaySecondsMax;
    /* 0084 */ int32_t m_birthSoundID;
    /* 0088 */ int32_t m_spellCastDirectedSoundID;
    /* 008C */ int32_t m_submergeSoundID;
    /* 0090 */ int32_t m_submergedSoundID;
    /* 0094 */ int32_t m_creatureSoundDataIdpet;
};

struct CreatureSpellDataRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_spells [4];
    /* 0014 */ int32_t m_availability [4];
};

struct CreatureTypeRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_flags;
};

struct CurrencyCategoryRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_flags;
    /* 0008 */ const char* m_name;
};

struct CurrencyTypesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_itemID;
    /* 0008 */ int32_t m_categoryID;
    /* 000C */ int32_t m_bitIndex;
};

struct DanceMovesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_type;
    /* 0008 */ int32_t m_param;
    /* 000C */ int32_t m_fallback;
    /* 0010 */ int32_t m_racemask;
    /* 0014 */ const char* m_internalName;
    /* 0018 */ const char* m_name;
    /* 001C */ int32_t m_lockID;
};

struct DeathThudLookupsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_sizeClass;
    /* 0008 */ int32_t m_terrainTypeSoundID;
    /* 000C */ int32_t m_soundEntryID;
    /* 0010 */ int32_t m_soundEntryIdwater;
};

struct DeclinedWordCasesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_declinedWordID;
    /* 0008 */ int32_t m_caseIndex;
    /* 000C */ const char* m_declinedWord;
};

struct DeclinedWordRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_word;
};

struct DestructibleModelDataRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_state0ImpactEffectDoodadSet;
    /* 0008 */ int32_t m_state0AmbientDoodadSet;
    /* 000C */ int32_t m_state1WMO;
    /* 0010 */ int32_t m_state1DestructionDoodadSet;
    /* 0014 */ int32_t m_state1ImpactEffectDoodadSet;
    /* 0018 */ int32_t m_state1AmbientDoodadSet;
    /* 001C */ int32_t m_state2WMO;
    /* 0020 */ int32_t m_state2DestructionDoodadSet;
    /* 0024 */ int32_t m_state2ImpactEffectDoodadSet;
    /* 0028 */ int32_t m_state2AmbientDoodadSet;
    /* 002C */ int32_t m_state3WMO;
    /* 0030 */ int32_t m_state3InitDoodadSet;
    /* 0034 */ int32_t m_state3AmbientDoodadSet;
    /* 0038 */ int32_t m_ejectDirection;
    /* 003C */ int32_t m_repairGroundFx;
    /* 0040 */ int32_t m_doNotHighlight;
    /* 0044 */ int32_t m_healEffect;
    /* 0048 */ int32_t m_healEffectSpeed;
};

struct DungeonEncounterRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_mapID;
    /* 0008 */ int32_t m_difficulty;
    /* 000C */ int32_t m_orderIndex;
    /* 0010 */ int32_t m_bit;
    /* 0014 */ const char* m_name;
    /* 0018 */ int32_t m_spellIconID;
};

struct DungeonMapChunkRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_mapID;
    /* 0008 */ int32_t m_wmogroupID;
    /* 000C */ int32_t m_dungeonMapID;
    /* 0010 */ float m_minZ;
};

struct DungeonMapRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_mapID;
    /* 0008 */ int32_t m_floorIndex;
    /* 000C */ float m_minX;
    /* 0010 */ float m_maxX;
    /* 0014 */ float m_minY;
    /* 0018 */ float m_maxY;
    /* 001C */ int32_t m_parentWorldMapID;
};

struct DurabilityCostsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_weaponSubClassCost [21];
    /* 0058 */ int32_t m_armorSubClassCost [8];
};

struct DurabilityQualityRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ float m_data;
};

struct EmotesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_emoteSlashCommand;
    /* 0008 */ int32_t m_animID;
    /* 000C */ int32_t m_emoteFlags;
    /* 0010 */ int32_t m_emoteSpecProc;
    /* 0014 */ int32_t m_emoteSpecProcParam;
    /* 0018 */ int32_t m_eventSoundID;
};

struct EmotesTextDataRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_text;
};

struct EmotesTextRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_emoteID;
    /* 000C */ int32_t m_emoteText [16];
};

struct EmotesTextSoundRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_emotesTextID;
    /* 0008 */ int32_t m_raceID;
    /* 000C */ int32_t m_sexID;
    /* 0010 */ int32_t m_soundID;
};

struct EnvironmentalDamageRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_enumID;
    /* 0008 */ int32_t m_visualkitID;
};

struct ExhaustionRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_xp;
    /* 0008 */ float m_factor;
    /* 000C */ float m_outdoorHours;
    /* 0010 */ float m_innHours;
    /* 0014 */ const char* m_name;
    /* 0018 */ float m_threshold;
};

struct FactionGroupRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_maskID;
    /* 0008 */ const char* m_internalName;
    /* 000C */ const char* m_name;
};

struct FactionRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_reputationIndex;
    /* 0008 */ int32_t m_reputationRaceMask [4];
    /* 0018 */ int32_t m_reputationClassMask [4];
    /* 0028 */ int32_t m_reputationBase [4];
    /* 0038 */ int32_t m_reputationFlags [4];
    /* 0048 */ int32_t m_parentFactionID;
    /* 004C */ float m_parentFactionMod [2];
    /* 0054 */ int32_t m_parentFactionCap [2];
    /* 005C */ const char* m_name;
    /* 0060 */ const char* m_description;
};

struct FactionTemplateRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_faction;
    /* 0008 */ int32_t m_flags;
    /* 000C */ int32_t m_factionGroup;
    /* 0010 */ int32_t m_friendGroup;
    /* 0014 */ int32_t m_enemyGroup;
    /* 0018 */ int32_t m_enemies [4];
    /* 0028 */ int32_t m_friend [4];
};

struct FileDataRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_filename;
    /* 0008 */ const char* m_filepath;
};

struct FootprintTexturesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_footstepFilename;
};

struct FootstepTerrainLookupRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_creatureFootstepID;
    /* 0008 */ int32_t m_terrainSoundID;
    /* 000C */ int32_t m_soundID;
    /* 0010 */ int32_t m_soundIdsplash;
};

struct GMSurveyAnswersRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_sortIndex;
    /* 0008 */ int32_t m_gmsurveyQuestionID;
    /* 000C */ const char* m_answer;
};

struct GMSurveyCurrentSurveyRec {
    /* 0000 */ int32_t m_langid;
    /* 0004 */ int32_t m_gmsurveyID;
    /* 0008 */ int32_t m_generatedID;
};

struct GMSurveyQuestionsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_question;
};

struct GMSurveySurveysRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t unk_0004 [10];
};

struct GMTicketCategoryRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_category;
};

struct GameObjectArtKitRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_textureVariation [3];
    /* 0010 */ const char* m_attachModel [4];
};

struct GameObjectDisplayInfoRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_modelName;
    /* 0008 */ int32_t m_sound [10];
    /* 0030 */ float m_geoBoxMin [3];
    /* 003C */ float m_geoBoxMax [3];
    /* 0048 */ int32_t m_objectEffectPackageID;
};

struct GameTablesRec {
    /* 0000 */ const char* m_name;
    /* 0004 */ int32_t m_numRows;
    /* 0008 */ int32_t m_numColumns;
    /* 000C */ int32_t m_generatedID;
};

struct GameTipsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_text;
};

struct GemPropertiesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_enchantID;
    /* 0008 */ int32_t m_maxcountInv;
    /* 000C */ int32_t m_maxcountItem;
    /* 0010 */ int32_t m_type;
};

struct GlueScreenEmoteRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_classID;
    /* 0008 */ int32_t m_raceID;
    /* 000C */ int32_t m_sexID;
    /* 0010 */ int32_t m_leftHandItemType;
    /* 0014 */ int32_t m_rightHandItemType;
    /* 0018 */ int32_t m_animKitID;
    /* 001C */ int32_t m_spellVisualKitID;
};

struct GlyphPropertiesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_spellID;
    /* 0008 */ int32_t m_glyphSlotFlags;
    /* 000C */ int32_t m_spellIconID;
};

struct GlyphSlotRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_type;
    /* 0008 */ int32_t m_tooltip;
};

struct GroundEffectDoodadRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_doodadpath;
    /* 0008 */ int32_t m_flags;
};

struct GroundEffectTextureRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_doodadID [4];
    /* 0014 */ int32_t m_doodadWeight [4];
    /* 0024 */ int32_t m_density;
    /* 0028 */ int32_t m_sound;
};

struct GuildPerkSpellsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_guildLevel;
    /* 0008 */ int32_t m_spellID;
};

struct HelmetGeosetVisDataRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_hideGeoset [7];
};

struct HolidayDescriptionsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_description;
};

struct HolidayNamesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
};

struct HolidaysRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_duration [10];
    /* 002C */ int32_t m_date [26];
    /* 0094 */ int32_t m_region;
    /* 0098 */ int32_t m_looping;
    /* 009C */ int32_t m_calendarFlags [10];
    /* 00C4 */ int32_t m_holidayNameID;
    /* 00C8 */ int32_t m_holidayDescriptionID;
    /* 00CC */ const char* m_textureFileName;
    /* 00D0 */ int32_t m_priority;
    /* 00D4 */ int32_t m_calendarFilterType;
    /* 00D8 */ int32_t m_flags;
};

struct ItemArmorQualityRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ float m_qualitymod [7];
    /* 0020 */ int32_t m_itemLevel;
};

struct ItemArmorShieldRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_itemLevel;
    /* 0008 */ float m_quality [7];
};

struct ItemArmorTotalRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_itemLevel;
    /* 0008 */ float m_cloth;
    /* 000C */ float m_leather;
    /* 0010 */ float m_mail;
    /* 0014 */ float m_plate;
};

struct ItemBagFamilyRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
};

struct ItemClassRec {
    /* 0000 */ int32_t m_classID;
    /* 0004 */ int32_t m_subclassMapID;
    /* 0008 */ int32_t m_flags;
    /* 000C */ const char* m_className;
    /* 0010 */ int32_t m_generatedID;
};

struct ItemCondExtCostsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_condExtendedCost;
    /* 0008 */ int32_t m_itemExtendedCostEntry;
    /* 000C */ int32_t m_arenaSeason;
};

struct ItemDamageAmmoRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ float m_quality [7];
    /* 0020 */ int32_t m_itemLevel;
};

struct ItemDamageOneHandCasterRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ float m_quality [7];
    /* 0020 */ int32_t m_itemLevel;
};

struct ItemDamageOneHandRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ float m_quality [7];
    /* 0020 */ int32_t m_itemLevel;
};

struct ItemDamageRangedRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ float m_quality [7];
    /* 0020 */ int32_t m_itemLevel;
};

struct ItemDamageThrownRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ float m_quality [7];
    /* 0020 */ int32_t m_itemLevel;
};

struct ItemDamageTwoHandCasterRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ float m_quality [7];
    /* 0020 */ int32_t m_itemLevel;
};

struct ItemDamageTwoHandRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ float m_quality [7];
    /* 0020 */ int32_t m_itemLevel;
};

struct ItemDamageWandRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ float m_quality [7];
    /* 0020 */ int32_t m_itemLevel;
};

struct ItemDisplayInfoRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_modelName [2];
    /* 000C */ const char* m_modelTexture [2];
    /* 0014 */ const char* m_inventoryIcon [2];
    /* 001C */ int32_t m_geosetGroup [3];
    /* 0028 */ int32_t m_flags;
    /* 002C */ int32_t m_spellVisualID;
    /* 0030 */ int32_t m_groupSoundIndex;
    /* 0034 */ int32_t m_helmetGeosetVisID [2];
    /* 003C */ const char* m_texture [8];
    /* 005C */ int32_t m_itemVisual;
    /* 0060 */ int32_t m_particleColorID;
};

struct ItemExtendedCostRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_honorPoints;
    /* 0008 */ int32_t m_arenaPoints;
    /* 000C */ int32_t m_arenaBracket;
    /* 0010 */ int32_t m_itemID [5];
    /* 0024 */ int32_t m_itemCount [5];
    /* 0038 */ int32_t m_requiredArenaRating;
    /* 003C */ int32_t m_itemPurchaseGroup;
};

struct ItemGroupSoundsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_sound [4];
};

struct ItemLimitCategoryRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_quantity;
    /* 000C */ int32_t m_flags;
};

struct ItemPetFoodRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
};

struct ItemPurchaseGroupRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_itemID [8];
    /* 0024 */ const char* m_name;
};

struct ItemRandomPropertiesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_enchantment [5];
    /* 001C */ const char* m_nameLang;
};

struct ItemRandomSuffixRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ const char* m_internalName;
    /* 000C */ int32_t m_enchantment [5];
    /* 0020 */ int32_t m_allocationPct [5];
};

struct ItemRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_classID;
    /* 0008 */ int32_t m_subclassID;
    /* 000C */ int32_t m_soundOverrideSubclassID;
    /* 0010 */ int32_t m_material;
    /* 0014 */ int32_t m_displayInfoID;
    /* 0018 */ int32_t m_inventoryType;
    /* 001C */ int32_t m_sheatheType;
};

struct ItemReforgeRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_sourceStat;
    /* 0008 */ float m_sourceMultiplier;
    /* 000C */ int32_t m_targetStat;
    /* 0010 */ float m_targetMultiplier;
};

struct ItemSetRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_itemID [17];
    /* 004C */ int32_t m_setSpellID [8];
    /* 006C */ int32_t m_setThreshold [8];
    /* 008C */ int32_t m_requiredSkill;
    /* 0090 */ int32_t m_requiredSkillRank;
};

struct ItemSubClassMaskRec {
    /* 0000 */ int32_t m_classID;
    /* 0004 */ int32_t m_mask;
    /* 0008 */ const char* m_name;
    /* 000C */ int32_t m_generatedID;
};

struct ItemSubClassRec {
    /* 0000 */ int32_t m_classID;
    /* 0004 */ int32_t m_subClassID;
    /* 0008 */ int32_t m_prerequisiteProficiency;
    /* 000C */ int32_t m_postrequisiteProficiency;
    /* 0010 */ int32_t m_flags;
    /* 0014 */ int32_t m_displayFlags;
    /* 0018 */ int32_t m_weaponParrySeq;
    /* 001C */ int32_t m_weaponReadySeq;
    /* 0020 */ int32_t m_weaponAttackSeq;
    /* 0024 */ int32_t m_weaponSwingSize;
    /* 0028 */ const char* m_displayName;
    /* 002C */ const char* m_verboseName;
    /* 0030 */ int32_t m_generatedID;
};

struct ItemVisualEffectsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_model;
};

struct ItemVisualsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_slot [5];
};

struct LanguageWordsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_languageID;
    /* 0008 */ const char* m_word;
};

struct LanguagesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
};

struct LfgDungeonExpansionRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_lfgID;
    /* 0008 */ int32_t m_expansionLevel;
    /* 000C */ int32_t m_randomID;
    /* 0010 */ int32_t m_hardLevelMin;
    /* 0014 */ int32_t m_hardLevelMax;
    /* 0018 */ int32_t m_targetLevelMin;
    /* 001C */ int32_t m_targetLevelMax;
};

struct LfgDungeonGroupRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_orderIndex;
    /* 000C */ int32_t m_parentGroupID;
    /* 0010 */ int32_t m_typeID;
};

struct LfgDungeonsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_minLevel;
    /* 000C */ int32_t m_maxLevel;
    /* 0010 */ int32_t m_targetLevel;
    /* 0014 */ int32_t m_targetLevelMin;
    /* 0018 */ int32_t m_targetLevelMax;
    /* 001C */ int32_t m_mapID;
    /* 0020 */ int32_t m_difficulty;
    /* 0024 */ int32_t m_flags;
    /* 0028 */ int32_t m_typeID;
    /* 002C */ int32_t m_faction;
    /* 0030 */ const char* m_textureFilename;
    /* 0034 */ int32_t m_expansionLevel;
    /* 0038 */ int32_t m_orderIndex;
    /* 003C */ int32_t m_groupID;
    /* 0040 */ const char* m_description;
};

struct LightFloatBandRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_num;
    /* 0008 */ int32_t m_time [16];
    /* 0048 */ float m_data [16];
};

struct LightIntBandRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_num;
    /* 0008 */ int32_t m_time [16];
    /* 0048 */ int32_t m_data [16];
};

struct LightParamsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_highlightSky;
    /* 0008 */ int32_t m_lightSkyboxID;
    /* 000C */ float m_glow;
    /* 0010 */ float m_waterShallowAlpha;
    /* 0014 */ float m_waterDeepAlpha;
    /* 0018 */ float m_oceanShallowAlpha;
    /* 001C */ float m_oceanDeepAlpha;
    /* 0020 */ int32_t m_flags;
};

struct LightRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_continentID;
    /* 0008 */ float m_gameCoords [3];
    /* 0014 */ float m_gameFalloffStart;
    /* 0018 */ float m_gameFalloffEnd;
    /* 001C */ int32_t m_lightParamsID [8];
};

struct LightSkyboxRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_flags;
};

struct LiquidMaterialRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_lvf;
    /* 0008 */ int32_t m_flags;
};

struct LiquidObjectRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ float m_flowDirection;
    /* 0008 */ float m_flowSpeed;
    /* 000C */ int32_t m_liquidTypeID;
    /* 0010 */ int32_t m_fishable;
    /* 0014 */ int32_t m_reflection;
};

struct LiquidTypeRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_flags;
    /* 000C */ int32_t m_soundBank;
    /* 0010 */ int32_t m_soundID;
    /* 0014 */ int32_t m_spellID;
    /* 0018 */ float m_maxDarkenDepth;
    /* 001C */ float m_fogDarkenIntensity;
    /* 0020 */ float m_ambDarkenIntensity;
    /* 0024 */ float m_dirDarkenIntensity;
    /* 0028 */ int32_t m_lightID;
    /* 002C */ float m_particleScale;
    /* 0030 */ int32_t m_particleMovement;
    /* 0034 */ int32_t m_particleTexSlots;
    /* 0038 */ int32_t m_materialID;
    /* 003C */ const char* m_texture [6];
    /* 0054 */ int32_t m_color [2];
    /* 005C */ float m_float [18];
    /* 00A4 */ int32_t m_int [4];
};

struct LoadingScreenTaxiSplinesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_pathID;
    /* 0008 */ float m_locx [8];
    /* 0028 */ float m_locy [8];
    /* 0048 */ int32_t m_legIndex;
};

struct LoadingScreensRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ const char* m_fileName;
    /* 000C */ int32_t m_hasWideScreen;
};

struct LockRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_type [8];
    /* 0024 */ int32_t m_index [8];
    /* 0044 */ int32_t m_skill [8];
    /* 0064 */ int32_t m_action [8];
};

struct LockTypeRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ const char* m_resourceName;
    /* 000C */ const char* m_verb;
    /* 0010 */ const char* m_cursorName;
};

struct M2HitRec {
    /* 0000 */ CM2Model* m_model;
    /* 0004 */ float m_tNear;
    /* 0008 */ float m_tFar;
    /* 000C */ uint32_t m_priority;
};

struct MailTemplateRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_subject;
    /* 0008 */ const char* m_body;
};

struct MapDifficultyRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_mapID;
    /* 0008 */ int32_t m_difficulty;
    /* 000C */ const char* m_message;
    /* 0010 */ int32_t m_raidDuration;
    /* 0014 */ int32_t m_maxPlayers;
    /* 0018 */ const char* m_difficultystring;
};

struct MapRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_directory;
    /* 0008 */ int32_t m_instanceType;
    /* 000C */ int32_t m_flags;
    /* 0010 */ int32_t m_pvp;
    /* 0014 */ const char* m_mapName;
    /* 0018 */ int32_t m_areaTableID;
    /* 001C */ const char* m_mapDescription0;
    /* 0020 */ const char* m_mapDescription1;
    /* 0024 */ int32_t m_loadingScreenID;
    /* 0028 */ float m_minimapIconScale;
    /* 002C */ int32_t m_corpseMapID;
    /* 0030 */ float m_corpse [2];
    /* 0038 */ int32_t m_timeOfDayOverride;
    /* 003C */ int32_t m_expansionID;
    /* 0040 */ int32_t m_raidOffset;
    /* 0044 */ int32_t m_maxPlayers;
};

struct MaterialRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_flags;
    /* 0008 */ int32_t m_foleySoundID;
    /* 000C */ int32_t m_sheatheSoundID;
    /* 0010 */ int32_t m_unsheatheSoundID;
};

struct MountTypeRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_capability [8];
};

struct MovieFileDataRec {
    /* 0000 */ int32_t m_fileDataID;
    /* 0004 */ int32_t m_resolution;
    /* 0008 */ int32_t m_generatedID;
};

struct MovieRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_filename;
    /* 0008 */ int32_t m_volume;
};

struct MovieVariationRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_movieID;
    /* 0008 */ int32_t m_fileDataID;
};

struct NPCSoundsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_soundID [4];
};

struct NameGenRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_raceID;
    /* 000C */ int32_t m_sex;
};

struct NamesProfanityRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_language;
};

struct NamesReservedRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_language;
};

struct ObjectEffectGroupRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
};

struct ObjectEffectModifierRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_inputType;
    /* 0008 */ int32_t m_mapType;
    /* 000C */ int32_t m_outputType;
    /* 0010 */ float m_param [4];
};

struct ObjectEffectPackageElemRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_objectEffectPackageID;
    /* 0008 */ int32_t m_objectEffectGroupID;
    /* 000C */ int32_t m_stateType;
};

struct ObjectEffectPackageRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
};

struct ObjectEffectRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_objectEffectGroupID;
    /* 000C */ int32_t m_triggerType;
    /* 0010 */ int32_t m_eventType;
    /* 0014 */ int32_t m_effectRecType;
    /* 0018 */ int32_t m_effectRecID;
    /* 001C */ int32_t m_attachment;
    /* 0020 */ float m_offset [3];
    /* 002C */ int32_t m_objectEffectModifierID;
};

struct OverrideSpellDataRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_spells [10];
    /* 002C */ int32_t m_flags;
};

struct PackageRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_icon;
    /* 0008 */ int32_t m_cost;
    /* 000C */ const char* m_name;
};

struct PageTextMaterialRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
};

struct PaperDollItemFrameRec {
    /* 0000 */ const char* m_itemButtonName;
    /* 0004 */ const char* m_slotIcon;
    /* 0008 */ int32_t m_slotNumber;
    /* 000C */ int32_t m_generatedID;
};

struct ParticleColorRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_start [3];
    /* 0010 */ int32_t m_mid [3];
    /* 001C */ int32_t m_end [3];
};

struct PetPersonalityRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_happinessThreshold [3];
    /* 0014 */ float m_happinessDamage [3];
};

struct PetitionTypeRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_type;
};

struct PhaseRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_mapID;
    /* 0008 */ int32_t m_parentMapID [2];
    /* 0010 */ const char* m_name;
    /* 0014 */ int32_t m_flags;
};

struct PhaseShiftZoneSoundsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_areaID;
    /* 0008 */ int32_t m_wmoareaID;
    /* 000C */ int32_t m_phaseID;
    /* 0010 */ int32_t m_phaseGroupID;
    /* 0014 */ int32_t m_phaseUseFlags;
    /* 0018 */ int32_t m_zoneIntroMusicID;
    /* 001C */ int32_t m_zoneMusicID;
    /* 0020 */ int32_t m_soundAmbienceID;
    /* 0024 */ int32_t m_soundProviderPreferencesID;
    /* 0028 */ int32_t m_uwzoneIntroMusicID;
    /* 002C */ int32_t m_uwzoneMusicID;
    /* 0030 */ int32_t m_uwsoundAmbienceID;
    /* 0034 */ int32_t m_uwsoundProviderPreferencesID;
};

struct PhaseXPhaseGroupRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_phaseID;
    /* 0008 */ int32_t m_phaseGroupID;
};

struct PowerDisplayRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_actualType;
    /* 0008 */ const char* m_globalStringBaseTag;
    /* 000C */ int8_t m_red;
    /* 000D */ int8_t m_green;
    /* 000E */ int8_t m_blue;
    /* 000F */ uint8_t unk_000F[1];
};

struct PvpDifficultyRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_mapID;
    /* 0008 */ int32_t m_rangeIndex;
    /* 000C */ int32_t m_minLevel;
    /* 0010 */ int32_t m_maxLevel;
    /* 0014 */ int32_t m_difficulty;
};

struct QuestFactionRewardRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_difficulty [10];
};

struct QuestInfoRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_infoName;
};

struct QuestSortRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_sortName;
};

struct QuestXPRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_difficulty [10];
};

struct RandPropPointsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_epic [5];
    /* 0018 */ int32_t m_superior [5];
    /* 002C */ int32_t m_good [5];
};

struct ResearchBranchRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_researchFieldID;
    /* 000C */ int32_t m_currencyID;
    /* 0010 */ const char* m_texture;
    /* 0014 */ int32_t m_itemID;
};

struct ResistancesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_flags;
    /* 0008 */ int32_t m_fizzleSoundID;
    /* 000C */ const char* m_name;
};

struct ScalingStatDistributionRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_statID [10];
    /* 002C */ int32_t m_bonus [10];
    /* 0054 */ int32_t m_maxlevel;
};

struct ScalingStatValuesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_charlevel;
    /* 0008 */ int32_t m_shoulderBudget;
    /* 000C */ int32_t m_trinketBudget;
    /* 0010 */ int32_t m_weaponBudget1h;
    /* 0014 */ int32_t m_rangedBudget;
    /* 0018 */ int32_t m_clothShoulderArmor;
    /* 001C */ int32_t m_leatherShoulderArmor;
    /* 0020 */ int32_t m_mailShoulderArmor;
    /* 0024 */ int32_t m_plateShoulderArmor;
    /* 0028 */ int32_t m_weaponDps1h;
    /* 002C */ int32_t m_weaponDps2h;
    /* 0030 */ int32_t m_spellcasterDps1h;
    /* 0034 */ int32_t m_spellcasterDps2h;
    /* 0038 */ int32_t m_rangedDps;
    /* 003C */ int32_t m_wandDps;
    /* 0040 */ int32_t m_spellPower;
    /* 0044 */ int32_t m_primaryBudget;
    /* 0048 */ int32_t m_tertiaryBudget;
    /* 004C */ int32_t m_clothCloakArmor;
    /* 0050 */ int32_t m_clothChestArmor;
    /* 0054 */ int32_t m_leatherChestArmor;
    /* 0058 */ int32_t m_mailChestArmor;
    /* 005C */ int32_t m_plateChestArmor;
};

struct ScreenEffectRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_effect;
    /* 000C */ int32_t m_param [4];
    /* 001C */ int32_t m_lightParamsID;
    /* 0020 */ int32_t m_soundAmbienceID;
    /* 0024 */ int32_t m_zoneMusicID;
};

struct ServerMessagesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_text;
};

struct SheatheSoundLookupsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_classID;
    /* 0008 */ int32_t m_subclassID;
    /* 000C */ int32_t m_material;
    /* 0010 */ int32_t m_checkMaterial;
    /* 0014 */ int32_t m_sheatheSound;
    /* 0018 */ int32_t m_unsheatheSound;
};

struct SkillCostsDataRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_skillCostsID;
    /* 0008 */ int32_t m_cost [3];
};

struct SkillLineAbilityRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_skillLine;
    /* 0008 */ int32_t m_spell;
    /* 000C */ int32_t m_raceMask;
    /* 0010 */ int32_t m_classMask;
    /* 0014 */ int32_t m_excludeRace;
    /* 0018 */ int32_t m_excludeClass;
    /* 001C */ int32_t m_minSkillLineRank;
    /* 0020 */ int32_t m_supercededBySpell;
    /* 0024 */ int32_t m_acquireMethod;
    /* 0028 */ int32_t m_trivialSkillLineRankHigh;
    /* 002C */ int32_t m_trivialSkillLineRankLow;
    /* 0030 */ int32_t m_characterPoints [2];
};

struct SkillLineCategoryRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_sortIndex;
};

struct SkillLineRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_categoryID;
    /* 0008 */ int32_t m_skillCostsID;
    /* 000C */ const char* m_displayName;
    /* 0010 */ const char* m_description;
    /* 0014 */ int32_t m_spellIconID;
    /* 0018 */ const char* m_alternateVerb;
    /* 001C */ int32_t m_canLink;
};

struct SkillRaceClassInfoRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_skillID;
    /* 0008 */ int32_t m_raceMask;
    /* 000C */ int32_t m_classMask;
    /* 0010 */ int32_t m_flags;
    /* 0014 */ int32_t m_minLevel;
    /* 0018 */ int32_t m_skillTierID;
    /* 001C */ int32_t m_skillCostIndex;
};

struct SkillTiersRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_cost [16];
    /* 0044 */ int32_t m_value [16];
};

struct SoundAmbienceRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_ambienceID [2];
};

struct SoundEmittersRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ float m_position [3];
    /* 0010 */ float m_direction [3];
    /* 001C */ int32_t m_soundEntryAdvancedID;
    /* 0020 */ int32_t m_mapID;
    /* 0024 */ const char* m_name;
};

struct SoundEntriesAdvancedRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_soundEntryID;
    /* 0008 */ float m_innerRadius2d;
    /* 000C */ int32_t m_timeA;
    /* 0010 */ int32_t m_timeB;
    /* 0014 */ int32_t m_timeC;
    /* 0018 */ int32_t m_timeD;
    /* 001C */ int32_t m_randomOffsetRange;
    /* 0020 */ int32_t m_usage;
    /* 0024 */ int32_t m_timeIntervalMin;
    /* 0028 */ int32_t m_timeIntervalMax;
    /* 002C */ int32_t m_volumeSliderCategory;
    /* 0030 */ float m_duckToSfx;
    /* 0034 */ float m_duckToMusic;
    /* 0038 */ float m_duckToAmbience;
    /* 003C */ float m_innerRadiusOfInfluence;
    /* 0040 */ float m_outerRadiusOfInfluence;
    /* 0044 */ int32_t m_timeToDuck;
    /* 0048 */ int32_t m_timeToUnduck;
    /* 004C */ float m_insideAngle;
    /* 0050 */ float m_outsideAngle;
    /* 0054 */ float m_outsideVolume;
    /* 0058 */ float m_outerRadius2d;
    /* 005C */ const char* m_name;
};

struct SoundEntriesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_soundType;
    /* 0008 */ const char* m_name;
    /* 000C */ const char* m_file [10];
    /* 0034 */ int32_t m_freq [10];
    /* 005C */ const char* m_directoryBase;
    /* 0060 */ float m_volumeFloat;
    /* 0064 */ int32_t m_flags;
    /* 0068 */ float m_minDistance;
    /* 006C */ float m_distanceCutoff;
    /* 0070 */ int32_t m_eaxdef;
    /* 0074 */ int32_t m_soundEntriesAdvancedID;
};

struct SoundFilterElemRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_soundFilterID;
    /* 0008 */ int32_t m_orderIndex;
    /* 000C */ int32_t m_filterType;
    /* 0010 */ float m_params [9];
};

struct SoundFilterRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
};

struct SoundProviderPreferencesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_description;
    /* 0008 */ int32_t m_flags;
    /* 000C */ int32_t m_eaxenvironmentSelection;
    /* 0010 */ float m_eaxdecayTime;
    /* 0014 */ float m_eax2EnvironmentSize;
    /* 0018 */ float m_eax2EnvironmentDiffusion;
    /* 001C */ int32_t m_eax2Room;
    /* 0020 */ int32_t m_eax2RoomHf;
    /* 0024 */ float m_eax2DecayHfratio;
    /* 0028 */ int32_t m_eax2Reflections;
    /* 002C */ float m_eax2ReflectionsDelay;
    /* 0030 */ int32_t m_eax2Reverb;
    /* 0034 */ float m_eax2ReverbDelay;
    /* 0038 */ float m_eax2RoomRolloff;
    /* 003C */ float m_eax2AirAbsorption;
    /* 0040 */ int32_t m_eax3RoomLf;
    /* 0044 */ float m_eax3DecayLfratio;
    /* 0048 */ float m_eax3EchoTime;
    /* 004C */ float m_eax3EchoDepth;
    /* 0050 */ float m_eax3ModulationTime;
    /* 0054 */ float m_eax3ModulationDepth;
    /* 0058 */ float m_eax3Hfreference;
    /* 005C */ float m_eax3Lfreference;
};

struct SoundSamplePreferencesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_field0603592001;
    /* 0008 */ int32_t m_field0603592002;
    /* 000C */ int32_t m_eax2SampleRoom;
    /* 0010 */ int32_t m_field0603592004;
    /* 0014 */ int32_t m_field0603592005;
    /* 0018 */ float m_field0603592006;
    /* 001C */ int32_t m_field0603592007;
    /* 0020 */ float m_eax2SampleOcclusionLfratio;
    /* 0024 */ float m_eax2SampleOcclusionRoomRatio;
    /* 0028 */ int32_t m_field0603592010;
    /* 002C */ float m_eax1EffectLevel;
    /* 0030 */ int32_t m_field0603592012;
    /* 0034 */ float m_field0603592013;
    /* 0038 */ float m_eax3SampleExclusion;
    /* 003C */ float m_field0603592015;
    /* 0040 */ int32_t m_field0603592016;
};

struct SoundWaterTypeRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_soundType;
    /* 0008 */ int32_t m_soundSubtype;
    /* 000C */ int32_t m_soundID;
};

struct SpamMessagesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_text;
};

struct SpellAuraOptionsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_cumulativeAura;
    /* 0008 */ int32_t m_procChance;
    /* 000C */ int32_t m_procCharges;
    /* 0010 */ int32_t m_procTypeMask;
};

struct SpellAuraRestrictionsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_casterAuraState;
    /* 0008 */ int32_t m_targetAuraState;
    /* 000C */ int32_t m_excludeCasterAuraState;
    /* 0010 */ int32_t m_excludeTargetAuraState;
    /* 0014 */ int32_t m_casterAuraSpell;
    /* 0018 */ int32_t m_targetAuraSpell;
    /* 001C */ int32_t m_excludeCasterAuraSpell;
    /* 0020 */ int32_t m_excludeTargetAuraSpell;
};

struct SpellCastTimesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_base;
    /* 0008 */ int32_t m_perLevel;
    /* 000C */ int32_t m_minimum;
};

struct SpellCastingRequirementsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_facingCasterFlags;
    /* 0008 */ int32_t m_minFactionID;
    /* 000C */ int32_t m_minReputation;
    /* 0010 */ int32_t m_requiredAreasID;
    /* 0014 */ int32_t m_requiredAuraVision;
    /* 0018 */ int32_t m_requiresSpellFocus;
};

struct SpellCategoriesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_category;
    /* 0008 */ int32_t m_defenseType;
    /* 000C */ int32_t m_dispelType;
    /* 0010 */ int32_t m_mechanic;
    /* 0014 */ int32_t m_preventionType;
    /* 0018 */ int32_t m_startRecoveryCategory;
};

struct SpellCategoryRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_flags;
};

struct SpellChainEffectsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ float m_avgSegLen;
    /* 0008 */ float m_width;
    /* 000C */ float m_noiseScale;
    /* 0010 */ float m_texCoordScale;
    /* 0014 */ int32_t m_segDuration;
    /* 0018 */ int32_t m_segDelay;
    /* 001C */ const char* m_texture;
    /* 0020 */ int32_t m_flags;
    /* 0024 */ int32_t m_jointCount;
    /* 0028 */ float m_jointOffsetRadius;
    /* 002C */ int32_t m_jointsPerMinorJoint;
    /* 0030 */ int32_t m_minorJointsPerMajorJoint;
    /* 0034 */ float m_minorJointScale;
    /* 0038 */ float m_majorJointScale;
    /* 003C */ float m_jointMoveSpeed;
    /* 0040 */ float m_jointSmoothness;
    /* 0044 */ float m_minDurationBetweenJointJumps;
    /* 0048 */ float m_maxDurationBetweenJointJumps;
    /* 004C */ float m_waveHeight;
    /* 0050 */ float m_waveFreq;
    /* 0054 */ float m_waveSpeed;
    /* 0058 */ float m_minWaveAngle;
    /* 005C */ float m_maxWaveAngle;
    /* 0060 */ float m_minWaveSpin;
    /* 0064 */ float m_maxWaveSpin;
    /* 0068 */ float m_arcHeight;
    /* 006C */ float m_minArcAngle;
    /* 0070 */ float m_maxArcAngle;
    /* 0074 */ float m_minArcSpin;
    /* 0078 */ float m_maxArcSpin;
    /* 007C */ float m_delayBetweenEffects;
    /* 0080 */ float m_minFlickerOnDuration;
    /* 0084 */ float m_maxFlickerOnDuration;
    /* 0088 */ float m_minFlickerOffDuration;
    /* 008C */ float m_maxFlickerOffDuration;
    /* 0090 */ float m_pulseSpeed;
    /* 0094 */ float m_pulseOnLength;
    /* 0098 */ float m_pulseFadeLength;
    /* 009C */ int8_t m_alpha;
    /* 009D */ int8_t m_red;
    /* 009E */ int8_t m_green;
    /* 009F */ int8_t m_blue;
    /* 00A0 */ int8_t m_blendMode;
    /* 00A1 */ uint8_t unk_00A1[3];
    /* 00A4 */ const char* m_combo;
    /* 00A8 */ int32_t m_renderLayer;
    /* 00AC */ float m_textureLength;
    /* 00B0 */ float m_wavePhase;
};

struct SpellClassOptionsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_modalNextSpell;
    /* 0008 */ int32_t m_spellClassMask [3];
    /* 0014 */ int32_t m_spellClassSet;
};

struct SpellCooldownsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_categoryRecoveryTime;
    /* 0008 */ int32_t m_recoveryTime;
    /* 000C */ int32_t m_startRecoveryTime;
};

struct SpellDescriptionVariablesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_variables;
};

struct SpellDifficultyRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_difficultySpellID [4];
};

struct SpellDispelTypeRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_mask;
    /* 000C */ int32_t m_immunityPossible;
    /* 0010 */ const char* m_internalName;
};

struct SpellDurationRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_duration;
    /* 0008 */ int32_t m_durationPerLevel;
    /* 000C */ int32_t m_maxDuration;
};

struct SpellEffectCameraShakesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_cameraShake [3];
};

struct SpellEffectRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_effect;
    /* 0008 */ float m_effectAmplitude;
    /* 000C */ int32_t m_effectAura;
    /* 0010 */ int32_t m_effectAuraPeriod;
    /* 0014 */ int32_t m_effectBasePoints;
    /* 0018 */ float m_effectBonusCoefficient;
    /* 001C */ float m_effectChainAmplitude;
    /* 0020 */ int32_t m_effectChainTargets;
    /* 0024 */ int32_t m_effectDieSides;
    /* 0028 */ int32_t m_effectItemType;
    /* 002C */ int32_t m_effectMechanic;
    /* 0030 */ int32_t m_effectMiscValue [2];
    /* 0038 */ float m_effectPointsPerResource;
    /* 003C */ int32_t m_effectRadiusIndex [2];
    /* 0044 */ float m_effectRealPointsPerLevel;
    /* 0048 */ int32_t m_effectSpellClassMask [3];
    /* 0054 */ int32_t m_effectTriggerSpell;
    /* 0058 */ int32_t m_implicitTarget [2];
    /* 0060 */ int32_t m_spellID;
    /* 0064 */ int32_t m_effectIndex;
};

struct SpellEquippedItemsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_equippedItemClass;
    /* 0008 */ int32_t m_equippedItemInvTypes;
    /* 000C */ int32_t m_equippedItemSubclass;
};

struct SpellFocusObjectRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
};

struct SpellIconRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_textureFilename;
};

struct SpellInterruptsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_auraInterruptFlags [2];
    /* 000C */ int32_t m_channelInterruptFlags [2];
    /* 0014 */ int32_t m_interruptFlags;
};

struct SpellItemEnchantmentConditionRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int8_t m_ltOperandType [5];
    /* 0009 */ uint8_t unk_0009[3];
    /* 000C */ int32_t m_ltOperand [5];
    /* 0020 */ int8_t m_operator [5];
    /* 0025 */ int8_t m_rtOperandType [5];
    /* 002A */ uint8_t unk_002A[2];
    /* 002C */ int32_t m_rtOperand [5];
    /* 0040 */ int8_t m_logic [5];
    /* 0045 */ uint8_t unk_0045[3];
};

struct SpellItemEnchantmentRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_charges;
    /* 0008 */ int32_t m_effect [3];
    /* 0014 */ int32_t m_effectPointsMin [3];
    /* 0020 */ int32_t m_effectPointsMax [3];
    /* 002C */ int32_t m_effectArg [3];
    /* 0038 */ const char* m_name;
    /* 003C */ int32_t m_itemVisual;
    /* 0040 */ int32_t m_flags;
    /* 0044 */ int32_t m_srcItemID;
    /* 0048 */ int32_t m_conditionID;
    /* 004C */ int32_t m_requiredSkillID;
    /* 0050 */ int32_t m_requiredSkillRank;
    /* 0054 */ int32_t m_minLevel;
};

struct SpellLevelsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_baseLevel;
    /* 0008 */ int32_t m_maxLevel;
    /* 000C */ int32_t m_spellLevel;
};

struct SpellMechanicRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_stateName;
};

struct SpellMissileMotionRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ const char* m_scriptBody;
    /* 000C */ int32_t m_flags;
    /* 0010 */ int32_t m_missileCount;
};

struct SpellMissileRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_flags;
    /* 0008 */ float m_defaultPitchMin;
    /* 000C */ float m_defaultPitchMax;
    /* 0010 */ float m_defaultSpeedMin;
    /* 0014 */ float m_defaultSpeedMax;
    /* 0018 */ float m_randomizeFacingMin;
    /* 001C */ float m_randomizeFacingMax;
    /* 0020 */ float m_randomizePitchMin;
    /* 0024 */ float m_randomizePitchMax;
    /* 0028 */ float m_randomizeSpeedMin;
    /* 002C */ float m_randomizeSpeedMax;
    /* 0030 */ float m_gravity;
    /* 0034 */ float m_maxDuration;
    /* 0038 */ float m_collisionRadius;
};

struct SpellPowerRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_manaCost;
    /* 0008 */ int32_t m_manaCostPerLevel;
    /* 000C */ int32_t m_powerCost;
    /* 0010 */ int32_t m_manaPerSecond;
    /* 0014 */ int32_t m_powerDisplayID;
};

struct SpellRadiusRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ float m_radius;
    /* 0008 */ float m_radiusPerLevel;
    /* 000C */ float m_radiusMax;
};

struct SpellRangeRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ float m_rangeMin [2];
    /* 000C */ float m_rangeMax [2];
    /* 0014 */ int32_t m_flags;
    /* 0018 */ const char* m_displayName;
    /* 001C */ const char* m_displayNameShort;
};

struct SpellReagentsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_reagent [8];
    /* 0024 */ int32_t m_reagentCount [8];
};

class SpellRec {
    public:
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_category;
    /* 0008 */ int32_t m_dispelType;
    /* 000C */ int32_t m_mechanic;
    /* 0010 */ int32_t m_attributes;
    /* 0014 */ int32_t m_attributesEx;
    /* 0018 */ int32_t m_attributesExB;
    /* 001C */ int32_t m_attributesExC;
    /* 0020 */ int32_t m_attributesExD;
    /* 0024 */ int32_t m_attributesExE;
    /* 0028 */ int32_t m_attributesExF;
    /* 002C */ int32_t m_attributesExG;
    /* 0030 */ int32_t m_shapeshiftMask [2];
    /* 0038 */ int32_t m_shapeshiftExclude [2];
    /* 0040 */ int32_t m_targets;
    /* 0044 */ int32_t m_targetCreatureType;
    /* 0048 */ int32_t m_requiresSpellFocus;
    /* 004C */ int32_t m_facingCasterFlags;
    /* 0050 */ int32_t m_casterAuraState;
    /* 0054 */ int32_t m_targetAuraState;
    /* 0058 */ int32_t m_excludeCasterAuraState;
    /* 005C */ int32_t m_excludeTargetAuraState;
    /* 0060 */ int32_t m_casterAuraSpell;
    /* 0064 */ int32_t m_targetAuraSpell;
    /* 0068 */ int32_t m_excludeCasterAuraSpell;
    /* 006C */ int32_t m_excludeTargetAuraSpell;
    /* 0070 */ int32_t m_castingTimeIndex;
    /* 0074 */ int32_t m_recoveryTime;
    /* 0078 */ int32_t m_categoryRecoveryTime;
    /* 007C */ int32_t m_interruptFlags;
    /* 0080 */ int32_t m_auraInterruptFlags;
    /* 0084 */ int32_t m_channelInterruptFlags;
    /* 0088 */ int32_t m_procTypeMask;
    /* 008C */ int32_t m_procChance;
    /* 0090 */ int32_t m_procCharges;
    /* 0094 */ int32_t m_maxLevel;
    /* 0098 */ int32_t m_baseLevel;
    /* 009C */ int32_t m_spellLevel;
    /* 00A0 */ int32_t m_durationIndex;
    /* 00A4 */ int32_t m_powerType;
    /* 00A8 */ int32_t m_manaCost;
    /* 00AC */ int32_t m_manaCostPerLevel;
    /* 00B0 */ int32_t m_manaPerSecond;
    /* 00B4 */ int32_t m_manaPerSecondPerLevel;
    /* 00B8 */ int32_t m_rangeIndex;
    /* 00BC */ float m_speed;
    /* 00C0 */ int32_t m_modalNextSpell;
    /* 00C4 */ int32_t m_cumulativeAura;
    /* 00C8 */ int32_t m_totem [2];
    /* 00D0 */ int32_t m_reagent [8];
    /* 00F0 */ int32_t m_reagentCount [8];
    /* 0110 */ int32_t m_equippedItemClass;
    /* 0114 */ int32_t m_equippedItemSubclass;
    /* 0118 */ int32_t m_equippedItemInvTypes;
    /* 011C */ int32_t m_effect [3];
    /* 0128 */ int32_t m_effectDieSides [3];
    /* 0134 */ float m_effectRealPointsPerLevel [3];
    /* 0140 */ int32_t m_effectBasePoints [3];
    /* 014C */ int32_t m_effectMechanic [3];
    /* 0158 */ int32_t m_implicitTargetA [3];
    /* 0164 */ int32_t m_implicitTargetB [3];
    /* 0170 */ int32_t m_effectRadiusIndex [3];
    /* 017C */ int32_t m_effectAura [3];
    /* 0188 */ int32_t m_effectAuraPeriod [3];
    /* 0194 */ float m_effectAmplitude [3];
    /* 01A0 */ int32_t m_effectChainTargets [3];
    /* 01AC */ int32_t m_effectItemType [3];
    /* 01B8 */ int32_t m_effectMiscValue [3];
    /* 01C4 */ int32_t m_effectMiscValueB [3];
    /* 01D0 */ int32_t m_effectTriggerSpell [3];
    /* 01DC */ float m_effectPointsPerCombo [3];
    /* 01E8 */ int32_t m_effectSpellClassMaskA [3];
    /* 01F4 */ int32_t m_effectSpellClassMaskB [3];
    /* 0200 */ int32_t m_effectSpellClassMaskC [3];
    /* 020C */ int32_t m_spellVisualID [2];
    /* 0214 */ int32_t m_spellIconID;
    /* 0218 */ int32_t m_activeIconID;
    /* 021C */ int32_t m_spellPriority;
    /* 0220 */ const char* m_name;
    /* 0224 */ const char* m_nameSubtext;
    /* 0228 */ const char* m_description;
    /* 022C */ const char* m_auraDescription;
    /* 0230 */ int32_t m_manaCostPct;
    /* 0234 */ int32_t m_startRecoveryCategory;
    /* 0238 */ int32_t m_startRecoveryTime;
    /* 023C */ int32_t m_maxTargetLevel;
    /* 0240 */ int32_t m_spellClassSet;
    /* 0244 */ int32_t m_spellClassMask [3];
    /* 0250 */ int32_t m_maxTargets;
    /* 0254 */ int32_t m_defenseType;
    /* 0258 */ int32_t m_preventionType;
    /* 025C */ int32_t m_stanceBarOrder;
    /* 0260 */ float m_effectChainAmplitude [3];
    /* 026C */ int32_t m_minFactionID;
    /* 0270 */ int32_t m_minReputation;
    /* 0274 */ int32_t m_requiredAuraVision;
    /* 0278 */ int32_t m_requiredTotemCategoryID [2];
    /* 0280 */ int32_t m_requiredAreasID;
    /* 0284 */ int32_t m_schoolMask;
    /* 0288 */ int32_t m_runeCostID;
    /* 028C */ int32_t m_spellMissileID;
    /* 0290 */ int32_t m_powerDisplayID;
    /* 0294 */ float m_effectBonusCoefficient [3];
    /* 02A0 */ int32_t m_descriptionVariablesID;
    /* 02A4 */ int32_t m_difficulty;
};

struct SpellRuneCostRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_blood;
    /* 0008 */ int32_t m_unholy;
    /* 000C */ int32_t m_frost;
    /* 0010 */ int32_t m_runicPower;
};

struct SpellScalingRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_castTimeMin;
    /* 0008 */ int32_t m_castTimeMax;
    /* 000C */ int32_t m_castTimeMaxLevel;
    /* 0010 */ int32_t m_class;
    /* 0014 */ float m_coefficient [3];
    /* 0020 */ float m_variance [3];
    /* 002C */ float m_comboPointsCoefficient [3];
    /* 0038 */ float m_nerfFactor;
    /* 003C */ int32_t m_nerfMaxLevel;
};

struct SpellShapeshiftFormRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_bonusActionBar;
    /* 0008 */ const char* m_name;
    /* 000C */ int32_t m_flags;
    /* 0010 */ int32_t m_creatureType;
    /* 0014 */ int32_t m_attackIconID;
    /* 0018 */ int32_t m_combatRoundTime;
    /* 001C */ int32_t m_creatureDisplayID [4];
    /* 002C */ int32_t m_presetSpellID [8];
};

struct SpellShapeshiftRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_shapeshiftExclude [2];
    /* 000C */ int32_t m_shapeshiftMask [2];
    /* 0014 */ int32_t m_stanceBarOrder;
};

struct SpellTargetRestrictionsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_maxTargets;
    /* 0008 */ int32_t m_maxTargetLevel;
    /* 000C */ int32_t m_targetCreatureType;
    /* 0010 */ int32_t m_targets;
};

struct SpellTotemsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_requiredTotemCategoryID [2];
    /* 000C */ int32_t m_totem [2];
};

struct SpellVisualEffectNameRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ const char* m_fileName;
    /* 000C */ float m_areaEffectSize;
    /* 0010 */ float m_scale;
    /* 0014 */ float m_minAllowedScale;
    /* 0018 */ float m_maxAllowedScale;
};

struct SpellVisualKitAreaModelRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_enumID;
};

struct SpellVisualKitModelAttachRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_parentSpellVisualKitID;
    /* 0008 */ int32_t m_spellVisualEffectNameID;
    /* 000C */ int32_t m_attachmentID;
    /* 0010 */ float m_offset [3];
    /* 001C */ float m_yaw;
    /* 0020 */ float m_pitch;
    /* 0024 */ float m_roll;
};

struct SpellVisualKitRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_startAnimID;
    /* 0008 */ int32_t m_animID;
    /* 000C */ int32_t m_headEffect;
    /* 0010 */ int32_t m_chestEffect;
    /* 0014 */ int32_t m_baseEffect;
    /* 0018 */ int32_t m_leftHandEffect;
    /* 001C */ int32_t m_rightHandEffect;
    /* 0020 */ int32_t m_breathEffect;
    /* 0024 */ int32_t m_leftWeaponEffect;
    /* 0028 */ int32_t m_rightWeaponEffect;
    /* 002C */ int32_t m_specialEffect [3];
    /* 0038 */ int32_t m_worldEffect;
    /* 003C */ int32_t m_soundID;
    /* 0040 */ int32_t m_shakeID;
    /* 0044 */ int32_t m_charProc [4];
    /* 0054 */ float m_charParamZero [4];
    /* 0064 */ float m_charParamOne [4];
    /* 0074 */ float m_charParamTwo [4];
    /* 0084 */ float m_charParamThree [4];
    /* 0094 */ int32_t m_flags;
};

struct SpellVisualPrecastTransitionsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_precastLoadAnimName;
    /* 0008 */ const char* m_precastHoldAnimName;
};

struct SpellVisualRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_precastKit;
    /* 0008 */ int32_t m_castKit;
    /* 000C */ int32_t m_impactKit;
    /* 0010 */ int32_t m_stateKit;
    /* 0014 */ int32_t m_stateDoneKit;
    /* 0018 */ int32_t m_channelKit;
    /* 001C */ int32_t m_hasMissile;
    /* 0020 */ int32_t m_missileModel;
    /* 0024 */ int32_t m_missilePathType;
    /* 0028 */ int32_t m_missileDestinationAttachment;
    /* 002C */ int32_t m_missileSound;
    /* 0030 */ int32_t m_animEventSoundID;
    /* 0034 */ int32_t m_flags;
    /* 0038 */ int32_t m_casterImpactKit;
    /* 003C */ int32_t m_targetImpactKit;
    /* 0040 */ int32_t m_missileAttachment;
    /* 0044 */ int32_t m_missileFollowGroundHeight;
    /* 0048 */ int32_t m_missileFollowGroundDropSpeed;
    /* 004C */ int32_t m_missileFollowGroundApproach;
    /* 0050 */ int32_t m_missileFollowGroundFlags;
    /* 0054 */ int32_t m_missileMotion;
    /* 0058 */ int32_t m_missileTargetingKit;
    /* 005C */ int32_t m_instantAreaKit;
    /* 0060 */ int32_t m_impactAreaKit;
    /* 0064 */ int32_t m_persistentAreaKit;
    /* 0068 */ float m_missileCastOffset [3];
    /* 0074 */ float m_missileImpactOffset [3];
};

struct StableSlotPricesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_cost;
};

struct StationeryRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_itemID;
    /* 0008 */ const char* m_texture;
    /* 000C */ int32_t m_flags;
};

struct StringLookupsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_string;
};

struct SummonPropertiesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_control;
    /* 0008 */ int32_t m_faction;
    /* 000C */ int32_t m_title;
    /* 0010 */ int32_t m_slot;
    /* 0014 */ int32_t m_flags;
};

struct TalentRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_tabID;
    /* 0008 */ int32_t m_tierID;
    /* 000C */ int32_t m_columnIndex;
    /* 0010 */ int32_t m_spellRank [9];
    /* 0034 */ int32_t m_prereqTalent [3];
    /* 0040 */ int32_t m_prereqRank [3];
    /* 004C */ int32_t m_flags;
    /* 0050 */ int32_t m_requiredSpellID;
    /* 0054 */ int32_t m_categoryMask [2];
};

struct TalentTabRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_spellIconID;
    /* 000C */ int32_t m_raceMask;
    /* 0010 */ int32_t m_classMask;
    /* 0014 */ int32_t m_categoryEnumID;
    /* 0018 */ int32_t m_orderIndex;
    /* 001C */ const char* m_backgroundFile;
};

struct TaxiNodesRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_continentID;
    /* 0008 */ float m_pos [3];
    /* 0014 */ const char* m_name;
    /* 0018 */ int32_t m_mountCreatureID [2];
};

struct TaxiPathNodeRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_pathID;
    /* 0008 */ int32_t m_nodeIndex;
    /* 000C */ int32_t m_continentID;
    /* 0010 */ float m_loc [3];
    /* 001C */ int32_t m_flags;
    /* 0020 */ int32_t m_delay;
    /* 0024 */ int32_t m_arrivalEventID;
    /* 0028 */ int32_t m_departureEventID;
};

struct TaxiPathRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_fromTaxiNode;
    /* 0008 */ int32_t m_toTaxiNode;
    /* 000C */ int32_t m_cost;
};

struct TeamContributionPointsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ float m_data;
};

struct TerrainMaterialRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_shader;
    /* 000C */ const char* m_envMapPath;
};

struct TerrainTypeRec {
    /* 0000 */ int32_t m_terrainID;
    /* 0004 */ const char* m_terrainDesc;
    /* 0008 */ int32_t m_footstepSprayRun;
    /* 000C */ int32_t m_footstepSprayWalk;
    /* 0010 */ int32_t m_soundID;
    /* 0014 */ int32_t m_flags;
    /* 0018 */ int32_t m_generatedID;
};

struct TerrainTypeSoundsRec {
    /* 0000 */ int32_t m_id;
};

struct TotemCategoryRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_totemCategoryType;
    /* 000C */ int32_t m_totemCategoryMask;
};

struct TransportAnimationRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_transportID;
    /* 0008 */ int32_t m_timeIndex;
    /* 000C */ float m_pos [3];
    /* 0018 */ int32_t m_sequenceID;
};

struct TransportPhysicsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ float m_waveAmp;
    /* 0008 */ float m_waveTimeScale;
    /* 000C */ float m_rollAmp;
    /* 0010 */ float m_rollTimeScale;
    /* 0014 */ float m_pitchAmp;
    /* 0018 */ float m_pitchTimeScale;
    /* 001C */ float m_maxBank;
    /* 0020 */ float m_maxBankTurnSpeed;
    /* 0024 */ float m_speedDampThresh;
    /* 0028 */ float m_speedDamp;
};

struct TransportRotationRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_gameObjectsID;
    /* 0008 */ int32_t m_timeIndex;
    /* 000C */ float m_rot [4];
};

struct UISoundLookupsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_soundID;
    /* 0008 */ const char* m_soundName;
};

struct UnitBloodLevelsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_violencelevel [3];
};

struct UnitBloodRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_combatBloodSpurtFront [2];
    /* 000C */ int32_t m_combatBloodSpurtBack [2];
    /* 0014 */ const char* m_groundBlood [5];
};

struct VehicleRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_flags;
    /* 0008 */ float m_turnSpeed;
    /* 000C */ float m_pitchSpeed;
    /* 0010 */ float m_pitchMin;
    /* 0014 */ float m_pitchMax;
    /* 0018 */ int32_t m_seatID [8];
    /* 0038 */ float m_mouseLookOffsetPitch;
    /* 003C */ float m_cameraFadeDistScalarMin;
    /* 0040 */ float m_cameraFadeDistScalarMax;
    /* 0044 */ float m_cameraPitchOffset;
    /* 0048 */ float m_facingLimitRight;
    /* 004C */ float m_facingLimitLeft;
    /* 0050 */ float m_msslTrgtTurnLingering;
    /* 0054 */ float m_msslTrgtPitchLingering;
    /* 0058 */ float m_msslTrgtMouseLingering;
    /* 005C */ float m_msslTrgtEndOpacity;
    /* 0060 */ float m_msslTrgtArcSpeed;
    /* 0064 */ float m_msslTrgtArcRepeat;
    /* 0068 */ float m_msslTrgtArcWidth;
    /* 006C */ float m_msslTrgtImpactRadius [2];
    /* 0074 */ const char* m_msslTrgtArcTexture;
    /* 0078 */ const char* m_msslTrgtImpactTexture;
    /* 007C */ const char* m_msslTrgtImpactModel [2];
    /* 0084 */ float m_cameraYawOffset;
    /* 0088 */ int32_t m_uiLocomotionType;
    /* 008C */ float m_msslTrgtImpactTexRadius;
    /* 0090 */ int32_t m_vehicleUiindicatorID;
    /* 0094 */ int32_t m_powerDisplayID [3];
};

struct VehicleSeatRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ float m_field33512213001;
    /* 0008 */ int32_t m_attachmentID;
    /* 000C */ float m_attachmentOffset [3];
    /* 0018 */ float m_field33512213004;
    /* 001C */ float m_enterSpeed;
    /* 0020 */ float m_enterGravity;
    /* 0024 */ float m_enterMinDuration;
    /* 0028 */ float m_enterMaxDuration;
    /* 002C */ float m_enterMinArcHeight;
    /* 0030 */ float m_enterMaxArcHeight;
    /* 0034 */ int32_t m_enterAnimStart;
    /* 0038 */ int32_t m_enterAnimLoop;
    /* 003C */ int32_t m_rideAnimStart;
    /* 0040 */ int32_t m_rideAnimLoop;
    /* 0044 */ int32_t m_rideUpperAnimStart;
    /* 0048 */ int32_t m_rideUpperAnimLoop;
    /* 004C */ float m_field33512213017;
    /* 0050 */ float m_exitSpeed;
    /* 0054 */ float m_exitGravity;
    /* 0058 */ float m_exitMinDuration;
    /* 005C */ float m_exitMaxDuration;
    /* 0060 */ float m_exitMinArcHeight;
    /* 0064 */ float m_exitMaxArcHeight;
    /* 0068 */ int32_t m_exitAnimStart;
    /* 006C */ int32_t m_exitAnimLoop;
    /* 0070 */ int32_t m_exitAnimEnd;
    /* 0074 */ float m_field33512213027;
    /* 0078 */ float m_passengerPitch;
    /* 007C */ float m_field33512213029;
    /* 0080 */ int32_t m_passengerAttachmentID;
    /* 0084 */ int32_t m_vehicleEnterAnim;
    /* 0088 */ int32_t m_vehicleExitAnim;
    /* 008C */ int32_t m_vehicleRideAnimLoop;
    /* 0090 */ int32_t m_field33512213034;
    /* 0094 */ int32_t m_vehicleExitAnimBone;
    /* 0098 */ int32_t m_vehicleEnterAnimBone;
    /* 009C */ float m_field33512213037;
    /* 00A0 */ float m_field33512213038;
    /* 00A4 */ int32_t m_vehicleAbilityDisplay;
    /* 00A8 */ int32_t m_enterUisoundID;
    /* 00AC */ int32_t m_field33512213041;
    /* 00B0 */ int32_t m_uiSkin;
    /* 00B4 */ float m_field33512213043;
    /* 00B8 */ float m_field33512213044;
    /* 00BC */ float m_field33512213045;
    /* 00C0 */ int32_t m_field33512213046;
    /* 00C4 */ float m_field33512213047;
    /* 00C8 */ float m_field33512213048;
    /* 00CC */ float m_field33512213049;
    /* 00D0 */ float m_field33512213050;
    /* 00D4 */ float m_field33512213051;
    /* 00D8 */ float m_field33512213052;
    /* 00DC */ float m_field33512213053;
    /* 00E0 */ float m_field33512213054;
    /* 00E4 */ float m_field33512213055;
};

struct VehicleUIIndSeatRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_vehicleUiindicatorID;
    /* 0008 */ int32_t m_virtualSeatIndex;
    /* 000C */ float m_xpos;
    /* 0010 */ float m_ypos;
};

struct VehicleUIIndicatorRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_backgroundTexture;
};

struct VideoHardwareRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_vendorID;
    /* 0008 */ int32_t m_deviceID;
    /* 000C */ int32_t m_farclipIdx;
    /* 0010 */ int32_t m_terrainLoddistIdx;
    /* 0014 */ int32_t m_terrainShadowLod;
    /* 0018 */ int32_t m_detailDoodadDensityIdx;
    /* 001C */ int32_t m_detailDoodadAlpha;
    /* 0020 */ int32_t m_animatingDoodadIdx;
    /* 0024 */ int32_t m_trilinear;
    /* 0028 */ int32_t m_numLights;
    /* 002C */ int32_t m_specularity;
    /* 0030 */ int32_t m_waterLodidx;
    /* 0034 */ int32_t m_particleDensityIdx;
    /* 0038 */ int32_t m_unitDrawDistIdx;
    /* 003C */ int32_t m_smallCullDistIdx;
    /* 0040 */ int32_t m_resolutionIdx;
    /* 0044 */ int32_t m_baseMipLevel;
    /* 0048 */ const char* m_oglOverrides;
    /* 004C */ const char* m_d3DOverrides;
    /* 0050 */ int32_t m_fixLag;
    /* 0054 */ int32_t m_multisample;
    /* 0058 */ int32_t m_atlasdisable;
};

struct VocalUISoundsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_vocalUienum;
    /* 0008 */ int32_t m_raceID;
    /* 000C */ int32_t m_normalSoundID [2];
    /* 0014 */ int32_t m_pissedSoundID [2];
};

struct WMOAreaTableRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_wmoid;
    /* 0008 */ int32_t m_nameSetID;
    /* 000C */ int32_t m_wmogroupID;
    /* 0010 */ int32_t m_soundProviderPref;
    /* 0014 */ int32_t m_soundProviderPrefUnderwater;
    /* 0018 */ int32_t m_ambienceID;
    /* 001C */ int32_t m_zoneMusic;
    /* 0020 */ int32_t m_introSound;
    /* 0024 */ int32_t m_flags;
    /* 0028 */ int32_t m_areaTableID;
    /* 002C */ const char* m_areaName;
};

struct WeaponImpactSoundsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_weaponSubClassID;
    /* 0008 */ int32_t m_parrySoundType;
    /* 000C */ int32_t m_impactSoundID [10];
    /* 0034 */ int32_t m_critImpactSoundID [10];
};

struct WeaponSwingSounds2Rec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_swingType;
    /* 0008 */ int32_t m_crit;
    /* 000C */ int32_t m_soundID;
};

struct WeatherRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_ambienceID;
    /* 0008 */ int32_t m_effectType;
    /* 000C */ float m_transitionSkyBox;
    /* 0010 */ float m_effectColor [3];
    /* 001C */ const char* m_effectTexture;
};

struct WorldChunkSoundsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_chunkX;
    /* 0008 */ int32_t m_chunkY;
    /* 000C */ int32_t m_subchunkX;
    /* 0010 */ int32_t m_subchunkY;
    /* 0014 */ int32_t m_zoneIntroMusicID;
    /* 0018 */ int32_t m_zoneMusicID;
    /* 001C */ int32_t m_soundAmbienceID;
    /* 0020 */ int32_t m_soundProviderPreferencesID;
};

struct WorldMapAreaRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_mapID;
    /* 0008 */ int32_t m_areaID;
    /* 000C */ const char* m_areaName;
    /* 0010 */ float m_locLeft;
    /* 0014 */ float m_locRight;
    /* 0018 */ float m_locTop;
    /* 001C */ float m_locBottom;
    /* 0020 */ int32_t m_displayMapID;
    /* 0024 */ int32_t m_defaultDungeonFloor;
    /* 0028 */ int32_t m_parentWorldMapID;
};

struct WorldMapContinentRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_mapID;
    /* 0008 */ int32_t m_leftBoundary;
    /* 000C */ int32_t m_rightBoundary;
    /* 0010 */ int32_t m_topBoundary;
    /* 0014 */ int32_t m_bottomBoundary;
    /* 0018 */ float m_continentOffset [2];
    /* 0020 */ float m_scale;
    /* 0024 */ float m_taxiMin [2];
    /* 002C */ float m_taxiMax [2];
    /* 0034 */ int32_t m_worldMapID;
};

struct WorldMapOverlayRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_mapAreaID;
    /* 0008 */ int32_t m_areaID [4];
    /* 0018 */ int32_t m_mapPointX;
    /* 001C */ int32_t m_mapPointY;
    /* 0020 */ const char* m_textureName;
    /* 0024 */ int32_t m_textureWidth;
    /* 0028 */ int32_t m_textureHeight;
    /* 002C */ int32_t m_offsetX;
    /* 0030 */ int32_t m_offsetY;
    /* 0034 */ int32_t m_hitRectTop;
    /* 0038 */ int32_t m_hitRectLeft;
    /* 003C */ int32_t m_hitRectBottom;
    /* 0040 */ int32_t m_hitRectRight;
};

struct WorldMapTransformsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_mapID;
    /* 0008 */ float m_regionMin [2];
    /* 0010 */ float m_regionMax [2];
    /* 0018 */ int32_t m_newMapID;
    /* 001C */ float m_regionOffset [2];
    /* 0024 */ int32_t m_newDungeonMapID;
};

struct WorldSafeLocsRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_continent;
    /* 0008 */ float m_loc [3];
    /* 0014 */ const char* m_areaName;
};

struct WorldStateUIRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_mapID;
    /* 0008 */ int32_t m_areaID;
    /* 000C */ int32_t m_phaseShift;
    /* 0010 */ const char* m_icon;
    /* 0014 */ const char* m_string;
    /* 0018 */ const char* m_tooltip;
    /* 001C */ int32_t m_stateVariable;
    /* 0020 */ int32_t m_type;
    /* 0024 */ const char* m_dynamicIcon;
    /* 0028 */ const char* m_dynamicTooltip;
    /* 002C */ const char* m_extendedUi;
    /* 0030 */ int32_t m_extendedUistateVariable [3];
};

struct WorldStateZoneSoundsRec {
    /* 0000 */ int32_t m_worldStateID;
    /* 0004 */ int32_t m_worldStateValue;
    /* 0008 */ int32_t m_areaID;
    /* 000C */ int32_t m_wmoareaID;
    /* 0010 */ int32_t m_zoneIntroMusicID;
    /* 0014 */ int32_t m_zoneMusicID;
    /* 0018 */ int32_t m_soundAmbienceID;
    /* 001C */ int32_t m_soundProviderPreferencesID;
    /* 0020 */ int32_t m_generatedID;
};

struct ZoneIntroMusicTableRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_soundID;
    /* 000C */ int32_t m_priority;
    /* 0010 */ int32_t m_minDelayMinutes;
};

struct ZoneLightPointRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_zoneLightID;
    /* 0008 */ float m_pos [2];
    /* 0010 */ int32_t m_pointOrder;
};

struct ZoneLightRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_mapID;
    /* 000C */ int32_t m_lightID;
};

struct ZoneMusicRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_setName;
    /* 0008 */ int32_t m_silenceIntervalMin [2];
    /* 0010 */ int32_t m_silenceIntervalMax [2];
    /* 0018 */ int32_t m_sounds [2];
};

struct gtBarberShopCostBaseRec {
    /* 0000 */ float m_data;
    /* 0004 */ int32_t m_generatedID;
};

struct gtChanceToMeleeCritBaseRec {
    /* 0000 */ float m_data;
    /* 0004 */ int32_t m_generatedID;
};

struct gtChanceToMeleeCritRec {
    /* 0000 */ float m_data;
    /* 0004 */ int32_t m_generatedID;
};

struct gtChanceToSpellCritBaseRec {
    /* 0000 */ float m_data;
    /* 0004 */ int32_t m_generatedID;
};

struct gtChanceToSpellCritRec {
    /* 0000 */ float m_data;
    /* 0004 */ int32_t m_generatedID;
};

struct gtCombatRatingsRec {
    /* 0000 */ float m_data;
    /* 0004 */ int32_t m_generatedID;
};

struct gtNPCManaCostScalerRec {
    /* 0000 */ float m_data;
    /* 0004 */ int32_t m_generatedID;
};

struct gtOCTClassCombatRatingScalarRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ float m_data;
};

struct gtOCTRegenHPRec {
    /* 0000 */ float m_data;
    /* 0004 */ int32_t m_generatedID;
};

struct gtOCTRegenMPRec {
    /* 0000 */ float m_data;
    /* 0004 */ int32_t m_generatedID;
};

struct gtRegenHPPerSptRec {
    /* 0000 */ float m_data;
    /* 0004 */ int32_t m_generatedID;
};

struct gtRegenMPPerSptRec {
    /* 0000 */ float m_data;
    /* 0004 */ int32_t m_generatedID;
};

struct gtSpellScalingRec {
    /* 0000 */ int32_t m_id;
    /* 0004 */ float m_data;
};

class ClientDb {
    public:
    /* 0000 */ uint8_t unk_0000[12];
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint8_t unk_0014[12];
    /* 0020 */ uint32_t unk_0020;
};

#endif
