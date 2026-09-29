#include "QuestCache.h"
#include "Engine/Source/Base/CDataStore.h"

// VERIFY: 43c9bc4a manual
// OFFSET: 0x00634E70
QuestCache::QuestCache() {
    this->m_rewardHonorBonus = 0.0f;
    this->m_id = 0;
    this->m_method = 0;
    this->m_level = 0;
    this->m_requiredLevel = 0;
    this->m_areaIdOrSortId = 0;
    this->m_infoId = 0;
    this->m_suggestedPlayers = 0;
    this->m_friendlyFactionId = 0;
    this->m_friendlyFactionAmount = 0;
    this->m_hostileFactionId = 0;
    this->m_hostileFactionAmount = 0;
    this->m_nextQuestId = 0;
    this->m_xPId = 0;
    this->m_rewardMoney = 0;
    this->m_rewardMoneyInsteadOfXp = 0;
    this->m_rewardSpellId = 0;
    this->m_effectOnPlayer = 0;
    this->m_rewardHonor = 0;
    this->m_startingItemId = 0;
    this->m_flags = 0;
    this->m_rewardTitleId = 0;
    this->m_requiredPlayersKilled = 0;
    this->m_rewardTalentPoints = 0;
    this->m_rewardArenaPoints = 0;

    for (uint32_t i = 0; i < 5; i++) {
        this->m_rewardReputationFaction[i] = 0;
    }

    for (uint32_t i = 0; i < 5; i++) {
        this->m_factionRewardId[i] = 0;
    }

    for (uint32_t i = 0; i < 5; i++) {
        this->m_rewardReputationOverride[i] = 0;
    }

    this->unk_28E0 = 0;

    for (uint32_t i = 0; i < 4; i++) {
        this->m_rewardItem[i] = 0;
    }

    for (uint32_t i = 0; i < 4; i++) {
        this->m_rewardItemCount[i] = 1;
    }

    for (uint32_t i = 0; i < 6; i++) {
        this->m_rewardChoiceItem[i] = 0;
        this->m_rewardChoiceItemCount[i] = 1;
    }

    this->m_pointX = 0.0f;
    this->m_pointY = 0.0f;
    this->m_pointMapId = 0;
    this->m_pointOptional = 0;
    this->m_name[0] = '\0';
    this->m_objText[0] = '\0';
    this->m_description[0] = '\0';
    this->m_endText[0] = '\0';
    this->m_completionText[0] = '\0';

    for (uint32_t i = 0; i < 4; i++) {
        this->m_objectiveId[i] = 0;
        this->m_objectiveRequiredCount[i] = 0;
        this->m_objectiveText[i][0] = '\0';
    }

    for (uint32_t i = 0; i < 4; i++) {
        this->m_intermediateItemId[i] = 0;
        this->m_intermediateItemCount[i] = 0;
    }

    for (uint32_t i = 0; i < 6; i++) {
        this->m_collectItemId[i] = 0;
        this->m_collectItemCount[i] = 0;
    }
}

// VERIFY: 580631db manual
// OFFSET: 0x007F6D60
void QuestCache::Pack(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(this->m_id));
    msg->Put(this->m_method);
    msg->Put(this->m_level);
    msg->Put(this->m_requiredLevel);
    msg->Put(this->m_areaIdOrSortId);
    msg->Put(this->m_infoId);
    msg->Put(this->m_suggestedPlayers);
    msg->Put(this->m_friendlyFactionId);
    msg->Put(this->m_friendlyFactionAmount);
    msg->Put(this->m_hostileFactionId);
    msg->Put(this->m_hostileFactionAmount);
    msg->Put(this->m_nextQuestId);
    msg->Put(this->m_xPId);
    msg->Put(static_cast<uint32_t>(this->m_rewardMoney));
    msg->Put(this->m_rewardMoneyInsteadOfXp);
    msg->Put(this->m_rewardSpellId);
    msg->Put(this->m_effectOnPlayer);
    msg->Put(this->m_rewardHonor);
    msg->Put(this->m_rewardHonorBonus);
    msg->Put(this->m_startingItemId);
    msg->Put(this->m_flags);
    msg->Put(this->m_rewardTitleId);
    msg->Put(this->m_requiredPlayersKilled);
    msg->Put(this->m_rewardTalentPoints);
    msg->Put(this->m_rewardArenaPoints);
    msg->Put(this->unk_28E0);

    uint32_t count = 0;

    for (uint32_t i = 0; i < 4; i++) {
        if (this->m_rewardItem[i]) {
            msg->Put(this->m_rewardItem[i]);
            msg->Put(this->m_rewardItemCount[i]);
            count++;
        }
    }

    for (; count < 4; count++) {
        msg->Put(static_cast<uint32_t>(0));
        msg->Put(static_cast<uint32_t>(0));
    }

    count = 0;

    for (uint32_t i = 0; i < 6; i++) {
        if (this->m_rewardChoiceItem[i]) {
            msg->Put(this->m_rewardChoiceItem[i]);
            msg->Put(this->m_rewardChoiceItemCount[i]);
            count++;
        }
    }

    for (; count < 6; count++) {
        msg->Put(static_cast<uint32_t>(0));
        msg->Put(static_cast<uint32_t>(0));
    }

    for (uint32_t i = 0; i < 5; i++) {
        msg->Put(this->m_rewardReputationFaction[i]);
    }

    for (uint32_t i = 0; i < 5; i++) {
        msg->Put(this->m_factionRewardId[i]);
    }

    for (uint32_t i = 0; i < 5; i++) {
        msg->Put(static_cast<uint32_t>(this->m_rewardReputationOverride[i]));
    }

    msg->Put(this->m_pointMapId);
    msg->Put(this->m_pointX);
    msg->Put(this->m_pointY);
    msg->Put(this->m_pointOptional);
    msg->PutString(this->m_name);
    msg->PutString(this->m_objText);
    msg->PutString(this->m_description);
    msg->PutString(this->m_endText);
    msg->PutString(this->m_completionText);

    for (uint32_t i = 0; i < 4; i++) {
        msg->Put(this->m_objectiveId[i]);
        msg->Put(this->m_objectiveRequiredCount[i]);
        msg->Put(this->m_intermediateItemId[i]);
        msg->Put(this->m_intermediateItemCount[i]);
    }

    for (uint32_t i = 0; i < 6; i++) {
        msg->Put(this->m_collectItemId[i]);
        msg->Put(this->m_collectItemCount[i]);
    }

    for (uint32_t i = 0; i < 4; i++) {
        msg->PutString(this->m_objectiveText[i]);
    }
}

