// Donor schema declarations, unchanged spell graph subset. GPL-3.0-or-later.
#pragma once
#include "vendor/extensions/wxl-db2/src/api/Db2.hpp"
#include <array>
namespace wxl::runtime::db2::retail {
    inline constexpr std::array kSpellXSpellVisualFields{
        // This table keeps its row ID inline (unlike the other visual graph
        // tables below), so ID is part of the physical 12-field WDC5 layout.
        Field{"ID"}, Field{"DifficultyID"}, Field{"SpellVisualID"},
        Field{"Probability"}, Field{"Flags2"}, Field{"Priority"},
        Field{"SpellIconFileID"}, Field{"ActiveIconFileID"},
        Field{"ViewerUnitConditionID"}, Field{"ViewerPlayerConditionID"},
        Field{"CasterUnitConditionID"}, Field{"CasterPlayerConditionID"},
    };
    inline constexpr Definition SpellXSpellVisual{
        "SpellXSpellVisual", "spellxspellvisual.db2", 0x7994A890u,
        kSpellXSpellVisualFields,
    };

    inline constexpr std::array kSpellVisualFields{
        Field{"MissileCastOffset", 3}, Field{"MissileImpactOffset", 3},
        Field{"StateKit"}, Field{"AnimEventSoundID"}, Field{"Flags"},
        Field{"MissileAttachment"}, Field{"MissileDestinationAttachment"},
        Field{"MissileCastPositionerID"}, Field{"MissileImpactPositionerID"},
        Field{"MissileTargetingKit"}, Field{"HostileSpellVisualID"},
        Field{"CasterSpellVisualID"}, Field{"SpellVisualMissileSetID"},
        Field{"DamageNumberDelay"}, Field{"LowViolenceSpellVisualID"},
        Field{"RaidSpellVisualMissileSetID"},
        Field{"ReducedUnexpectedCameraMovementSpellVisualID"},
    };
    inline constexpr Definition SpellVisual{
        "SpellVisual", "spellvisual.db2", 0x4B85C90Fu,
        kSpellVisualFields,
    };

    inline constexpr std::array kSpellVisualMissileFields{
        Field{"CastOffset", 3}, Field{"ImpactOffset", 3}, Field{"ID"},
        Field{"SpellVisualEffectNameID"}, Field{"SoundEntriesID"},
        Field{"Attachment"}, Field{"DestinationAttachment"},
        Field{"CastPositionerID"}, Field{"ImpactPositionerID"},
        Field{"FollowGroundHeight"}, Field{"FollowGroundDropSpeed"},
        Field{"FollowGroundApproach"}, Field{"Flags"},
        Field{"SpellMissileMotionID"}, Field{"AnimKitID"},
        Field{"ClutterLevel"}, Field{"DecayTimeAfterImpact"},
        Field{"Field_11_0_0_54210_017"},
    };
    inline constexpr std::array kSpellVisualMissileRelations{
        Relation{"missileSet", RelationSource::ParentId, {}, 0,
                 "SpellVisual", "SpellVisualMissileSetID"},
    };
    inline constexpr Definition SpellVisualMissile{
        "SpellVisualMissile", "spellvisualmissile.db2", 0xAE389078u,
        kSpellVisualMissileFields, kSpellVisualMissileRelations,
    };

    inline constexpr std::array kSpellVisualEventFields{
        Field{"StartEvent"}, Field{"EndEvent"}, Field{"StartMinOffsetMs"},
        Field{"StartMaxOffsetMs"}, Field{"EndMinOffsetMs"},
        Field{"EndMaxOffsetMs"}, Field{"TargetType"},
        Field{"SpellVisualKitID"}, Field{"Field_10_0_0_44649_008"},
        Field{"Field_10_0_0_44649_009"},
    };
    inline constexpr Definition SpellVisualEvent{
        "SpellVisualEvent", "spellvisualevent.db2", 0x865F512Eu,
        kSpellVisualEventFields,
    };

    inline constexpr std::array kSpellVisualKitEffectFields{
        Field{"EffectType"}, Field{"Effect"},
    };
    inline constexpr Definition SpellVisualKitEffect{
        "SpellVisualKitEffect", "spellvisualkiteffect.db2", 0xE3206CA2u,
        kSpellVisualKitEffectFields,
    };

    inline constexpr std::array kSpellVisualKitModelAttachFields{
        Field{"Offset", 3}, Field{"OffsetVariation", 3},
        Field{"SpellVisualEffectNameID"}, Field{"AttachmentID"},
        Field{"PositionerID"}, Field{"Yaw"}, Field{"Pitch"},
        Field{"Roll"}, Field{"YawVariation"}, Field{"PitchVariation"},
        Field{"RollVariation"}, Field{"Scale"}, Field{"ScaleVariation"},
        Field{"StartAnimID"}, Field{"AnimID"}, Field{"EndAnimID"},
        Field{"AnimKitID"}, Field{"Flags"}, Field{"LowDefModelAttachID"},
        Field{"StartDelay"}, Field{"Field_9_0_1_33978_021"},
        Field{"Field_11_0_0_54210_022"},
    };
    inline constexpr Definition SpellVisualKitModelAttach{
        "SpellVisualKitModelAttach", "spellvisualkitmodelattach.db2",
        0x02CF8554u, kSpellVisualKitModelAttachFields,
    };

    inline constexpr std::array kSpellVisualEffectNameFields{
        Field{"ModelFileDataID"}, Field{"BaseMissileSpeed"},
        Field{"Scale"}, Field{"MinAllowedScale"},
        Field{"MaxAllowedScale"}, Field{"Alpha"}, Field{"Flags"},
        Field{"TextureFileDataID"}, Field{"EffectRadius"}, Field{"Type"},
        Field{"GenericID"}, Field{"RibbonQualityID"},
        Field{"DissolveEffectID"}, Field{"ModelPosition"},
        Field{"Field_9_1_0_38549_014"}, Field{"Field_11_0_0_54210_015"},
    };
    inline constexpr Definition SpellVisualEffectName{
        "SpellVisualEffectName", "spellvisualeffectname.db2", 0x2245CEE6u,
        kSpellVisualEffectNameFields,
    };

}
