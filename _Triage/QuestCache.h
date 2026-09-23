#ifndef _TRIAGE_QUESTCACHE_H
#define _TRIAGE_QUESTCACHE_H

#include <cstddef>
#include <cstdint>

class QuestCache {
    public:
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_method;
    /* 0008 */ int32_t m_level;
    /* 000C */ int32_t m_requiredLevel;
    /* 0010 */ int32_t m_areaIdOrSortId;
    /* 0014 */ int32_t m_infoId;
    /* 0018 */ int32_t m_suggestedPlayers;
    /* 001C */ int32_t m_friendlyFactionID;
    /* 0020 */ int32_t m_friendlyFactionAmount;
    /* 0024 */ int32_t m_hostileFactionID;
    /* 0028 */ int32_t m_hostileFactionAmount;
    /* 002C */ int32_t m_nextQuestId;
    /* 0030 */ int32_t m_xPId;
    /* 0034 */ int32_t m_rewardMoney;
    /* 0038 */ int32_t m_rewardMoneyInsteadOfXp;
    /* 003C */ int32_t m_rewardSpellId;
    /* 0040 */ int32_t m_effectOnPlayer;
    /* 0044 */ int32_t m_rewardHonor;
    /* 0048 */ float m_rewardHonorBonus;
    /* 004C */ int32_t m_startingItemId;
    /* 0050 */ int32_t m_flags;
    /* 0054 */ int32_t m_rewardItem [4];
    /* 0064 */ int32_t m_rewardItemCount [4];
    /* 0074 */ int32_t m_rewardChoiceItem [6];
    /* 008C */ int32_t m_rewardChoiceItemCount [6];
    /* 00A4 */ int32_t m_pointMapID;
    /* 00A8 */ float m_pointX;
    /* 00AC */ float m_pointY;
    /* 00B0 */ int32_t m_pointOptional;
    /* 00B4 */ char m_name [512];
    /* 02B4 */ char m_objText [3000];
    /* 0E6C */ char m_description [3000];
    /* 1A24 */ char m_endText [512];
    /* 1C24 */ int32_t m_objectiveId [4];
    /* 1C34 */ int32_t m_objectiveRequiredCount [4];
    /* 1C44 */ int32_t m_collectItemId [6];
    /* 1C5C */ int32_t m_collectItemCount [6];
    /* 1C74 */ int32_t m_intermediateItemId [4];
    /* 1C84 */ int32_t m_intermediateItemCount [4];
    /* 1C94 */ char m_objectiveText [4][256];
    /* 2094 */ int32_t m_rewardTitleId;
    /* 2098 */ int32_t m_requiredPlayersKilled;
    /* 209C */ int32_t m_rewardTalentPoints;
    /* 20A0 */ int32_t m_rewardArenaPoints;
    /* 20A4 */ char m_completionText [2048];
    /* 28A4 */ int32_t m_rewardReputationFaction [5];
    /* 28B8 */ int32_t m_factionRewardID [5];
    /* 28CC */ int32_t m_rewardReputationOverride [5];
    /* 28E0 */ int32_t unk_28E0;
};

#endif