// VERIFY: b3658ed9 auto
// OFFSET: 0x007F70E0
void QuestCache::Unpack(CDataStore* msg) {
    msg->Get(reinterpret_cast<uint32_t&>(this->m_id));
    msg->Get(this->m_method);
    msg->Get(this->m_level);
    msg->Get(this->m_requiredLevel);
    msg->Get(this->m_areaIdOrSortId);
    msg->Get(this->m_infoId);
    msg->Get(this->m_suggestedPlayers);
    msg->Get(this->m_friendlyFactionId);
    msg->Get(this->m_friendlyFactionAmount);
    msg->Get(this->m_hostileFactionId);
    msg->Get(this->m_hostileFactionAmount);
    msg->Get(this->m_nextQuestId);
    msg->Get(this->m_xPId);
    msg->Get(reinterpret_cast<uint32_t&>(this->m_rewardMoney));
    msg->Get(this->m_rewardMoneyInsteadOfXp);
    msg->Get(this->m_rewardSpellId);
    msg->Get(this->m_effectOnPlayer);
    msg->Get(this->m_rewardHonor);
    msg->Get(this->m_rewardHonorBonus);
    msg->Get(this->m_startingItemId);
    msg->Get(this->m_flags);
    msg->Get(this->m_rewardTitleId);
    msg->Get(this->m_requiredPlayersKilled);
    msg->Get(this->m_rewardTalentPoints);
    msg->Get(this->m_rewardArenaPoints);
    msg->Get(this->unk_28E0);

    for (uint32_t i = 0; i < 4; i++) {
        msg->Get(this->m_rewardItem[i]);
        msg->Get(this->m_rewardItemCount[i]);
    }

    for (uint32_t i = 0; i < 6; i++) {
        msg->Get(this->m_rewardChoiceItem[i]);
        msg->Get(this->m_rewardChoiceItemCount[i]);
    }

    for (uint32_t i = 0; i < 5; i++) {
        msg->Get(this->m_rewardReputationFaction[i]);
    }

    for (uint32_t i = 0; i < 5; i++) {
        msg->Get(this->m_factionRewardId[i]);
    }

    for (uint32_t i = 0; i < 5; i++) {
        msg->Get(reinterpret_cast<uint32_t&>(this->m_rewardReputationOverride[i]));
    }

    msg->Get(this->m_pointMapId);
    msg->Get(this->m_pointX);
    msg->Get(this->m_pointY);
    msg->Get(this->m_pointOptional);
    msg->GetString(this->m_name, sizeof(this->m_name));
    msg->GetString(this->m_objText, sizeof(this->m_objText));
    msg->GetString(this->m_description, sizeof(this->m_description));
    msg->GetString(this->m_endText, sizeof(this->m_endText));
    msg->GetString(this->m_completionText, 0x7FFFFFFF);

    for (uint32_t i = 0; i < 4; i++) {
        msg->Get(this->m_objectiveId[i]);
        msg->Get(this->m_objectiveRequiredCount[i]);
        msg->Get(this->m_intermediateItemId[i]);
        msg->Get(this->m_intermediateItemCount[i]);
    }

    for (uint32_t i = 0; i < 6; i++) {
        msg->Get(this->m_collectItemId[i]);
        msg->Get(this->m_collectItemCount[i]);
    }

    for (uint32_t i = 0; i < 4; i++) {
        msg->GetString(this->m_objectiveText[i], 0x7FFFFFFF);
    }
}
