#include "GameUI.h"
#include <cmath>
#include <cstdarg>
#include <cstring>
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Streaming.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Event/CMouseEvent.h"
#include "Engine/Source/Event/CSizeEvent.h"
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Frame/CSimpleTop.h"
#include "Engine/Source/Frame/SimpleFrameScript.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/FrameXML/FrameXML.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Os/OsFile.h"
#include "Engine/Source/Os/Time.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/ShaderEffect/ShadowCache.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/Source/MD5.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Tempest/CRect.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/Game/GameClient/ComSatClient.h"
#include "WoW/Source/Glue/CGlueMgr.h"
#include "WoW/Source/LoadingScreen.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/SimpleScript.h"
#include "WoW/Source/Object/ObjectClient/UnitCombatLog_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/ObjectClient/VehiclePassenger_C.h"
#include "WoW/Source/Object/Types.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/SoundInterface/SoundInterface2.h"
#include "WoW/Source/UIUtil/Camera.h"
#include "WoW/Source/UIUtil/Cursor.h"
#include "WoW/Source/UIUtil/InputControl.h"
#include "WoW/Source/UIUtil/PortraitButton.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/Ui/AchievementInfo.h"
#include "WoW/Source/Game/GameClient/PlayerName.h"
#include "WoW/Source/Object/ObjectClient/Bag_C.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Ui/GuildBankFrame.h"
#include "WoW/Source/Ui/LootFrame.h"
#include "WoW/Source/Ui/PetInfo.h"
#include "WoW/Source/Ui/SpellBookFrame.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "WoW/Source/Object/ObjectClient/Vehicle_C.h"
#include "WoW/Source/Ui/InstanceEncounter_C.h"
#include "WoW/Source/Ui/TradeFrame.h"
#include "WoW/Source/Ui/Tutorial.h"
#include "WoW/Source/Ui/UIBindings.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "WoW/Source/Ui/ActionBarFrame.h"
#include "WoW/Source/Ui/AddOns.h"
#include "WoW/Source/Ui/ArenaTeamInfo.h"
#include "WoW/Source/Ui/AuctionHouse.h"
#include "WoW/Source/Ui/BarberShop.h"
#include "WoW/Source/Ui/BattlefieldInfo.h"
#include "WoW/Source/Ui/BattlenetUI.h"
#include "WoW/Source/Ui/CGAutoCompleteName.h"
#include "WoW/Source/Ui/CGBankInfo.h"
#include "WoW/Source/Ui/CGCooldown.h"
#include "WoW/Source/Ui/CGMinigameInfo.h"
#include "WoW/Source/Ui/CGQuestPOIFrame.h"
#include "WoW/Source/Ui/CGTabardCreationFrame.h"
#include "WoW/Source/Ui/CGWorldStateInfo.h"
#include "WoW/Source/Ui/Calendar.h"
#include "WoW/Source/Ui/CharacterModelBase.h"
#include "WoW/Source/Ui/ChatBubbleFrame.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/ClassTrainerFrame.h"
#include "WoW/Source/Ui/CommentatorFrame.h"
#include "WoW/Source/Ui/ContainerFrame.h"
#include "WoW/Source/Ui/CurrencyTypes.h"
#include "WoW/Source/Ui/DanceStudio.h"
#include "WoW/Source/Ui/DressUpModelFrame.h"
#include "WoW/Source/Ui/DuelInfo.h"
#include "WoW/Source/Ui/EquipmentManager.h"
#include "WoW/Source/Ui/GMSurveyInfo.h"
#include "WoW/Source/Ui/GMTicketInfo.h"
#include "WoW/Source/Ui/GlyphInfo.h"
#include "WoW/Source/Ui/GossipInfo.h"
#include "WoW/Source/Ui/GuildBankFrame.h"
#include "WoW/Source/Ui/GuildInfo.h"
#include "WoW/Source/Ui/GuildRegistrar.h"
#include "WoW/Source/Ui/ItemSocketInfo.h"
#include "WoW/Source/Ui/ItemTextFrame.h"
#include "WoW/Source/Ui/KnowledgeBase.h"
#include "WoW/Source/Ui/LFGInfo.h"
#include "WoW/Source/Ui/LootFrame.h"
#include "WoW/Source/Ui/MailInfo.h"
#include "WoW/Source/Ui/MerchantFrame.h"
#include "WoW/Source/Ui/MinimapFrame.h"
#include "WoW/Source/Ui/PaperDollInfoFrame.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/Ui/PetInfo.h"
#include "WoW/Source/Ui/PetitionInfo.h"
#include "WoW/Source/Ui/PetitionVendor.h"
#include "WoW/Source/Ui/QuestFrame.h"
#include "WoW/Source/Ui/QuestLog.h"
#include "WoW/Source/Ui/RaidInfo.h"
#include "WoW/Source/Ui/ReputationInfo.h"
#include "WoW/Source/Ui/RuneInfo.h"
#include "WoW/Source/Ui/Screen.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/SkillInfo.h"
#include "WoW/Source/Ui/SpellBookFrame.h"
#include "WoW/Source/Ui/StableInfo.h"
#include "WoW/Source/Ui/TabardModelFrame.h"
#include "WoW/Source/Ui/TalentInfo.h"
#include "WoW/Source/Ui/TaxiMapFrame.h"
#include "WoW/Source/Ui/TradeFrame.h"
#include "WoW/Source/Ui/TradeSkillFrame.h"
#include "WoW/Source/Ui/Tutorial.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/Ui/UIBindings.h"
#include "WoW/Source/Ui/UILCD/LCD.h"
#include "WoW/Source/Ui/UIMacros.h"
#include "WoW/Source/Ui/VideoOptions.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "WoW/Source/Ui/WorldMap.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/AccountData.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/FriendList.h"
#include "_Triage/CWOWClientStatus.h"
#include "_Triage/GxRestart.h"

// STATIC: 0x00BD0CC4
static CWOWClientStatus* s_loadAddOnStatus;

// STATIC: 0x00BD0CC8
static int32_t s_loadAddOnDepth;

// STATIC: 0x00AC804C
const char* g_macroTaint = "";

// STATIC: 0x00AC8088
static const char* s_readOnlyCVars[6] = { "realmList", "realmName", "decorateAccountName", "realmlistbn", "portal", "serverAlert" };

// STATIC: 0x00AC8AF0
static UIErrorInfo s_errorInfo[ERR_MAX_VALUE] = {
    { "ERR_INV_FULL", 2, "NONE", 0, 0 },
    { "ERR_BANK_FULL", 2, "NONE", 68, 0 },
    { "ERR_CANT_EQUIP_LEVEL_I", 2, "NONE", 2, 0 },
    { "ERR_CANT_EQUIP_SKILL", 2, "NONE", 41, 0 },
    { "ERR_CANT_EQUIP_EVER", 2, "NONE", 3, 0 },
    { "ERR_CANT_EQUIP_RANK", 2, "NONE", 68, 0 },
    { "ERR_CANT_EQUIP_RATING", 2, "NONE", 68, 0 },
    { "ERR_CANT_EQUIP_REPUTATION", 2, "NONE", 68, 0 },
    { "ERR_PROFICIENCY_NEEDED", 2, "NONE", 48, 0 },
    { "ERR_WRONG_SLOT", 2, "NONE", 27, 0 },
    { "ERR_CANT_EQUIP_NEED_TALENT", 2, "NONE", 41, 0 },
    { "ERR_BAG_FULL", 2, "NONE", 29, 0 },
    { "ERR_INTERNAL_BAG_ERROR", 2, "NONE", 68, 0 },
    { "ERR_DESTROY_NONEMPTY_BAG", 2, "NONE", 68, 0 },
    { "ERR_BAG_IN_BAG", 2, "NONE", 26, 0 },
    { "ERR_TOO_MANY_SPECIAL_BAGS", 2, "NONE", 68, 0 },
    { "ERR_TRADE_EQUIPPED_BAG", 2, "NONE", 68, 0 },
    { "ERR_AMMO_ONLY", 2, "NONE", 28, 0 },
    { "ERR_NO_SLOT_AVAILABLE", 2, "NONE", 68, 0 },
    { "ERR_WRONG_BAG_TYPE", 2, "NONE", 68, 0 },
    { "ERR_ITEM_MAX_COUNT", 2, "NONE", 30, 0 },
    { "ERR_NOT_EQUIPPABLE", 2, "NONE", 44, 0 },
    { "ERR_CANT_STACK", 2, "NONE", 68, 0 },
    { "ERR_CANT_SWAP", 2, "NONE", 68, 0 },
    { "ERR_SLOT_EMPTY", 2, "NONE", 68, 0 },
    { "ERR_ITEM_NOT_FOUND", 2, "NONE", 68, 0 },
    { "ERR_TOO_FEW_TO_SPLIT", 2, "NONE", 68, 0 },
    { "ERR_SPLIT_FAILED", 2, "NONE", 68, 0 },
    { "ERR_NOT_A_BAG", 2, "NONE", 25, 0 },
    { "ERR_NOT_OWNER", 2, "NONE", 68, 0 },
    { "ERR_ONLY_ONE_QUIVER", 2, "NONE", 68, 0 },
    { "ERR_NO_BANK_SLOT", 2, "NONE", 68, 0 },
    { "ERR_NO_BANK_HERE", 2, "NONE", 68, 0 },
    { "ERR_ITEM_LOCKED", 2, "NONE", 61, 0 },
    { "ERR_2HANDED_EQUIPPED", 2, "NONE", 42, 0 },
    { "ERR_VENDOR_NOT_INTERESTED", 2, "NONE", 68, 0 },
    { "ERR_VENDOR_HATES_YOU", 2, "NONE", 68, 0 },
    { "ERR_VENDOR_SOLD_OUT", 2, "NONE", 68, 0 },
    { "ERR_VENDOR_TOO_FAR", 2, "NONE", 68, 0 },
    { "ERR_VENDOR_DOESNT_BUY", 2, "NONE", 68, 0 },
    { "ERR_NOT_ENOUGH_MONEY", 2, "NONE", 40, 0 },
    { "ERR_RECEIVE_ITEM_S", 0, "ITEMGENERICSOUND", 68, 0 },
    { "ERR_DROP_BOUND_ITEM", 2, "NONE", 4, 0 },
    { "ERR_TRADE_BOUND_ITEM", 2, "NONE", 59, 0 },
    { "ERR_TRADE_QUEST_ITEM", 2, "NONE", 59, 0 },
    { "ERR_TRADE_TEMP_ENCHANT_BOUND", 2, "NONE", 59, 0 },
    { "ERR_TRADE_GROUND_ITEM", 2, "NONE", 68, 0 },
    { "ERR_TRADE_BAG", 2, "NONE", 68, 0 },
    { "ERR_SPELL_FAILED_S", 2, "NONE", 68, 0 },
    { "ERR_ITEM_COOLDOWN", 2, "NONE", 5, 0 },
    { "ERR_POTION_COOLDOWN", 2, "NONE", 47, 0 },
    { "ERR_FOOD_COOLDOWN", 2, "NONE", 7, 0 },
    { "ERR_SPELL_COOLDOWN", 2, "NONE", 12, 0 },
    { "ERR_ABILITY_COOLDOWN", 2, "NONE", 50, 0 },
    { "ERR_SPELL_ALREADY_KNOWN_S", 2, "NONE", 13, 0 },
    { "ERR_PET_SPELL_ALREADY_KNOWN_S", 2, "NONE", 68, 0 },
    { "ERR_PROFICIENCY_GAINED_S", 0, "NONE", 68, 26 },
    { "ERR_SKILL_GAINED_S", 0, "NONE", 68, 26 },
    { "ERR_SKILL_UP_SI", 0, "NONE", 68, 26 },
    { "ERR_LEARN_SPELL_S", 0, "NONE", 68, 0 },
    { "ERR_LEARN_ABILITY_S", 0, "NONE", 68, 0 },
    { "ERR_LEARN_RECIPE_S", 0, "NONE", 68, 0 },
    { "ERR_LEARN_COMPANION_S", 0, "NONE", 68, 0 },
    { "ERR_INVITE_PLAYER_S", 0, "NONE", 68, 0 },
    { "ERR_INVITE_SELF", 2, "NONE", 68, 0 },
    { "ERR_INVITED_TO_GROUP_SS", 0, "NONE", 68, 0 },
    { "ERR_INVITED_ALREADY_IN_GROUP_SS", 0, "NONE", 20, 0 },
    { "ERR_ALREADY_IN_GROUP_S", 0, "NONE", 20, 0 },
    { "ERR_PLAYER_BUSY_S", 0, "NONE", 68, 0 },
    { "ERR_NEW_LEADER_S", 0, "NONE", 68, 0 },
    { "ERR_NEW_LEADER_YOU", 0, "NONE", 68, 0 },
    { "ERR_NEW_GUIDE_S", 0, "NONE", 68, 0 },
    { "ERR_NEW_GUIDE_YOU", 0, "NONE", 68, 0 },
    { "ERR_LEFT_GROUP_S", 0, "NONE", 68, 0 },
    { "ERR_LEFT_GROUP_YOU", 0, "NONE", 68, 0 },
    { "ERR_GROUP_DISBANDED", 0, "NONE", 68, 0 },
    { "ERR_DECLINE_GROUP_S", 0, "igPlayerInviteDecline", 68, 0 },
    { "ERR_JOINED_GROUP_S", 0, "NONE", 68, 0 },
    { "ERR_UNINVITE_YOU", 0, "NONE", 68, 0 },
    { "ERR_BAD_PLAYER_NAME_S", 0, "NONE", 68, 0 },
    { "ERR_NOT_IN_GROUP", 0, "NONE", 68, 0 },
    { "ERR_TARGET_NOT_IN_GROUP_S", 0, "NONE", 68, 0 },
    { "ERR_TARGET_NOT_IN_INSTANCE_S", 0, "NONE", 68, 0 },
    { "ERR_GROUP_FULL", 0, "NONE", 8, 0 },
    { "ERR_NOT_LEADER", 0, "NONE", 68, 0 },
    { "ERR_PLAYER_DIED_S", 0, "NONE", 68, 0 },
    { "ERR_GUILD_CREATE_S", 0, "LEVELUP", 68, 0 },
    { "ERR_GUILD_INVITE_S", 0, "NONE", 68, 0 },
    { "ERR_INVITED_TO_GUILD_SSS", 0, "LEVELUP", 68, 0 },
    { "ERR_ALREADY_IN_GUILD_S", 0, "NONE", 68, 0 },
    { "ERR_ALREADY_INVITED_TO_GUILD_S", 0, "NONE", 68, 0 },
    { "ERR_INVITED_TO_GUILD", 0, "NONE", 68, 0 },
    { "ERR_ALREADY_IN_GUILD", 0, "NONE", 68, 0 },
    { "ERR_GUILD_ACCEPT", 0, "NONE", 68, 0 },
    { "ERR_GUILD_DECLINE_S", 0, "NONE", 68, 0 },
    { "ERR_GUILD_PERMISSIONS", 0, "NONE", 62, 0 },
    { "ERR_GUILD_JOIN_S", 0, "NONE", 68, 0 },
    { "ERR_GUILD_FOUNDER_S", 0, "NONE", 68, 0 },
    { "ERR_GUILD_PROMOTE_SSS", 0, "NONE", 68, 0 },
    { "ERR_GUILD_DEMOTE_SSS", 0, "NONE", 68, 0 },
    { "ERR_GUILD_QUIT_S", 0, "NONE", 68, 0 },
    { "ERR_GUILD_LEAVE_S", 0, "NONE", 68, 0 },
    { "ERR_GUILD_REMOVE_SS", 0, "NONE", 68, 0 },
    { "ERR_GUILD_REMOVE_SELF", 0, "NONE", 68, 0 },
    { "ERR_GUILD_DISBAND_S", 0, "NONE", 68, 0 },
    { "ERR_GUILD_DISBAND_SELF", 0, "NONE", 68, 0 },
    { "ERR_GUILD_LEADER_S", 0, "NONE", 68, 0 },
    { "ERR_GUILD_LEADER_SELF", 0, "NONE", 68, 0 },
    { "ERR_GUILD_PLAYER_NOT_FOUND_S", 0, "NONE", 68, 0 },
    { "ERR_GUILD_PLAYER_NOT_IN_GUILD_S", 0, "NONE", 68, 0 },
    { "ERR_GUILD_PLAYER_NOT_IN_GUILD", 0, "NONE", 68, 0 },
    { "ERR_GUILD_CANT_PROMOTE_S", 0, "NONE", 68, 0 },
    { "ERR_GUILD_CANT_DEMOTE_S", 0, "NONE", 68, 0 },
    { "ERR_GUILD_NOT_IN_A_GUILD", 0, "NONE", 68, 0 },
    { "ERR_GUILD_INTERNAL", 0, "NONE", 68, 0 },
    { "ERR_GUILD_LEADER_IS_S", 0, "NONE", 68, 0 },
    { "ERR_GUILD_LEADER_CHANGED_SS", 0, "NONE", 68, 0 },
    { "ERR_GUILD_DISBANDED", 0, "NONE", 68, 0 },
    { "ERR_GUILD_NOT_ALLIED", 0, "NONE", 68, 0 },
    { "ERR_GUILD_LEADER_LEAVE", 0, "NONE", 68, 0 },
    { "ERR_GUILD_RANKS_LOCKED", 0, "NONE", 68, 0 },
    { "ERR_GUILD_RANK_IN_USE", 0, "NONE", 68, 0 },
    { "ERR_GUILD_RANK_TOO_HIGH_S", 0, "NONE", 68, 0 },
    { "ERR_GUILD_RANK_TOO_LOW_S", 0, "NONE", 68, 0 },
    { "ERR_GUILD_NAME_EXISTS_S", 2, "NONE", 68, 0 },
    { "ERR_GUILD_WITHDRAW_LIMIT", 2, "NONE", 68, 0 },
    { "ERR_GUILD_NOT_ENOUGH_MONEY", 2, "NONE", 68, 0 },
    { "ERR_GUILD_BANK_CONJURED_ITEM", 2, "NONE", 68, 0 },
    { "ERR_GUILD_BANK_EQUIPPED_ITEM", 2, "NONE", 68, 0 },
    { "ERR_GUILD_BANK_BOUND_ITEM", 2, "NONE", 68, 0 },
    { "ERR_GUILD_BANK_QUEST_ITEM", 2, "NONE", 68, 0 },
    { "ERR_GUILD_BANK_WRAPPED_ITEM", 2, "NONE", 68, 0 },
    { "ERR_GUILD_BANK_FULL", 2, "NONE", 68, 0 },
    { "ERR_NO_GUILD_CHARTER", 2, "NONE", 68, 0 },
    { "ERR_OUT_OF_RANGE", 2, "NONE", 10, 0 },
    { "ERR_PLAYER_DEAD", 2, "NONE", 68, 0 },
    { "ERR_CLIENT_LOCKED_OUT", 2, "NONE", 68, 0 },
    { "ERR_KILLED_BY_S", 0, "NONE", 68, 0 },
    { "ERR_LOOT_LOCKED", 2, "NONE", 33, 0 },
    { "ERR_LOOT_TOO_FAR", 2, "NONE", 35, 0 },
    { "ERR_LOOT_DIDNT_KILL", 2, "NONE", 31, 0 },
    { "ERR_LOOT_BAD_FACING", 2, "NONE", 32, 0 },
    { "ERR_LOOT_NOTSTANDING", 2, "NONE", 68, 0 },
    { "ERR_LOOT_STUNNED", 2, "NONE", 68, 0 },
    { "ERR_LOOT_NO_UI", 2, "NONE", 68, 0 },
    { "ERR_LOOT_WHILE_INVULNERABLE", 2, "NONE", 68, 0 },
    { "ERR_QUEST_ACCEPTED_S", 0, "QUESTADDED", 68, 0 },
    { "ERR_QUEST_COMPLETE_S", 0, "NONE", 68, 0 },
    { "ERR_QUEST_FAILED_S", 0, "igQuestFailed", 68, 0 },
    { "ERR_QUEST_FAILED_BAG_FULL_S", 0, "igQuestFailed", 68, 0 },
    { "ERR_QUEST_FAILED_MAX_COUNT_S", 0, "igQuestFailed", 68, 0 },
    { "ERR_QUEST_FAILED_LOW_LEVEL", 0, "igQuestFailed", 68, 0 },
    { "ERR_QUEST_FAILED_MISSING_ITEMS", 0, "igQuestFailed", 68, 0 },
    { "ERR_QUEST_FAILED_WRONG_RACE", 0, "igQuestFailed", 68, 0 },
    { "ERR_QUEST_FAILED_NOT_ENOUGH_MONEY", 0, "igQuestFailed", 68, 0 },
    { "ERR_QUEST_FAILED_EXPANSION", 0, "igQuestFailed", 68, 0 },
    { "ERR_QUEST_ONLY_ONE_TIMED", 0, "igQuestFailed", 68, 0 },
    { "ERR_QUEST_NEED_PREREQS", 0, "igQuestFailed", 68, 0 },
    { "ERR_QUEST_ALREADY_ON", 0, "igQuestFailed", 68, 0 },
    { "ERR_QUEST_ALREADY_DONE", 0, "igQuestFailed", 68, 0 },
    { "ERR_QUEST_ALREADY_DONE_DAILY", 0, "igQuestFailed", 68, 0 },
    { "ERR_QUEST_REWARD_EXP_I", 0, "NONE", 68, 0 },
    { "ERR_QUEST_REWARD_ITEM_S", 0, "NONE", 68, 0 },
    { "ERR_QUEST_REWARD_ITEM_MULT_IS", 0, "NONE", 68, 0 },
    { "ERR_QUEST_REWARD_MONEY_S", 0, "NONE", 68, 0 },
    { "ERR_QUEST_MUST_CHOOSE", 2, "igQuestFailed", 68, 0 },
    { "ERR_QUEST_LOG_FULL", 2, "igQuestFailed", 68, 0 },
    { "ERR_COMBAT_DAMAGE_SSI", 0, "NONE", 68, 0 },
    { "ERR_INSPECT_S", 0, "NONE", 68, 0 },
    { "ERR_CANT_USE_ITEM", 2, "NONE", 51, 0 },
    { "ERR_CANT_USE_ITEM_IN_ARENA", 2, "NONE", 51, 0 },
    { "ERR_MUST_EQUIP_ITEM", 2, "NONE", 49, 0 },
    { "ERR_PASSIVE_ABILITY", 2, "NONE", 68, 0 },
    { "ERR_2HSKILLNOTFOUND", 2, "NONE", 43, 0 },
    { "ERR_NO_ATTACK_TARGET", 2, "NONE", 38, 0 },
    { "ERR_INVALID_ATTACK_TARGET", 2, "NONE", 11, 0 },
    { "ERR_ATTACK_STUNNED", 2, "NONE", 68, 0 },
    { "ERR_ATTACK_PACIFIED", 2, "NONE", 68, 0 },
    { "ERR_ATTACK_MOUNTED", 2, "NONE", 68, 0 },
    { "ERR_ATTACK_FLEEING", 2, "NONE", 68, 0 },
    { "ERR_ATTACK_CONFUSED", 2, "NONE", 68, 0 },
    { "ERR_ATTACK_CHARMED", 2, "NONE", 68, 0 },
    { "ERR_ATTACK_DEAD", 2, "NONE", 68, 0 },
    { "ERR_ATTACK_PREVENTED_BY_MECHANIC_S", 2, "NONE", 68, 0 },
    { "ERR_ATTACK_CHANNEL", 2, "NONE", 68, 0 },
    { "ERR_TAXISAMENODE", 2, "NONE", 68, 0 },
    { "ERR_TAXINOSUCHPATH", 1, "NONE", 68, 0 },
    { "ERR_TAXIUNSPECIFIEDSERVERERROR", 2, "NONE", 68, 0 },
    { "ERR_TAXINOTENOUGHMONEY", 1, "NONE", 54, 0 },
    { "ERR_TAXITOOFARAWAY", 1, "NONE", 68, 0 },
    { "ERR_TAXINOVENDORNEARBY", 1, "NONE", 68, 0 },
    { "ERR_TAXINOTVISITED", 1, "NONE", 68, 0 },
    { "ERR_TAXIPLAYERBUSY", 2, "NONE", 68, 0 },
    { "ERR_TAXIPLAYERALREADYMOUNTED", 2, "NONE", 68, 0 },
    { "ERR_TAXIPLAYERSHAPESHIFTED", 2, "NONE", 68, 0 },
    { "ERR_TAXIPLAYERMOVING", 1, "NONE", 68, 0 },
    { "ERR_TAXINOPATHS", 2, "NONE", 68, 0 },
    { "ERR_TAXINOTSTANDING", 1, "NONE", 68, 0 },
    { "ERR_NO_REPLY_TARGET", 0, "NONE", 68, 0 },
    { "ERR_GENERIC_NO_TARGET", 2, "NONE", 45, 0 },
    { "ERR_INITIATE_TRADE_S", 0, "NONE", 68, 0 },
    { "ERR_TRADE_REQUEST_S", 0, "LEVELUP", 68, 0 },
    { "ERR_TRADE_BLOCKED_S", 0, "NONE", 68, 0 },
    { "ERR_TRADE_TARGET_DEAD", 2, "NONE", 68, 0 },
    { "ERR_TRADE_TOO_FAR", 2, "NONE", 68, 0 },
    { "ERR_TRADE_CANCELLED", 1, "NONE", 68, 0 },
    { "ERR_TRADE_COMPLETE", 1, "NONE", 68, 0 },
    { "ERR_TRADE_BAG_FULL", 2, "NONE", 68, 0 },
    { "ERR_TRADE_TARGET_BAG_FULL", 2, "NONE", 68, 0 },
    { "ERR_TRADE_MAX_COUNT_EXCEEDED", 2, "NONE", 68, 0 },
    { "ERR_TRADE_TARGET_MAX_COUNT_EXCEEDED", 2, "NONE", 68, 0 },
    { "ERR_ALREADY_TRADING", 2, "NONE", 68, 0 },
    { "ERR_MOUNT_INVALIDMOUNTEE", 2, "NONE", 68, 0 },
    { "ERR_MOUNT_TOOFARAWAY", 2, "NONE", 68, 0 },
    { "ERR_MOUNT_ALREADYMOUNTED", 2, "NONE", 68, 0 },
    { "ERR_MOUNT_NOTMOUNTABLE", 2, "NONE", 68, 0 },
    { "ERR_MOUNT_NOTYOURPET", 2, "NONE", 68, 0 },
    { "ERR_MOUNT_OTHER", 2, "NONE", 68, 0 },
    { "ERR_MOUNT_LOOTING", 2, "NONE", 68, 0 },
    { "ERR_MOUNT_RACECANTMOUNT", 2, "NONE", 68, 0 },
    { "ERR_MOUNT_SHAPESHIFTED", 2, "NONE", 68, 0 },
    { "ERR_DISMOUNT_NOPET", 2, "NONE", 68, 0 },
    { "ERR_DISMOUNT_NOTMOUNTED", 2, "NONE", 68, 0 },
    { "ERR_DISMOUNT_NOTYOURPET", 2, "NONE", 68, 0 },
    { "ERR_SPELL_FAILED_TOTEMS", 2, "NONE", 68, 0 },
    { "ERR_SPELL_FAILED_REAGENTS", 2, "NONE", 68, 0 },
    { "ERR_SPELL_FAILED_REAGENTS_GENERIC", 2, "NONE", 68, 0 },
    { "ERR_SPELL_FAILED_EQUIPPED_ITEM", 2, "NONE", 68, 0 },
    { "ERR_SPELL_FAILED_EQUIPPED_ITEM_CLASS_S", 2, "NONE", 68, 0 },
    { "ERR_SPELL_FAILED_SHAPESHIFT_FORM_S", 2, "NONE", 68, 0 },
    { "ERR_BADATTACKFACING", 2, "NONE", 68, 0 },
    { "ERR_BADATTACKPOS", 2, "NONE", 68, 0 },
    { "ERR_CHEST_IN_USE", 2, "NONE", 52, 0 },
    { "ERR_USE_CANT_OPEN", 2, "NONE", 68, 0 },
    { "ERR_USE_LOCKED", 2, "NONE", 61, 0 },
    { "ERR_DOOR_LOCKED", 2, "NONE", 61, 0 },
    { "ERR_BUTTON_LOCKED", 2, "NONE", 61, 0 },
    { "ERR_USE_LOCKED_WITH_ITEM_S", 2, "NONE", 68, 0 },
    { "ERR_USE_LOCKED_WITH_SPELL_S", 2, "NONE", 68, 0 },
    { "ERR_USE_LOCKED_WITH_SPELL_KNOWN_SI", 2, "NONE", 68, 0 },
    { "ERR_USE_TOO_FAR", 2, "NONE", 57, 0 },
    { "ERR_USE_BAD_ANGLE", 2, "NONE", 68, 0 },
    { "ERR_USE_OBJECT_MOVING", 2, "NONE", 68, 0 },
    { "ERR_USE_SPELL_FOCUS", 1, "NONE", 68, 0 },
    { "ERR_USE_DESTROYED", 2, "NONE", 68, 0 },
    { "ERR_SET_LOOT_FREEFORALL", 0, "NONE", 68, 0 },
    { "ERR_SET_LOOT_ROUNDROBIN", 0, "NONE", 68, 0 },
    { "ERR_SET_LOOT_MASTER", 0, "NONE", 68, 0 },
    { "ERR_SET_LOOT_GROUP", 0, "NONE", 68, 0 },
    { "ERR_SET_LOOT_NBG", 0, "NONE", 68, 0 },
    { "ERR_SET_LOOT_THRESHOLD_S", 0, "NONE", 68, 0 },
    { "ERR_NEW_LOOT_MASTER_S", 0, "NONE", 68, 0 },
    { "ERR_SPECIFY_MASTER_LOOTER", 2, "NONE", 68, 0 },
    { "ERR_TAME_FAILED", 2, "NONE", 68, 0 },
    { "ERR_CHAT_WHILE_DEAD", 2, "NONE", 68, 0 },
    { "ERR_CHAT_WRONG_FACTION", 0, "NONE", 68, 0 },
    { "ERR_CHAT_PLAYER_NOT_FOUND_S", 0, "NONE", 68, 0 },
    { "ERR_NEWTAXIPATH", 1, "TaxiNodeDiscovered", 68, 0 },
    { "ERR_NO_PET", 2, "NONE", 68, 0 },
    { "ERR_NOTYOURPET", 2, "NONE", 68, 0 },
    { "ERR_PET_NOT_RENAMEABLE", 2, "NONE", 68, 0 },
    { "ERR_QUEST_OBJECTIVE_COMPLETE_S", 1, "NONE", 68, 0 },
    { "ERR_QUEST_UNKNOWN_COMPLETE", 1, "NONE", 68, 0 },
    { "ERR_QUEST_ADD_KILL_SII", 1, "NONE", 68, 0 },
    { "ERR_QUEST_ADD_FOUND_SII", 1, "NONE", 68, 0 },
    { "ERR_QUEST_ADD_ITEM_SII", 1, "NONE", 68, 0 },
    { "ERR_QUEST_ADD_PLAYER_KILL_SII", 1, "NONE", 68, 0 },
    { "ERR_CANNOTCREATEDIRECTORY", 2, "NONE", 68, 0 },
    { "ERR_CANNOTCREATEFILE", 2, "NONE", 68, 0 },
    { "ERR_PLAYER_WRONG_FACTION", 2, "NONE", 68, 0 },
    { "ERR_BANKSLOT_FAILED_TOO_MANY", 2, "NONE", 68, 0 },
    { "ERR_BANKSLOT_INSUFFICIENT_FUNDS", 2, "NONE", 22, 0 },
    { "ERR_BANKSLOT_NOTBANKER", 2, "NONE", 68, 0 },
    { "ERR_FRIEND_DB_ERROR", 0, "NONE", 68, 0 },
    { "ERR_FRIEND_LIST_FULL", 0, "NONE", 68, 0 },
    { "ERR_FRIEND_ADDED_S", 0, "NONE", 68, 0 },
    { "ERR_FRIEND_ONLINE_SS", 0, "FRIENDJOINGAME", 68, 0 },
    { "ERR_FRIEND_OFFLINE_S", 0, "NONE", 68, 0 },
    { "ERR_FRIEND_NOT_FOUND", 0, "NONE", 68, 0 },
    { "ERR_FRIEND_WRONG_FACTION", 0, "NONE", 68, 0 },
    { "ERR_FRIEND_REMOVED_S", 0, "NONE", 68, 0 },
    { "ERR_FRIEND_ERROR", 0, "NONE", 68, 0 },
    { "ERR_FRIEND_ALREADY_S", 0, "NONE", 68, 0 },
    { "ERR_FRIEND_SELF", 0, "NONE", 68, 0 },
    { "ERR_FRIEND_DELETED", 0, "NONE", 68, 0 },
    { "ERR_IGNORE_FULL", 0, "NONE", 68, 0 },
    { "ERR_IGNORE_SELF", 0, "NONE", 68, 0 },
    { "ERR_IGNORE_NOT_FOUND", 0, "NONE", 68, 0 },
    { "ERR_IGNORE_ALREADY_S", 0, "NONE", 68, 0 },
    { "ERR_IGNORE_ADDED_S", 0, "NONE", 68, 0 },
    { "ERR_IGNORE_REMOVED_S", 0, "NONE", 68, 0 },
    { "ERR_IGNORE_AMBIGUOUS", 0, "NONE", 68, 0 },
    { "ERR_IGNORE_DELETED", 0, "NONE", 68, 0 },
    { "ERR_ONLY_ONE_BOLT", 2, "NONE", 68, 0 },
    { "ERR_ONLY_ONE_AMMO", 2, "NONE", 68, 0 },
    { "ERR_SPELL_FAILED_EQUIPPED_SPECIFIC_ITEM", 2, "NONE", 68, 0 },
    { "ERR_WRONG_BAG_TYPE_SUBCLASS", 2, "NONE", 28, 0 },
    { "ERR_CANT_WRAP_STACKABLE", 2, "NONE", 68, 0 },
    { "ERR_CANT_WRAP_EQUIPPED", 2, "NONE", 68, 0 },
    { "ERR_CANT_WRAP_WRAPPED", 2, "NONE", 68, 0 },
    { "ERR_CANT_WRAP_BOUND", 2, "NONE", 68, 0 },
    { "ERR_CANT_WRAP_UNIQUE", 2, "NONE", 68, 0 },
    { "ERR_CANT_WRAP_BAGS", 2, "NONE", 68, 0 },
    { "ERR_OUT_OF_MANA", 2, "NONE", 15, 0 },
    { "ERR_OUT_OF_RAGE", 2, "NONE", 63, 0 },
    { "ERR_OUT_OF_FOCUS", 2, "NONE", 68, 0 },
    { "ERR_OUT_OF_ENERGY", 2, "NONE", 64, 0 },
    { "ERR_OUT_OF_HEALTH", 2, "NONE", 68, 0 },
    { "ERR_OUT_OF_RUNES", 2, "NONE", 68, 0 },
    { "ERR_OUT_OF_RUNIC_POWER", 2, "NONE", 68, 0 },
    { "ERR_OUT_OF_POWER_DISPLAY", 2, "NONE", 68, 0 },
    { "ERR_LOOT_GONE", 2, "NONE", 68, 0 },
    { "ERR_MOUNT_FORCEDDISMOUNT", 2, "NONE", 68, 0 },
    { "ERR_AUTOFOLLOW_TOO_FAR", 2, "NONE", 68, 0 },
    { "ERR_UNIT_NOT_FOUND", 2, "NONE", 68, 0 },
    { "ERR_INVALID_FOLLOW_TARGET", 2, "NONE", 68, 0 },
    { "ERR_INVALID_INSPECT_TARGET", 2, "NONE", 68, 0 },
    { "ERR_GUILDEMBLEM_SUCCESS", 2, "NONE", 68, 0 },
    { "ERR_GUILDEMBLEM_INVALID_TABARD_COLORS", 2, "NONE", 68, 0 },
    { "ERR_GUILDEMBLEM_NOGUILD", 2, "NONE", 68, 0 },
    { "ERR_GUILDEMBLEM_NOTGUILDMASTER", 2, "NONE", 68, 0 },
    { "ERR_GUILDEMBLEM_NOTENOUGHMONEY", 2, "NONE", 40, 0 },
    { "ERR_GUILDEMBLEM_INVALIDVENDOR", 2, "NONE", 68, 0 },
    { "ERR_EMBLEMERROR_NOTABARDGEOSET", 2, "NONE", 68, 0 },
    { "ERR_SPELL_OUT_OF_RANGE", 2, "NONE", 46, 0 },
    { "ERR_COMMAND_NEEDS_TARGET", 0, "NONE", 68, 0 },
    { "ERR_NOAMMO_S", 2, "NONE", 1, 0 },
    { "ERR_TOOBUSYTOFOLLOW", 2, "NONE", 68, 0 },
    { "ERR_DUEL_REQUESTED", 0, "LEVELUP", 68, 0 },
    { "ERR_DUEL_CANCELLED", 1, "NONE", 68, 0 },
    { "ERR_DEATHBINDALREADYBOUND", 2, "NONE", 68, 0 },
    { "ERR_DEATHBIND_SUCCESS_S", 0, "NONE", 68, 0 },
    { "ERR_NOEMOTEWHILERUNNING", 2, "NONE", 68, 0 },
    { "ERR_ZONE_EXPLORED", 1, "NONE", 68, 0 },
    { "ERR_ZONE_EXPLORED_XP", 0, "NONE", 68, 0 },
    { "ERR_INVALID_ITEM_TARGET", 2, "NONE", 51, 0 },
    { "ERR_IGNORING_YOU_S", 0, "NONE", 68, 0 },
    { "ERR_FISH_NOT_HOOKED", 1, "NONE", 68, 0 },
    { "ERR_FISH_ESCAPED", 1, "NONE", 68, 0 },
    { "ERR_SPELL_FAILED_NOTUNSHEATHED", 1, "NONE", 68, 0 },
    { "ERR_PETITION_OFFERED_S", 0, "NONE", 68, 0 },
    { "ERR_PETITION_SIGNED", 0, "igPlayerInviteAccept", 68, 0 },
    { "ERR_PETITION_SIGNED_S", 0, "igPlayerInviteAccept", 68, 0 },
    { "ERR_PETITION_DECLINED_S", 0, "igPlayerInviteDecline", 68, 0 },
    { "ERR_PETITION_ALREADY_SIGNED", 2, "igPlayerInviteDecline", 68, 0 },
    { "ERR_PETITION_RESTRICTED_ACCOUNT", 2, "igPlayerInviteDecline", 68, 0 },
    { "ERR_PETITION_ALREADY_SIGNED_OTHER", 2, "igPlayerInviteDecline", 68, 0 },
    { "ERR_PETITION_IN_GUILD", 2, "igPlayerInviteDecline", 68, 0 },
    { "ERR_PETITION_CREATOR", 2, "igPlayerInviteDecline", 68, 0 },
    { "ERR_PETITION_NOT_ENOUGH_SIGNATURES", 2, "igPlayerInviteDecline", 68, 0 },
    { "ERR_PETITION_NOT_SAME_SERVER", 2, "igPlayerInviteDecline", 68, 0 },
    { "ERR_PETITION_FULL", 2, "igPlayerInviteDecline", 68, 0 },
    { "ERR_SPELL_UNLEARNED_S", 0, "NONE", 68, 0 },
    { "ERR_PET_SPELL_ROOTED", 2, "NONE", 68, 0 },
    { "ERR_PET_SPELL_AFFECTING_COMBAT", 2, "NONE", 68, 0 },
    { "ERR_PET_SPELL_OUT_OF_RANGE", 2, "NONE", 68, 0 },
    { "ERR_PET_SPELL_NOT_BEHIND", 2, "NONE", 68, 0 },
    { "ERR_PET_SPELL_TARGETS_DEAD", 2, "NONE", 68, 0 },
    { "ERR_PET_SPELL_DEAD", 2, "NONE", 68, 0 },
    { "ERR_PET_SPELL_NOPATH", 2, "NONE", 68, 0 },
    { "ERR_ITEM_CANT_BE_DESTROYED", 2, "NONE", 68, 0 },
    { "ERR_TICKET_ALREADY_EXISTS", 2, "NONE", 68, 0 },
    { "ERR_TICKET_CREATE_ERROR", 2, "NONE", 68, 0 },
    { "ERR_TICKET_UPDATE_ERROR", 2, "NONE", 68, 0 },
    { "ERR_TICKET_DB_ERROR", 2, "NONE", 68, 0 },
    { "ERR_TICKET_NO_TEXT", 2, "NONE", 68, 0 },
    { "ERR_TICKET_TEXT_TOO_LONG", 2, "NONE", 68, 0 },
    { "ERR_OBJECT_IS_BUSY", 2, "NONE", 68, 0 },
    { "ERR_EXHAUSTION_WELLRESTED", 0, "NONE", 68, 0 },
    { "ERR_EXHAUSTION_RESTED", 0, "NONE", 68, 0 },
    { "ERR_EXHAUSTION_NORMAL", 0, "NONE", 68, 0 },
    { "ERR_EXHAUSTION_TIRED", 0, "NONE", 68, 0 },
    { "ERR_EXHAUSTION_EXHAUSTED", 0, "NONE", 68, 0 },
    { "ERR_NO_ITEMS_WHILE_SHAPESHIFTED", 2, "NONE", 68, 0 },
    { "ERR_CANT_INTERACT_SHAPESHIFTED", 2, "NONE", 68, 0 },
    { "ERR_MAIL_QUEST_ITEM", 2, "NONE", 59, 0 },
    { "ERR_MAIL_BOUND_ITEM", 2, "NONE", 59, 0 },
    { "ERR_MAIL_CONJURED_ITEM", 2, "NONE", 68, 0 },
    { "ERR_MAIL_BAG", 2, "NONE", 68, 0 },
    { "ERR_MAIL_TO_SELF", 2, "NONE", 68, 0 },
    { "ERR_MAIL_TARGET_NOT_FOUND", 2, "NONE", 68, 0 },
    { "ERR_MAIL_DATABASE_ERROR", 2, "NONE", 68, 0 },
    { "ERR_MAIL_WRAPPED_COD", 2, "NONE", 68, 0 },
    { "ERR_MAIL_SENT", 1, "NONE", 68, 0 },
    { "ERR_NOT_HAPPY_ENOUGH", 2, "NONE", 68, 0 },
    { "ERR_USE_CANT_IMMUNE", 2, "NONE", 68, 0 },
    { "ERR_CANT_BE_DISENCHANTED", 2, "NONE", 68, 0 },
    { "ERR_CANT_USE_DISARMED", 2, "NONE", 68, 0 },
    { "ERR_AUCTION_QUEST_ITEM", 2, "NONE", 59, 0 },
    { "ERR_AUCTION_BOUND_ITEM", 2, "NONE", 59, 0 },
    { "ERR_AUCTION_CONJURED_ITEM", 2, "NONE", 68, 0 },
    { "ERR_AUCTION_LIMITED_DURATION_ITEM", 2, "NONE", 68, 0 },
    { "ERR_AUCTION_WRAPPED_ITEM", 2, "NONE", 68, 0 },
    { "ERR_AUCTION_LOOT_ITEM", 2, "NONE", 68, 0 },
    { "ERR_AUCTION_BAG", 2, "NONE", 68, 0 },
    { "ERR_AUCTION_DATABASE_ERROR", 2, "NONE", 68, 0 },
    { "ERR_AUCTION_BID_OWN", 2, "NONE", 68, 0 },
    { "ERR_AUCTION_BID_INCREMENT", 2, "NONE", 68, 0 },
    { "ERR_AUCTION_HIGHER_BID", 2, "NONE", 68, 0 },
    { "ERR_AUCTION_MIN_BID", 2, "NONE", 68, 0 },
    { "ERR_AUCTION_REPAIR_ITEM", 2, "NONE", 68, 0 },
    { "ERR_AUCTION_USED_CHARGES", 2, "NONE", 68, 0 },
    { "ERR_AUCTION_STARTED", 0, "NONE", 68, 0 },
    { "ERR_AUCTION_REMOVED", 0, "NONE", 68, 0 },
    { "ERR_AUCTION_OUTBID_S", 0, "NONE", 68, 0 },
    { "ERR_AUCTION_WON_S", 0, "NONE", 68, 0 },
    { "ERR_AUCTION_SOLD_S", 0, "NONE", 68, 0 },
    { "ERR_AUCTION_EXPIRED_S", 0, "NONE", 68, 0 },
    { "ERR_AUCTION_REMOVED_S", 0, "NONE", 68, 0 },
    { "ERR_AUCTION_BID_PLACED", 0, "NONE", 68, 0 },
    { "ERR_LOGOUT_FAILED", 2, "NONE", 68, 0 },
    { "ERR_QUEST_PUSH_SUCCESS_S", 0, "NONE", 68, 0 },
    { "ERR_QUEST_PUSH_INVALID_S", 0, "NONE", 68, 0 },
    { "ERR_QUEST_PUSH_ACCEPTED_S", 0, "NONE", 68, 0 },
    { "ERR_QUEST_PUSH_DECLINED_S", 0, "NONE", 68, 0 },
    { "ERR_QUEST_PUSH_BUSY_S", 0, "NONE", 68, 0 },
    { "ERR_QUEST_PUSH_LOG_FULL_S", 0, "NONE", 68, 0 },
    { "ERR_QUEST_PUSH_ONQUEST_S", 0, "NONE", 68, 0 },
    { "ERR_QUEST_PUSH_ALREADY_DONE_S", 0, "NONE", 68, 0 },
    { "ERR_QUEST_PUSH_NOT_DAILY_S", 0, "NONE", 68, 0 },
    { "ERR_QUEST_PUSH_TIMER_EXPIRED_S", 0, "NONE", 68, 0 },
    { "ERR_QUEST_PUSH_NOT_IN_PARTY_S", 0, "NONE", 68, 0 },
    { "ERR_QUEST_PUSH_DIFFERENT_SERVER_DAILY_S", 0, "NONE", 68, 0 },
    { "ERR_RAID_GROUP_LOWLEVEL", 2, "NONE", 68, 0 },
    { "ERR_RAID_GROUP_ONLY", 2, "NONE", 68, 0 },
    { "ERR_RAID_GROUP_FULL", 2, "NONE", 68, 0 },
    { "ERR_RAID_GROUP_REQUIREMENTS_UNMATCH", 2, "NONE", 68, 0 },
    { "ERR_CORPSE_IS_NOT_IN_INSTANCE", 2, "NONE", 68, 0 },
    { "ERR_PVP_KILL_HONORABLE", 1, "NONE", 68, 0 },
    { "ERR_PVP_KILL_DISHONORABLE", 1, "NONE", 68, 0 },
    { "ERR_SPELL_FAILED_ALREADY_AT_FULL_HEALTH", 2, "NONE", 68, 0 },
    { "ERR_SPELL_FAILED_ALREADY_AT_FULL_MANA", 2, "NONE", 68, 0 },
    { "ERR_SPELL_FAILED_ALREADY_AT_FULL_POWER_S", 2, "NONE", 68, 0 },
    { "ERR_AUTOLOOT_MONEY_S", 0, "NONE", 68, 0 },
    { "ERR_GENERIC_STUNNED", 2, "NONE", 68, 0 },
    { "ERR_TARGET_STUNNED", 2, "NONE", 68, 0 },
    { "ERR_MUST_REPAIR_DURABILITY", 2, "NONE", 68, 0 },
    { "ERR_RAID_YOU_JOINED", 0, "NONE", 68, 0 },
    { "ERR_RAID_YOU_LEFT", 0, "NONE", 68, 0 },
    { "ERR_RAID_MEMBER_ADDED_S", 0, "NONE", 68, 0 },
    { "ERR_RAID_MEMBER_REMOVED_S", 0, "NONE", 68, 0 },
    { "ERR_CLICK_ON_ITEM_TO_FEED", 2, "NONE", 68, 0 },
    { "ERR_TOO_MANY_CHAT_CHANNELS", 0, "NONE", 68, 0 },
    { "ERR_LOOT_ROLL_PENDING", 2, "NONE", 68, 0 },
    { "ERR_LOOT_PLAYER_NOT_FOUND", 2, "NONE", 68, 0 },
    { "ERR_NOT_IN_RAID", 0, "NONE", 68, 0 },
    { "ERR_LOGGING_OUT", 2, "NONE", 68, 0 },
    { "ERR_TARGET_LOGGING_OUT", 2, "NONE", 68, 0 },
    { "ERR_NOT_WHILE_MOUNTED", 2, "NONE", 68, 0 },
    { "ERR_NOT_WHILE_SHAPESHIFTED", 2, "NONE", 68, 0 },
    { "ERR_NOT_IN_COMBAT", 2, "NONE", 68, 0 },
    { "ERR_NOT_WHILE_DISARMED", 2, "NONE", 68, 0 },
    { "ERR_PET_BROKEN", 2, "NONE", 68, 0 },
    { "ERR_TALENT_WIPE_ERROR", 2, "NONE", 68, 0 },
    { "ERR_FEIGN_DEATH_RESISTED", 2, "NONE", 68, 0 },
    { "ERR_MEETING_STONE_IN_QUEUE_S", 0, "PVPENTERQUEUE", 68, 0 },
    { "ERR_MEETING_STONE_LEFT_QUEUE_S", 0, "NONE", 68, 0 },
    { "ERR_MEETING_STONE_OTHER_MEMBER_LEFT", 0, "NONE", 68, 0 },
    { "ERR_MEETING_STONE_PARTY_KICKED_FROM_QUEUE", 0, "NONE", 68, 0 },
    { "ERR_MEETING_STONE_MEMBER_STILL_IN_QUEUE", 0, "NONE", 68, 0 },
    { "ERR_MEETING_STONE_SUCCESS", 0, "NONE", 68, 0 },
    { "ERR_MEETING_STONE_IN_PROGRESS", 0, "NONE", 68, 0 },
    { "ERR_MEETING_STONE_MEMBER_ADDED_S", 0, "NONE", 68, 0 },
    { "ERR_MEETING_STONE_GROUP_FULL", 2, "NONE", 68, 0 },
    { "ERR_MEETING_STONE_NOT_LEADER", 2, "NONE", 68, 0 },
    { "ERR_MEETING_STONE_INVALID_LEVEL", 2, "NONE", 68, 0 },
    { "ERR_MEETING_STONE_TARGET_NOT_IN_PARTY", 2, "NONE", 68, 0 },
    { "ERR_MEETING_STONE_TARGET_INVALID_LEVEL", 2, "NONE", 68, 0 },
    { "ERR_MEETING_STONE_MUST_BE_LEADER", 2, "NONE", 68, 0 },
    { "ERR_MEETING_STONE_NO_RAID_GROUP", 2, "NONE", 68, 0 },
    { "ERR_MEETING_STONE_NEED_PARTY", 2, "NONE", 68, 0 },
    { "ERR_MEETING_STONE_NOT_FOUND", 2, "NONE", 68, 0 },
    { "ERR_GUILDEMBLEM_SAME", 2, "NONE", 68, 0 },
    { "ERR_EQUIP_TRADE_ITEM", 2, "NONE", 68, 0 },
    { "ERR_PVP_TOGGLE_ON", 1, "NONE", 68, 0 },
    { "ERR_PVP_TOGGLE_OFF", 1, "NONE", 68, 0 },
    { "ERR_GROUP_JOIN_BATTLEGROUND_DESERTERS", 1, "NONE", 68, 0 },
    { "ERR_GROUP_JOIN_BATTLEGROUND_S", 1, "NONE", 68, 0 },
    { "ERR_GROUP_JOIN_BATTLEGROUND_FAIL", 2, "NONE", 68, 0 },
    { "ERR_GROUP_JOIN_BATTLEGROUND_TOO_MANY", 2, "NONE", 68, 0 },
    { "ERR_BATTLEGROUND_TOO_MANY_QUEUES", 2, "NONE", 68, 0 },
    { "ERR_BATTLEGROUND_CANNOT_QUEUE_FOR_RATED", 2, "NONE", 68, 0 },
    { "ERR_BATTLEDGROUND_QUEUED_FOR_RATED", 2, "NONE", 68, 0 },
    { "ERR_BATTLEGROUND_TEAM_LEFT_QUEUE", 2, "NONE", 68, 0 },
    { "ERR_BATTLEGROUND_NOT_IN_BATTLEGROUND", 2, "NONE", 68, 0 },
    { "ERR_INVALID_PROMOTION_CODE", 2, "NONE", 68, 0 },
    { "ERR_BG_PLAYER_JOINED_SS", 0, "NONE", 68, 0 },
    { "ERR_BG_PLAYER_LEFT_S", 0, "NONE", 68, 0 },
    { "ERR_RESTRICTED_ACCOUNT", 2, "NONE", 68, 0 },
    { "ERR_PLAY_TIME_EXCEEDED", 2, "NONE", 68, 0 },
    { "ERR_APPROACHING_PARTIAL_PLAY_TIME", 0, "NONE", 68, 0 },
    { "ERR_APPROACHING_PARTIAL_PLAY_TIME_2", 0, "NONE", 68, 0 },
    { "ERR_APPROACHING_NO_PLAY_TIME", 0, "NONE", 68, 0 },
    { "ERR_APPROACHING_NO_PLAY_TIME_2", 0, "NONE", 68, 0 },
    { "ERR_UNHEALTHY_TIME", 0, "NONE", 68, 0 },
    { "ERR_CHAT_RESTRICTED", 0, "NONE", 68, 0 },
    { "ERR_CHAT_THROTTLED", 0, "NONE", 68, 0 },
    { "ERR_MAIL_REACHED_CAP", 2, "NONE", 68, 0 },
    { "ERR_INVALID_RAID_TARGET", 2, "NONE", 68, 0 },
    { "ERR_RAID_LEADER_READY_CHECK_START_S", 0, "NONE", 68, 0 },
    { "ERR_READY_CHECK_IN_PROGRESS", 2, "NONE", 68, 0 },
    { "ERR_READY_CHECK_THROTTLED", 2, "NONE", 68, 0 },
    { "ERR_DUNGEON_DIFFICULTY_FAILED", 0, "NONE", 68, 0 },
    { "ERR_DUNGEON_DIFFICULTY_CHANGED_S", 0, "NONE", 68, 0 },
    { "ERR_TRADE_WRONG_REALM", 2, "NONE", 68, 0 },
    { "ERR_TRADE_NOT_ON_TAPLIST", 2, "NONE", 68, 0 },
    { "ERR_CHAT_PLAYER_AMBIGUOUS_S", 0, "NONE", 68, 0 },
    { "ERR_LOOT_CANT_LOOT_THAT_NOW", 2, "NONE", 68, 0 },
    { "ERR_LOOT_MASTER_INV_FULL", 2, "NONE", 68, 0 },
    { "ERR_LOOT_MASTER_UNIQUE_ITEM", 2, "NONE", 68, 0 },
    { "ERR_LOOT_MASTER_OTHER", 2, "NONE", 68, 0 },
    { "ERR_FILTERING_YOU_S", 0, "NONE", 68, 0 },
    { "ERR_USE_PREVENTED_BY_MECHANIC_S", 2, "NONE", 68, 0 },
    { "ERR_ITEM_UNIQUE_EQUIPPABLE", 2, "NONE", 68, 0 },
    { "ERR_LFG_LEADER_IS_LFM_S", 2, "NONE", 68, 0 },
    { "ERR_LFG_PENDING", 0, "NONE", 68, 0 },
    { "ERR_ARENA_NO_TEAM_II", 2, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_CREATE_S", 0, "LEVELUP", 68, 0 },
    { "ERR_ARENA_TEAM_INVITE_SS", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_QUIT_S", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_FOUNDER_S", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_INTERNAL", 0, "NONE", 68, 0 },
    { "ERR_ALREADY_IN_ARENA_TEAM", 0, "NONE", 68, 0 },
    { "ERR_ALREADY_IN_ARENA_TEAM_S", 0, "NONE", 68, 0 },
    { "ERR_INVITED_TO_ARENA_TEAM", 0, "LEVELUP", 68, 0 },
    { "ERR_ALREADY_INVITED_TO_ARENA_TEAM_S", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_NAME_INVALID", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_NAME_EXISTS_S", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_LEADER_LEAVE_S", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_PERMISSIONS", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_PLAYER_NOT_IN_TEAM", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_PLAYER_NOT_IN_TEAM_SS", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_PLAYER_NOT_FOUND_S", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_NOT_ALLIED", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_JOIN_SS", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_YOU_JOIN_S", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_LEAVE_SS", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_LEADER_IS_SS", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_LEADER_CHANGED_SSS", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_REMOVE_SSS", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_DISBANDED_S", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_TARGET_TOO_LOW_S", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_TARGET_TOO_HIGH_S", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_TOO_MANY_MEMBERS_S", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_LEVEL_TOO_LOW_I", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_NOT_FOUND", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAMS_LOCKED", 0, "NONE", 68, 0 },
    { "ERR_NO_ARENA_CHARTER", 2, "NONE", 68, 0 },
    { "ERR_CANT_SPEAK_LANGAGE", 0, "NONE", 68, 0 },
    { "ERR_VENDOR_MISSING_TURNINS", 2, "NONE", 68, 0 },
    { "ERR_BATTLEGROUND_NOT_IN_TEAM", 2, "NONE", 68, 0 },
    { "ERR_NOT_IN_BATTLEGROUND", 0, "NONE", 68, 0 },
    { "ERR_NOT_ENOUGH_HONOR_POINTS", 2, "NONE", 68, 0 },
    { "ERR_NOT_ENOUGH_ARENA_POINTS", 2, "NONE", 68, 0 },
    { "ERR_SOCKETING_REQUIRES_META_GEM", 2, "NONE", 68, 0 },
    { "ERR_SOCKETING_META_GEM_ONLY_IN_METASLOT", 2, "NONE", 68, 0 },
    { "ERR_ITEM_MAX_COUNT_SOCKETED", 2, "NONE", 68, 0 },
    { "ERR_SYSTEM_DISABLED", 2, "NONE", 68, 0 },
    { "ERR_QUEST_FAILED_TOO_MANY_DAILY_QUESTS_I", 0, "igQuestFailed", 68, 0 },
    { "ERR_ITEM_MAX_COUNT_EQUIPPED_SOCKETED", 2, "NONE", 68, 0 },
    { "ERR_ITEM_UNIQUE_EQUIPPABLE_SOCKETED", 2, "NONE", 68, 0 },
    { "ERR_USER_SQUELCHED", 0, "NONE", 68, 0 },
    { "ERR_TOO_MUCH_GOLD", 2, "NONE", 68, 0 },
    { "ERR_NOT_BARBER_SITTING", 2, "NONE", 68, 0 },
    { "ERR_QUEST_FAILED_CAIS", 0, "igQuestFailed", 68, 0 },
    { "ERR_INVITE_RESTRICTED", 0, "NONE", 68, 0 },
    { "ERR_VOICE_IGNORE_FULL", 0, "NONE", 68, 0 },
    { "ERR_VOICE_IGNORE_SELF", 0, "NONE", 68, 0 },
    { "ERR_VOICE_IGNORE_NOT_FOUND", 0, "NONE", 68, 0 },
    { "ERR_VOICE_IGNORE_ALREADY_S", 0, "NONE", 68, 0 },
    { "ERR_VOICE_IGNORE_ADDED_S", 0, "NONE", 68, 0 },
    { "ERR_VOICE_IGNORE_REMOVED_S", 0, "NONE", 68, 0 },
    { "ERR_VOICE_IGNORE_AMBIGUOUS", 0, "NONE", 68, 0 },
    { "ERR_VOICE_IGNORE_DELETED", 0, "NONE", 68, 0 },
    { "ERR_UNKNOWN_MACRO_OPTION_S", 0, "NONE", 68, 0 },
    { "ERR_NOT_DURING_ARENA_MATCH", 2, "NONE", 68, 0 },
    { "ERR_PLAYER_SILENCED", 0, "NONE", 68, 0 },
    { "ERR_PLAYER_UNSILENCED", 0, "NONE", 68, 0 },
    { "ERR_COMSAT_DISCONNECT", 0, "NONE", 68, 0 },
    { "ERR_COMSAT_RECONNECT_ATTEMPT", 0, "NONE", 68, 0 },
    { "ERR_COMSAT_CONNECT_FAIL", 0, "NONE", 68, 0 },
    { "ERR_DANCE_CREATE_DUPLICATE", 0, "NONE", 68, 0 },
    { "ERR_DANCE_SAVE_FAILED", 0, "NONE", 68, 0 },
    { "ERR_DANCE_DELETE_FAILED", 0, "NONE", 68, 0 },
    { "ERR_MAIL_INVALID_ATTACHMENT_SLOT", 2, "NONE", 68, 0 },
    { "ERR_MAIL_TOO_MANY_ATTACHMENTS", 2, "NONE", 68, 0 },
    { "ERR_MAIL_INVALID_ATTACHMENT", 2, "NONE", 68, 0 },
    { "ERR_MAIL_ATTACHMENT_EXPIRED", 2, "NONE", 68, 0 },
    { "ERR_VOICE_CHAT_PARENTAL_DISABLE_ALL", 2, "NONE", 68, 0 },
    { "ERR_VOICE_CHAT_PARENTAL_DISABLE_MIC", 2, "NONE", 68, 0 },
    { "ERR_PROFANE_CHAT_NAME", 2, "NONE", 68, 0 },
    { "ERR_PLAYER_SILENCED_ECHO", 0, "NONE", 68, 0 },
    { "ERR_PLAYER_UNSILENCED_ECHO", 0, "NONE", 68, 0 },
    { "ERR_VOICESESSION_FULL", 0, "NONE", 68, 0 },
    { "ERR_LOOT_CANT_LOOT_THAT", 2, "NONE", 68, 0 },
    { "ERR_ARENA_EXPIRED_CAIS", 2, "NONE", 68, 0 },
    { "ERR_GROUP_ACTION_THROTTLED", 0, "NONE", 68, 0 },
    { "ERR_ALREADY_PICKPOCKETED", 2, "NONE", 68, 0 },
    { "ERR_NAME_INVALID", 2, "NONE", 68, 0 },
    { "ERR_NAME_NO_NAME", 2, "NONE", 68, 0 },
    { "ERR_NAME_TOO_SHORT", 2, "NONE", 68, 0 },
    { "ERR_NAME_TOO_LONG", 2, "NONE", 68, 0 },
    { "ERR_NAME_MIXED_LANGUAGES", 2, "NONE", 68, 0 },
    { "ERR_NAME_PROFANE", 2, "NONE", 68, 0 },
    { "ERR_NAME_RESERVED", 2, "NONE", 68, 0 },
    { "ERR_NAME_THREE_CONSECUTIVE", 2, "NONE", 68, 0 },
    { "ERR_NAME_INVALID_SPACE", 2, "NONE", 68, 0 },
    { "ERR_NAME_CONSECUTIVE_SPACES", 2, "NONE", 68, 0 },
    { "ERR_NAME_RUSSIAN_CONSECUTIVE_SILENT_CHARACTERS", 2, "NONE", 68, 0 },
    { "ERR_NAME_RUSSIAN_SILENT_CHARACTER_AT_BEGINNING_OR_END", 2, "NONE", 68, 0 },
    { "ERR_NAME_DECLENSION_DOESNT_MATCH_BASE_NAME", 2, "NONE", 68, 0 },
    { "ERR_REFER_A_FRIEND_NOT_REFERRED_BY", 2, "NONE", 68, 0 },
    { "ERR_REFER_A_FRIEND_TARGET_TOO_HIGH", 2, "NONE", 68, 0 },
    { "ERR_REFER_A_FRIEND_INSUFFICIENT_GRANTABLE_LEVELS", 2, "NONE", 68, 0 },
    { "ERR_REFER_A_FRIEND_TOO_FAR", 2, "NONE", 68, 0 },
    { "ERR_REFER_A_FRIEND_DIFFERENT_FACTION", 2, "NONE", 68, 0 },
    { "ERR_REFER_A_FRIEND_NOT_NOW", 2, "NONE", 68, 0 },
    { "ERR_REFER_A_FRIEND_GRANT_LEVEL_MAX_I", 2, "NONE", 68, 0 },
    { "ERR_REFER_A_FRIEND_SUMMON_LEVEL_MAX_I", 2, "NONE", 68, 0 },
    { "ERR_REFER_A_FRIEND_SUMMON_COOLDOWN", 2, "NONE", 68, 0 },
    { "ERR_REFER_A_FRIEND_SUMMON_OFFLINE_S", 2, "NONE", 68, 0 },
    { "ERR_REFER_A_FRIEND_INSUF_EXPAN_LVL", 2, "NONE", 68, 0 },
    { "ERR_NOT_SAME_ACCOUNT", 2, "NONE", 59, 0 },
    { "ERR_BAD_ON_USE_ENCHANT", 2, "NONE", 68, 0 },
    { "ERR_TRADE_SELF", 2, "NONE", 68, 0 },
    { "ERR_TOO_MANY_SOCKETS", 2, "NONE", 68, 0 },
    { "ERR_ITEM_MAX_LIMIT_CATEGORY_COUNT_EXCEEDED_IS", 2, "NONE", 68, 0 },
    { "ERR_TRADE_TARGET_MAX_LIMIT_CATEGORY_COUNT_EXCEEDED_IS", 2, "NONE", 68, 0 },
    { "ERR_ITEM_MAX_LIMIT_CATEGORY_SOCKETED_EXCEEDED_IS", 2, "NONE", 68, 0 },
    { "ERR_ITEM_MAX_LIMIT_CATEGORY_EQUIPPED_EXCEEDED_IS", 2, "NONE", 68, 0 },
    { "ERR_SHAPESHIFT_FORM_CANNOT_EQUIP", 2, "NONE", 68, 0 },
    { "ERR_ITEM_INVENTORY_FULL_SATCHEL", 2, "NONE", 68, 0 },
    { "ERR_SCALING_STAT_ITEM_LEVEL_EXCEEDED", 2, "NONE", 68, 0 },
    { "ERR_PURCHASE_LEVEL_TOO_LOW", 2, "NONE", 68, 0 },
    { "ERR_GROUP_SWAP_FAILED", 2, "NONE", 68, 0 },
    { "ERR_INVITE_IN_COMBAT", 2, "NONE", 68, 0 },
    { "ERR_INVALID_GLYPH_SLOT", 2, "NONE", 68, 0 },
    { "ERR_GENERIC_NO_VALID_TARGETS", 2, "NONE", 45, 0 },
    { "ERR_CALENDAR_EVENT_ALERT_S", 0, "NONE", 68, 0 },
    { "ERR_PET_LEARN_SPELL_S", 0, "NONE", 68, 0 },
    { "ERR_PET_LEARN_ABILITY_S", 0, "NONE", 68, 0 },
    { "ERR_PET_SPELL_UNLEARNED_S", 0, "NONE", 68, 0 },
    { "ERR_INVITE_UNKNOWN_REALM", 2, "NONE", 68, 0 },
    { "ERR_INVITE_NO_PARTY_SERVER", 2, "NONE", 68, 0 },
    { "ERR_INVITE_PARTY_BUSY", 2, "NONE", 68, 0 },
    { "ERR_PARTY_TARGET_AMBIGUOUS", 2, "NONE", 68, 0 },
    { "ERR_PARTY_LFG_INVITE_RAID_LOCKED", 0, "NONE", 68, 0 },
    { "ERR_PARTY_LFG_BOOT_LIMIT", 2, "NONE", 68, 0 },
    { "ERR_PARTY_LFG_BOOT_COOLDOWN_S", 2, "NONE", 68, 0 },
    { "ERR_PARTY_LFG_BOOT_NOT_ELIGIBLE_S", 2, "NONE", 68, 0 },
    { "ERR_PARTY_LFG_BOOT_IN_PROGRESS", 2, "NONE", 68, 0 },
    { "ERR_PARTY_LFG_BOOT_TOO_FEW_PLAYERS", 2, "NONE", 68, 0 },
    { "ERR_PARTY_LFG_BOOT_VOTE_SUCCEEDED", 0, "NONE", 68, 0 },
    { "ERR_PARTY_LFG_BOOT_VOTE_FAILED", 0, "NONE", 68, 0 },
    { "ERR_PARTY_LFG_BOOT_IN_COMBAT", 2, "NONE", 68, 0 },
    { "ERR_PARTY_LFG_BOOT_DUNGEON_COMPLETE", 2, "NONE", 68, 0 },
    { "ERR_PARTY_LFG_BOOT_LOOT_ROLLS", 2, "NONE", 68, 0 },
    { "ERR_PARTY_LFG_TELEPORT_IN_COMBAT", 2, "NONE", 68, 0 },
    { "ERR_RAID_DISALLOWED_BY_LEVEL", 2, "NONE", 68, 0 },
    { "ERR_BATTLEGROUND_INFO_THROTTLED", 0, "NONE", 68, 0 },
    { "ERR_BATTLEGROUND_ALREADY_IN", 2, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_CHANGE_FAILED_QUEUED", 2, "NONE", 68, 0 },
    { "ERR_NOT_WHILE_FALLING", 2, "NONE", 68, 0 },
    { "ERR_NOT_WHILE_FATIGUED", 2, "NONE", 68, 0 },
    { "ERR_MAX_SOCKETS", 2, "NONE", 68, 0 },
    { "ERR_MULTI_CAST_ACTION_TOTEM_S", 2, "NONE", 68, 0 },
    { "ERR_REMOVE_FROM_PVP_QUEUE_GRANT_LEVEL", 2, "NONE", 68, 0 },
    { "ERR_REMOVE_FROM_PVP_QUEUE_XP_GAIN", 2, "NONE", 68, 0 },
    { "ERR_BATTLEGROUND_JOIN_XP_GAIN", 2, "NONE", 68, 0 },
    { "ERR_RAID_DIFFICULTY_FAILED", 0, "NONE", 68, 0 },
    { "ERR_RAID_DIFFICULTY_CHANGED_S", 0, "NONE", 68, 0 },
    { "ERR_RAID_LOCKOUT_CHANGED_S", 0, "NONE", 68, 0 },
    { "ERR_PLAYER_DIFFICULTY_CHANGED_S", 0, "NONE", 68, 0 },
    { "ERR_GMRESPONSE_DB_ERROR", 2, "NONE", 68, 0 },
    { "ERR_BATTLEGROUND_JOIN_RANGE_INDEX", 2, "NONE", 68, 0 },
    { "ERR_REMOVE_FROM_PVP_QUEUE_FACTION_CHANGE", 2, "NONE", 68, 0 },
    { "ERR_BATTLEGROUND_JOIN_FAILED", 2, "NONE", 68, 0 },
    { "ERR_BATTLEGROUND_JOIN_TIMED_OUT", 2, "NONE", 68, 0 },
    { "ERR_MAIL_LIMITED_DURATION_ITEM", 2, "NONE", 68, 0 },
    { "ERR_YELL_RESTRICTED", 0, "NONE", 68, 0 },
    { "ERR_LFG_ROLE_CHECK_FAILED", 0, "NONE", 68, 0 },
    { "ERR_LFG_ROLE_CHECK_FAILED_TIMEOUT", 0, "NONE", 68, 0 },
    { "ERR_LFG_ROLE_CHECK_FAILED_NOT_VIABLE", 0, "NONE", 68, 0 },
    { "ERR_LFG_GROUP_FULL", 0, "NONE", 68, 0 },
    { "ERR_LFG_NO_LFG_OBJECT", 0, "NONE", 68, 0 },
    { "ERR_LFG_NO_SLOTS_PLAYER", 0, "NONE", 68, 0 },
    { "ERR_LFG_NO_SLOTS_PARTY", 0, "NONE", 68, 0 },
    { "ERR_LFG_MISMATCHED_SLOTS", 0, "NONE", 68, 0 },
    { "ERR_LFG_PARTY_PLAYERS_FROM_DIFFERENT_REALMS", 0, "NONE", 68, 0 },
    { "ERR_LFG_MEMBERS_NOT_PRESENT", 0, "NONE", 68, 0 },
    { "ERR_LFG_GET_INFO_TIMEOUT", 0, "NONE", 68, 0 },
    { "ERR_LFG_INVALID_SLOT", 0, "NONE", 68, 0 },
    { "ERR_LFG_DESERTER_PLAYER", 0, "NONE", 68, 0 },
    { "ERR_LFG_DESERTER_PARTY", 0, "NONE", 68, 0 },
    { "ERR_LFG_RANDOM_COOLDOWN_PLAYER", 0, "NONE", 68, 0 },
    { "ERR_LFG_RANDOM_COOLDOWN_PARTY", 0, "NONE", 68, 0 },
    { "ERR_LFG_TOO_MANY_MEMBERS", 0, "NONE", 68, 0 },
    { "ERR_LFG_PROPOSAL_FAILED", 0, "NONE", 68, 0 },
    { "ERR_LFG_PROPOSAL_DECLINED_SELF", 0, "NONE", 68, 0 },
    { "ERR_LFG_PROPOSAL_DECLINED_PARTY", 0, "NONE", 68, 0 },
    { "ERR_LFG_NO_SLOTS_SELECTED", 2, "NONE", 68, 0 },
    { "ERR_LFG_NO_ROLES_SELECTED", 2, "NONE", 68, 0 },
    { "ERR_LFG_ROLE_CHECK_INITIATED", 0, "NONE", 68, 0 },
    { "ERR_LFG_PLAYER_DECLINED_ROLE_CHECK", 0, "NONE", 68, 0 },
    { "ERR_LFG_JOINED_QUEUE", 0, "NONE", 68, 0 },
    { "ERR_LFG_JOINED_LIST", 0, "NONE", 68, 0 },
    { "ERR_LFG_LEFT_QUEUE", 0, "NONE", 68, 0 },
    { "ERR_LFG_LEFT_LIST", 0, "NONE", 68, 0 },
    { "ERR_LFG_ROLE_CHECK_ABORTED", 0, "NONE", 68, 0 },
    { "ERR_LFG_CANT_USE_BATTLEGROUND", 2, "NONE", 68, 0 },
    { "ERR_LFG_CANT_USE_DUNGEONS", 2, "NONE", 68, 0 },
    { "ERR_INVALID_TELEPORT_LOCATION", 2, "NONE", 68, 0 },
    { "ERR_CURRENCY_FULL", 2, "NONE", 0, 0 },
    { "ERR_TOO_FAR_TO_INTERACT", 2, "NONE", 68, 0 },
    { "ERR_DIFFICULTY_CHANGE_COOLDOWN_S", 2, "NONE", 68, 0 },
    { "ERR_DIFFICULTY_CHANGE_WORLDSTATE", 2, "NONE", 68, 0 },
    { "ERR_DIFFICULTY_CHANGE_ENCOUNTER", 2, "NONE", 68, 0 },
    { "ERR_DIFFICULTY_CHANGE_COMBAT", 2, "NONE", 68, 0 },
    { "ERR_DIFFICULTY_CHANGE_PLAYER_BUSY", 2, "NONE", 68, 0 },
    { "ERR_DIFFICULTY_CHANGE_ALREADY_STARTED", 2, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_PARTY_SIZE", 2, "NONE", 68, 0 },
    { "ERR_QUEST_FORCE_REMOVED_S", 0, "NONE", 68, 0 },
    { "ERR_IN_RANDOM_BG", 2, "NONE", 68, 0 },
    { "ERR_IN_NON_RANDOM_BG", 2, "NONE", 68, 0 },
    { "ERR_AUCTION_ENOUGH_ITEMS", 2, "NONE", 68, 0 },
    { "ERR_BN_FRIEND_SELF", 2, "NONE", 68, 0 },
    { "ERR_BN_FRIEND_ALREADY", 2, "NONE", 68, 0 },
    { "ERR_BN_FRIEND_BLOCKED", 2, "NONE", 68, 0 },
    { "ERR_BN_FRIEND_REQUEST_SENT", 1, "NONE", 68, 0 },
    { "ERR_BN_BROADCAST_THROTTLE", 1, "NONE", 68, 0 },
};

// STATIC: 0x00AC80A0
int32_t CGGameUI::m_cursorEquipmentSet = -1;

// STATIC: 0x00AC80A4
int32_t CGGameUI::unk_AC80A4;

// STATIC: 0x00AC80A8
int32_t CGGameUI::m_visible = 1;

// VERIFY: 00d6780a manual
// OFFSET: 0x00510E20
static int32_t Script_GetItemQualityColor(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetItemQualityColor(index)");
        return 0;
    }

    uint32_t quality = static_cast<int32_t>(lua_tonumber(L, 1));
    const CImVector* color = CGTooltip::GetItemQualityColor(quality);
    lua_pushnumber(L, color->r * 0.00392156862745098);
    lua_pushnumber(L, color->g * 0.00392156862745098);
    lua_pushnumber(L, color->b * 0.00392156862745098);
    lua_pushstring(L, CGTooltip::GetItemQualityColorString(quality));
    return 4;
}

// VERIFY: fe358aee manual
// OFFSET: 0x00511B30
static int32_t Script_IsAddOnLoaded(lua_State* L) {
    const char* name;

    if (lua_isnumber(L, 1)) {
        name = GetAddOnByIndex(static_cast<int32_t>(lua_tonumber(L, 1)) - 1);

        if (!name) {
            luaL_error(L, "AddOn index must be in the range of 1 to %d", GetNumAddOns());
        }
    } else {
        if (!lua_isstring(L, 1)) {
            luaL_error(L, "Usage: IsAddOnLoaded(index or \"name\")");
        }

        name = lua_tolstring(L, 1, nullptr);
    }

    if (IsAddOnLoading(name)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (IsAddOnLoaded(name)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 2;
}

// VERIFY: b380dd1d manual
// OFFSET: 0x00515A50
static int32_t Script_GetCursorMoney(lua_State* L) {
    lua_pushnumber(L, CGGameUI::m_cursorMoney);
    return 1;
}

// VERIFY: 1c08ff80 manual
// OFFSET: 0x00510040
static int32_t Script_GetCVar(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetCVar(\"cvar\")");
        return 0;
    }

    CVar* var = CVar::LookupRegistered(lua_tolstring(L, 1, nullptr));

    if (!var || (var->m_flags & 0x40)) {
        lua_pushnil(L);
        return 1;
    }

    lua_pushstring(L, var->m_stringValue.m_str);
    return 1;
}

// VERIFY: 968466d6 manual
// OFFSET: 0x005100C0
static int32_t Script_GetCVarBool(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetCVarBool(\"cvar\")");
        return 0;
    }

    CVar* var = CVar::LookupRegistered(lua_tolstring(L, 1, nullptr));

    if (var && !(var->m_flags & 0x40) && StringToBOOL(var->m_stringValue.m_str)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: b062b828 manual
// OFFSET: 0x00510150
static int32_t Script_GetCVarDefault(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetCVarDefault(\"cvar\")");
        return 0;
    }

    CVar* var = CVar::LookupRegistered(lua_tolstring(L, 1, nullptr));

    if (!var || (var->m_flags & 0x40)) {
        luaL_error(L, "Couldn't find CVar named '%s'", lua_tolstring(L, 1, nullptr));
        return 0;
    }

    lua_pushstring(L, var->m_defaultValue.m_str);
    return 1;
}

// VERIFY: f62939f8 auto
// OFFSET: 0x00510A10
static int32_t Script_GetCursorPosition(lua_State* L) {
    float x = 0.0f;
    float y = 0.0f;

    if (!CSimpleTop::s_instance) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    NDCToDDC(CSimpleTop::s_instance->m_mousePosition.m_x, CSimpleTop::s_instance->m_mousePosition.m_y, &x, &y);
    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * x));
    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * y));
    return 2;
}

// VERIFY: 19aabffe auto
// OFFSET: 0x00514430
static int32_t Script_IsModifierKeyDown(lua_State* L) {
    for (int32_t key = KEY_LSHIFT; key <= KEY_RALT; key++) {
        if (CGGameUI::IsKeyDown(static_cast<KEY>(key))) {
            lua_pushnumber(L, 1.0);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// VERIFY: 16fc0e95 auto
// OFFSET: 0x00514580
static int32_t Script_IsShiftKeyDown(lua_State* L) {
    if (CGGameUI::IsKeyDown(KEY_LSHIFT) || CGGameUI::IsKeyDown(KEY_RSHIFT)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: e03161c3 auto
// OFFSET: 0x005146D0
static int32_t Script_IsControlKeyDown(lua_State* L) {
    if (CGGameUI::IsKeyDown(KEY_LCONTROL) || CGGameUI::IsKeyDown(KEY_RCONTROL)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 5cf9d269 auto
// OFFSET: 0x00514820
static int32_t Script_IsAltKeyDown(lua_State* L) {
    if (CGGameUI::IsKeyDown(KEY_LALT) || CGGameUI::IsKeyDown(KEY_RALT)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 9094ae9a manual
// OFFSET: 0x00514C10
static int32_t Script_SetCVar(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: SetCVar(\"cvar\", value [, \"scriptCvar\")");
        return 0;
    }

    const char* name = lua_tolstring(L, 1, nullptr);
    CVar* var = CVar::LookupRegistered(name);

    if (!var || (var->m_flags & 0x40)) {
        luaL_error(L, "Couldn't find CVar named '%s'", name);
        return 0;
    }

    if ((var->m_flags & 0x4) || (var->m_flags & 0x100)) {
        luaL_error(L, "\"%s\" is read-only", name);
        return 0;
    }

    if ((var->m_flags & 0x8) && !CSimpleTop::GetInstance()->m_allowProtectedActions) {
        CGGameUI::ShowBlockedActionFeedback(nullptr, 2);
        return 0;
    }

    const char* value = lua_tolstring(L, 2, nullptr);

    if (!value) {
        value = "0";
    }

    var->Set(value, true, false, false, true);

    if (lua_isstring(L, 3)) {
        FrameScript_SignalEvent(SCRIPT_EVENT_CVAR_UPDATE, "%s%s", lua_tolstring(L, 3, nullptr), value);
    }

    return 0;
}

// VERIFY: c3cc832c manual
// OFFSET: 0x00514D40
static int32_t Script_GetCVarMin(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetCVarMin(\"cvar\")");
        return 0;
    }

    CVar* var = CVar::LookupRegistered(lua_tolstring(L, 1, nullptr));

    if (!var || (var->m_flags & 0x40)) {
        luaL_error(L, "Couldn't find CVar named '%s'", lua_tolstring(L, 1, nullptr));
        return 0;
    }

    const char* name = var->m_key.m_str;

    if (!SStrCmpI(name, "extShadowQuality", STORM_MAX_STR)) {
        lua_pushnumber(L, 0.0);
    } else if (!SStrCmpI(name, "farclip", STORM_MAX_STR)) {
        lua_pushnumber(L, 177.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 2b500bc1 manual
// OFFSET: 0x00514E30
static int32_t Script_GetCVarMax(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetCVarMax(\"cvar\")");
        return 0;
    }

    CVar* var = CVar::LookupRegistered(lua_tolstring(L, 1, nullptr));

    if (!var || (var->m_flags & 0x40)) {
        luaL_error(L, "Couldn't find CVar named '%s'", lua_tolstring(L, 1, nullptr));
        return 0;
    }

    const char* name = var->m_key.m_str;

    if (!SStrCmpI(name, "extShadowQuality", STORM_MAX_STR)) {
        uint32_t mode = 0;
        uint32_t maxMode = CShadowCache::GetMaxShadowMode();

        do {
            if (!CShadowCache::ValidateShadowMode(static_cast<EShadowMode>(mode))) {
                break;
            }

            mode++;
        } while (mode <= maxMode);

        lua_pushnumber(L, mode - 1);
    } else if (!SStrCmpI(name, "farclip", STORM_MAX_STR)) {
        lua_pushnumber(L, 1277.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: fb7bea1c auto
// OFFSET: 0x0051AEF0
static int32_t Script_GetScreenWidth(lua_State* L) {
    float width = NDCToDDCWidth(1.0f);
    float scale = CGGameUI::m_UISimpleParent ? CGGameUI::m_UISimpleParent->m_layoutScale : 1.0f;
    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * (width / scale)));
    return 1;
}

// VERIFY: 53f1b9fb auto
// OFFSET: 0x0051AF50
static int32_t Script_GetScreenHeight(lua_State* L) {
    float height = NDCToDDCHeight(1.0f);
    float scale = CGGameUI::m_UISimpleParent ? CGGameUI::m_UISimpleParent->m_layoutScale : 1.0f;
    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * (height / scale)));
    return 1;
}

// VERIFY: 2ffb3aeb manual
// OFFSET: 0x00528920
static int32_t Script_LoadAddOn(lua_State* L) {
    const char* name;

    if (lua_isnumber(L, 1)) {
        name = GetAddOnByIndex(static_cast<int32_t>(lua_tonumber(L, 1)) - 1);

        if (!name) {
            luaL_error(L, "AddOn index must be in the range of 1 to %d", GetNumAddOns());
            return 0;
        }
    } else {
        if (!lua_isstring(L, 1)) {
            luaL_error(L, "Usage: LoadAddOn(index or \"name\")");
            return 0;
        }

        name = lua_tolstring(L, 1, nullptr);
    }

    if (!s_loadAddOnDepth) {
        s_loadAddOnStatus = new (__FILE__, __LINE__) CWOWClientStatus("Logs\\FrameXML.log", 1);
    }

    s_loadAddOnDepth++;
    bool loaded = LoadAddOn(name, true, s_loadAddOnStatus);

    if (--s_loadAddOnDepth == 0) {
        if (s_loadAddOnStatus) {
            delete s_loadAddOnStatus;
        }

        s_loadAddOnStatus = nullptr;
    }

    if (loaded) {
        lua_pushnumber(L, 1.0);
        lua_pushnil(L);
        return 2;
    }

    lua_pushnil(L);

    UIADDON_LOADABLE_REASON reason;
    UIADDON_LOADABLE_REASON dependencyReason;

    if (!IsAddOnLoadable(name, true, reason, dependencyReason, ClientServices::GetCharacterName()) && (reason || dependencyReason)) {
        char buffer[128];
        lua_pushstring(L, AddOnUnloadableToken(reason, dependencyReason, buffer, sizeof(buffer)));
        return 2;
    }

    lua_pushstring(L, "UNKNOWN_ERROR");
    return 2;
}

// STATIC: 0x00AC80B0
static FrameScript_Method s_ScriptFunctions[] = {
    { "FrameXML_Debug", &Script_FrameXML_Debug },
    { "GetBuildInfo", &Script_GetBuildInfo },
    { "ReloadUI", &Script_ReloadUI },
    { "RegisterForSave", &Script_RegisterForSave },
    { "RegisterForSavePerCharacter", &Script_RegisterForSavePerCharacter },
    { "SetLayoutMode", &Script_SetLayoutMode },
    { "IsModifierKeyDown", &Script_IsModifierKeyDown },
    { "IsLeftShiftKeyDown", &Script_IsLeftShiftKeyDown },
    { "IsRightShiftKeyDown", &Script_IsRightShiftKeyDown },
    { "IsShiftKeyDown", &Script_IsShiftKeyDown },
    { "IsLeftControlKeyDown", &Script_IsLeftControlKeyDown },
    { "IsRightControlKeyDown", &Script_IsRightControlKeyDown },
    { "IsControlKeyDown", &Script_IsControlKeyDown },
    { "IsLeftAltKeyDown", &Script_IsLeftAltKeyDown },
    { "IsRightAltKeyDown", &Script_IsRightAltKeyDown },
    { "IsAltKeyDown", &Script_IsAltKeyDown },
    { "IsMouseButtonDown", &Script_IsMouseButtonDown },
    { "GetMouseButtonName", &Script_GetMouseButtonName },
    { "GetMouseButtonClicked", &Script_GetMouseButtonClicked },
    { "SetConsoleKey", &Script_SetConsoleKey },
    { "Screenshot", &Script_Screenshot },
    { "GetFramerate", &Script_GetFramerate },
    { "TogglePerformanceDisplay", &Script_TogglePerformanceDisplay },
    { "TogglePerformancePause", &Script_TogglePerformancePause },
    { "TogglePerformanceValues", &Script_TogglePerformanceValues },
    { "ResetPerformanceValues", &Script_ResetPerformanceValues },
    { "GetDebugStats", &Script_GetDebugStats },
    { "IsDebugBuild", &Script_IsDebugBuild },
    { "RegisterCVar", &Script_RegisterCVar },
    { "GetCVarInfo", &Script_GetCVarInfo },
    { "SetCVar", &Script_SetCVar },
    { "GetCVar", &Script_GetCVar },
    { "GetCVarBool", &Script_GetCVarBool },
    { "GetCVarDefault", &Script_GetCVarDefault },
    { "GetCVarMin", &Script_GetCVarMin },
    { "GetCVarMax", &Script_GetCVarMax },
    { "GetCVarAbsoluteMin", &Script_GetCVarAbsoluteMin },
    { "GetCVarAbsoluteMax", &Script_GetCVarAbsoluteMax },
    { "GetWaterDetail", &Script_GetWaterDetail },
    { "SetWaterDetail", &Script_SetWaterDetail },
    { "GetFarclip", &Script_GetFarclip },
    { "SetFarclip", &Script_SetFarclip },
    { "GetTexLodBias", &Script_GetTexLodBias },
    { "SetTexLodBias", &Script_SetTexLodBias },
    { "SetBaseMip", &Script_SetBaseMip },
    { "GetBaseMip", &Script_GetBaseMip },
    { "ToggleTris", &Script_ToggleTris },
    { "TogglePortals", &Script_TogglePortals },
    { "ToggleCollision", &Script_ToggleCollision },
    { "ToggleCollisionDisplay", &Script_ToggleCollisionDisplay },
    { "TogglePlayerBounds", &Script_TogglePlayerBounds },
    { "Stuck", &Script_Stuck },
    { "Logout", &Script_Logout },
    { "Quit", &Script_Quit },
    { "SetCursor", &Script_SetCursor },
    { "ResetCursor", &Script_ResetCursor },
    { "ClearCursor", &Script_ClearCursor },
    { "CursorHasItem", &Script_CursorHasItem },
    { "CursorHasSpell", &Script_CursorHasSpell },
    { "CursorHasMacro", &Script_CursorHasMacro },
    { "CursorHasMoney", &Script_CursorHasMoney },
    { "GetCursorInfo", &Script_GetCursorInfo },
    { "EquipCursorItem", &Script_EquipCursorItem },
    { "DeleteCursorItem", &Script_DeleteCursorItem },
    { "EquipPendingItem", &Script_EquipPendingItem },
    { "CancelPendingEquip", &Script_CancelPendingEquip },
    { "TargetUnit", &Script_TargetUnit },
    { "TargetNearest", &Script_TargetNearest },
    { "TargetNearestEnemy", &Script_TargetNearestEnemy },
    { "TargetNearestEnemyPlayer", &Script_TargetNearestEnemyPlayer },
    { "TargetNearestFriend", &Script_TargetNearestFriend },
    { "TargetNearestFriendPlayer", &Script_TargetNearestFriendPlayer },
    { "TargetNearestPartyMember", &Script_TargetNearestPartyMember },
    { "TargetNearestRaidMember", &Script_TargetNearestRaidMember },
    { "TargetDirectionEnemy", &Script_TargetDirectionEnemy },
    { "TargetDirectionFriend", &Script_TargetDirectionFriend },
    { "TargetDirectionFinished", &Script_TargetDirectionFinished },
    { "TargetLastTarget", &Script_TargetLastTarget },
    { "TargetLastEnemy", &Script_TargetLastEnemy },
    { "TargetLastFriend", &Script_TargetLastFriend },
    { "AttackTarget", &Script_AttackTarget },
    { "AssistUnit", &Script_AssistUnit },
    { "FocusUnit", &Script_FocusUnit },
    { "FollowUnit", &Script_FollowUnit },
    { "InteractUnit", &Script_InteractUnit },
    { "ClearTarget", &Script_ClearTarget },
    { "ClearFocus", &Script_ClearFocus },
    { "AutoEquipCursorItem", &Script_AutoEquipCursorItem },
    { "ToggleSheath", &Script_ToggleSheath },
    { "GetZoneText", &Script_GetZoneText },
    { "GetRealZoneText", &Script_GetRealZoneText },
    { "GetSubZoneText", &Script_GetSubZoneText },
    { "GetMinimapZoneText", &Script_GetMinimapZoneText },
    { "InitiateTrade", &Script_InitiateTrade },
    { "CanInspect", &Script_CanInspect },
    { "NotifyInspect", &Script_NotifyInspect },
    { "InviteUnit", &Script_InviteUnit },
    { "UninviteUnit", &Script_UninviteUnit },
    { "RequestTimePlayed", &Script_RequestTimePlayed },
    { "RepopMe", &Script_RepopMe },
    { "AcceptResurrect", &Script_AcceptResurrect },
    { "DeclineResurrect", &Script_DeclineResurrect },
    { "ResurrectGetOfferer", &Script_ResurrectGetOfferer },
    { "ResurrectHasSickness", &Script_ResurrectHasSickness },
    { "ResurrectHasTimer", &Script_ResurrectHasTimer },
    { "BeginTrade", &Script_BeginTrade },
    { "CancelTrade", &Script_CancelTrade },
    { "AcceptGroup", &Script_AcceptGroup },
    { "DeclineGroup", &Script_DeclineGroup },
    { "AcceptGuild", &Script_AcceptGuild },
    { "DeclineGuild", &Script_DeclineGuild },
    { "AcceptArenaTeam", &Script_AcceptArenaTeam },
    { "DeclineArenaTeam", &Script_DeclineArenaTeam },
    { "CancelLogout", &Script_CancelLogout },
    { "ForceLogout", &Script_ForceLogout },
    { "ForceQuit", &Script_ForceQuit },
    { "GetCursorMoney", &Script_GetCursorMoney },
    { "DropCursorMoney", &Script_DropCursorMoney },
    { "PickupPlayerMoney", &Script_PickupPlayerMoney },
    { "HasSoulstone", &Script_HasSoulstone },
    { "UseSoulstone", &Script_UseSoulstone },
    { "HasKey", &Script_HasKey },
    { "GuildInvite", &Script_GuildInvite },
    { "GuildUninvite", &Script_GuildUninvite },
    { "GuildPromote", &Script_GuildPromote },
    { "GuildDemote", &Script_GuildDemote },
    { "GuildSetLeader", &Script_GuildSetLeader },
    { "GuildSetMOTD", &Script_GuildSetMOTD },
    { "GuildLeave", &Script_GuildLeave },
    { "GuildDisband", &Script_GuildDisband },
    { "GuildInfo", &Script_GuildInfo },
    { "ArenaTeamInviteByName", &Script_ArenaTeamInviteByName },
    { "ArenaTeamLeave", &Script_ArenaTeamLeave },
    { "ArenaTeamUninviteByName", &Script_ArenaTeamUninviteByName },
    { "ArenaTeamSetLeaderByName", &Script_ArenaTeamSetLeaderByName },
    { "ArenaTeamDisband", &Script_ArenaTeamDisband },
    { "GetScreenWidth", &Script_GetScreenWidth },
    { "GetScreenHeight", &Script_GetScreenHeight },
    { "GetDamageBonusStat", &Script_GetDamageBonusStat },
    { "GetReleaseTimeRemaining", &Script_GetReleaseTimeRemaining },
    { "GetCorpseRecoveryDelay", &Script_GetCorpseRecoveryDelay },
    { "GetInstanceBootTimeRemaining", &Script_GetInstanceBootTimeRemaining },
    { "GetInstanceLockTimeRemaining", &Script_GetInstanceLockTimeRemaining },
    { "GetInstanceLockTimeRemainingEncounter", &Script_GetInstanceLockTimeRemainingEncounter },
    { "GetSummonConfirmTimeLeft", &Script_GetSummonConfirmTimeLeft },
    { "GetSummonConfirmSummoner", &Script_GetSummonConfirmSummoner },
    { "GetSummonConfirmAreaName", &Script_GetSummonConfirmAreaName },
    { "ConfirmSummon", &Script_ConfirmSummon },
    { "CancelSummon", &Script_CancelSummon },
    { "GetCursorPosition", &Script_GetCursorPosition },
    { "GetNetStats", &Script_GetNetStats },
    { "SitStandOrDescendStart", &Script_SitStandOrDescendStart },
    { "StopCinematic", &Script_StopCinematic },
    { "RunScript", &Script_RunScript },
    { "CheckInteractDistance", &Script_CheckInteractDistance },
    { "RandomRoll", &Script_RandomRoll },
    { "OpeningCinematic", &Script_OpeningCinematic },
    { "InCinematic", &Script_InCinematic },
    { "IsWindowsClient", &Script_IsWindowsClient },
    { "IsMacClient", &Script_IsMacClient },
    { "IsLinuxClient", &Script_IsLinuxClient },
    { "AcceptXPLoss", &Script_AcceptXPLoss },
    { "CheckSpiritHealerDist", &Script_CheckSpiritHealerDist },
    { "CheckTalentMasterDist", &Script_CheckTalentMasterDist },
    { "CheckBinderDist", &Script_CheckBinderDist },
    { "RetrieveCorpse", &Script_RetrieveCorpse },
    { "BindEnchant", &Script_BindEnchant },
    { "ReplaceEnchant", &Script_ReplaceEnchant },
    { "ReplaceTradeEnchant", &Script_ReplaceTradeEnchant },
    { "NotWhileDeadError", &Script_NotWhileDeadError },
    { "GetRestState", &Script_GetRestState },
    { "GetXPExhaustion", &Script_GetXPExhaustion },
    { "GetTimeToWellRested", &Script_GetTimeToWellRested },
    { "GMRequestPlayerInfo", &Script_GMRequestPlayerInfo },
    { "GetCoinIcon", &Script_GetCoinIcon },
    { "GetCoinText", &Script_GetCoinText },
    { "GetCoinTextureString", &Script_GetCoinTextureString },
    { "IsSubZonePVPPOI", &Script_IsSubZonePVPPOI },
    { "GetZonePVPInfo", &Script_GetZonePVPInfo },
    { "TogglePVP", &Script_TogglePVP },
    { "SetPVP", &Script_SetPVP },
    { "GetPVPDesired", &Script_GetPVPDesired },
    { "GetPVPTimer", &Script_GetPVPTimer },
    { "IsPVPTimerRunning", &Script_IsPVPTimerRunning },
    { "ConfirmBindOnUse", &Script_ConfirmBindOnUse },
    { "SetPortraitToTexture", &Script_SetPortraitToTexture },
    { "GetLocale", &Script_GetLocale },
    { "GetGMTicketCategories", &Script_GetGMTicketCategories },
    { "DropItemOnUnit", &Script_DropItemOnUnit },
    { "RestartGx", &Script_RestartGx },
    { "RestoreVideoResolutionDefaults", &Script_RestoreVideoResolutionDefaults },
    { "RestoreVideoEffectsDefaults", &Script_RestoreVideoEffectsDefaults },
    { "RestoreVideoStereoDefaults", &Script_RestoreVideoStereoDefaults },
    { "GetBindLocation", &Script_GetBindLocation },
    { "ConfirmTalentWipe", &Script_ConfirmTalentWipe },
    { "ConfirmBinder", &Script_ConfirmBinder },
    { "ShowingHelm", &Script_ShowingHelm },
    { "ShowingCloak", &Script_ShowingCloak },
    { "ShowHelm", &Script_ShowHelm },
    { "ShowCloak", &Script_ShowCloak },
    { "SetEuropeanNumbers", &Script_SetEuropeanNumbers },
    { "GetAreaSpiritHealerTime", &Script_GetAreaSpiritHealerTime },
    { "AcceptAreaSpiritHeal", &Script_AcceptAreaSpiritHeal },
    { "CancelAreaSpiritHeal", &Script_CancelAreaSpiritHeal },
    { "GetMouseFocus", &Script_GetMouseFocus },
    { "GetRealmName", &Script_GetRealmName },
    { "GetItemQualityColor", &Script_GetItemQualityColor },
    { "GetItemInfo", &Script_GetItemInfo },
    { "GetItemGem", &Script_GetItemGem },
    { "GetExtendedItemInfo", &Script_GetExtendedItemInfo },
    { "GetItemIcon", &Script_GetItemIcon },
    { "GetItemFamily", &Script_GetItemFamily },
    { "GetItemCount", &Script_GetItemCount },
    { "GetItemSpell", &Script_GetItemSpell },
    { "GetItemCooldown", &Script_GetItemCooldown },
    { "PickupItem", &Script_PickupItem },
    { "IsCurrentItem", &Script_IsCurrentItem },
    { "IsUsableItem", &Script_IsUsableItem },
    { "IsHelpfulItem", &Script_IsHelpfulItem },
    { "IsHarmfulItem", &Script_IsHarmfulItem },
    { "IsConsumableItem", &Script_IsConsumableItem },
    { "IsEquippableItem", &Script_IsEquippableItem },
    { "IsEquippedItem", &Script_IsEquippedItem },
    { "IsEquippedItemType", &Script_IsEquippedItemType },
    { "IsDressableItem", &Script_IsDressableItem },
    { "ItemHasRange", &Script_ItemHasRange },
    { "IsItemInRange", &Script_IsItemInRange },
    { "GetNumAddOns", &Script_GetNumAddOns },
    { "GetAddOnInfo", &Script_GetAddOnInfo },
    { "GetAddOnMetadata", &Script_GetAddOnMetadata },
    { "UpdateAddOnMemoryUsage", &Script_UpdateAddOnMemoryUsage },
    { "GetAddOnMemoryUsage", &Script_GetAddOnMemoryUsage },
    { "GetScriptCPUUsage", &Script_GetScriptCPUUsage },
    { "UpdateAddOnCPUUsage", &Script_UpdateAddOnCPUUsage },
    { "GetAddOnCPUUsage", &Script_GetAddOnCPUUsage },
    { "GetFunctionCPUUsage", &Script_GetFunctionCPUUsage },
    { "GetFrameCPUUsage", &Script_GetFrameCPUUsage },
    { "GetEventCPUUsage", &Script_GetEventCPUUsage },
    { "ResetCPUUsage", &Script_ResetCPUUsage },
    { "GetAddOnDependencies", &Script_GetAddOnDependencies },
    { "EnableAddOn", &Script_EnableAddOn },
    { "EnableAllAddOns", &Script_EnableAllAddOns },
    { "DisableAddOn", &Script_DisableAddOn },
    { "DisableAllAddOns", &Script_DisableAllAddOns },
    { "ResetDisabledAddOns", &Script_ResetDisabledAddOns },
    { "IsAddOnLoadOnDemand", &Script_IsAddOnLoadOnDemand },
    { "IsAddOnLoaded", &Script_IsAddOnLoaded },
    { "LoadAddOn", &Script_LoadAddOn },
    { "PartialPlayTime", &Script_PartialPlayTime },
    { "NoPlayTime", &Script_NoPlayTime },
    { "GetBillingTimeRested", &Script_GetBillingTimeRested },
    { "CanShowResetInstances", &Script_CanShowResetInstances },
    { "ResetInstances", &Script_ResetInstances },
    { "IsInInstance", &Script_IsInInstance },
    { "GetInstanceDifficulty", &Script_GetInstanceDifficulty },
    { "GetInstanceInfo", &Script_GetInstanceInfo },
    { "GetDungeonDifficulty", &Script_GetDungeonDifficulty },
    { "SetDungeonDifficulty", &Script_SetDungeonDifficulty },
    { "GetRaidDifficulty", &Script_GetRaidDifficulty },
    { "SetRaidDifficulty", &Script_SetRaidDifficulty },
    { "ReportBug", &Script_ReportBug },
    { "ReportSuggestion", &Script_ReportSuggestion },
    { "GetMirrorTimerInfo", &Script_GetMirrorTimerInfo },
    { "GetMirrorTimerProgress", &Script_GetMirrorTimerProgress },
    { "GetNumTitles", &Script_GetNumTitles },
    { "GetCurrentTitle", &Script_GetCurrentTitle },
    { "SetCurrentTitle", &Script_SetCurrentTitle },
    { "IsTitleKnown", &Script_IsTitleKnown },
    { "GetTitleName", &Script_GetTitleName },
    { "UseItemByName", &Script_UseItemByName },
    { "EquipItemByName", &Script_EquipItemByName },
    { "GetExistingLocales", &Script_GetExistingLocales },
    { "InCombatLockdown", &Script_InCombatLockdown },
    { "StartAttack", &Script_StartAttack },
    { "StopAttack", &Script_StopAttack },
    { "SetTaxiBenchmarkMode", &Script_SetTaxiBenchmarkMode },
    { "GetTaxiBenchmarkMode", &Script_GetTaxiBenchmarkMode },
    { "Dismount", &Script_Dismount },
    { "VoicePushToTalkStart", &Script_VoicePushToTalkStart },
    { "VoicePushToTalkStop", &Script_VoicePushToTalkStop },
    { "SetUIVisibility", &Script_SetUIVisibility },
    { "IsReferAFriendLinked", &Script_IsReferAFriendLinked },
    { "CanGrantLevel", &Script_CanGrantLevel },
    { "GrantLevel", &Script_GrantLevel },
    { "CanSummonFriend", &Script_CanSummonFriend },
    { "SummonFriend", &Script_SummonFriend },
    { "GetSummonFriendCooldown", &Script_GetSummonFriendCooldown },
    { "GetTotemInfo", &Script_GetTotemInfo },
    { "GetTotemTimeLeft", &Script_GetTotemTimeLeft },
    { "TargetTotem", &Script_TargetTotem },
    { "DestroyTotem", &Script_DestroyTotem },
    { "GetNumDeclensionSets", &Script_GetNumDeclensionSets },
    { "DeclineName", &Script_DeclineName },
    { "AcceptLevelGrant", &Script_AcceptLevelGrant },
    { "DeclineLevelGrant", &Script_DeclineLevelGrant },
    { "UploadSettings", &Script_UploadSettings },
    { "DownloadSettings", &Script_DownloadSettings },
    { "GetMovieResolution", &Script_GetMovieResolution },
    { "GameMovieFinished", &Script_GameMovieFinished },
    { "IsDesaturateSupported", &Script_IsDesaturateSupported },
    { "GetThreatStatusColor", &Script_GetThreatStatusColor },
    { "IsThreatWarningEnabled", &Script_IsThreatWarningEnabled },
    { "ConsoleAddMessage", &Script_ConsoleAddMessage },
    { "GetItemUniqueness", &Script_GetItemUniqueness },
    { "EndRefund", &Script_EndRefund },
    { "EndBoundTradeable", &Script_EndBoundTradeable },
    { "CanMapChangeDifficulty", &Script_CanMapChangeDifficulty },
    { "GetExpansionLevel", &Script_GetAccountExpansionLevel },
    { "GetAllowLowLevelRaid", &Script_GetAllowLowLevelRaid },
    { "SetAllowLowLevelRaid", &Script_SetAllowLowLevelRaid },
};

// STATIC: 0x00AC8AE0
CCapturePointUIManager g_capturePointUIManager;

// STATIC: 0x00BCFB8C
int32_t CGGameUI::m_hasControl;

// STATIC: 0x00BCFB90
char CGGameUI::m_lastErrorMessage[3000];

// STATIC: 0x00BD0748
int32_t CGGameUI::m_cursorItemType;

// STATIC: 0x00BD074C
int32_t CGGameUI::m_cursorHasAction;

// STATIC: 0x00BD0750
int32_t CGGameUI::m_cursorVirtualSlot;

// STATIC: 0x00BD0754
int32_t CGGameUI::m_cursorVirtualDisplay;

// STATIC: 0x00BD0758
int32_t CGGameUI::m_cursorVirtualID;

// STATIC: 0x00BD075C
uint32_t CGGameUI::m_cursorItemSlot;

// STATIC: 0x00BD0760
WOWGUID CGGameUI::m_cursorItemContainer;

// STATIC: 0x00BD0768
WOWGUID CGGameUI::m_cursorItem;

// STATIC: 0x00BD0774
CGTooltip* CGGameUI::m_gameTooltip;

// STATIC: 0x00BD0778
CSimpleFrame* CGGameUI::m_UISimpleParent;

// STATIC: 0x00BD078C
CSimpleTop* CGGameUI::m_simpleTop;

// STATIC: 0x00BD0790
uint8_t CGGameUI::m_currentlyReloadingUI;

// STATIC: 0x00BD0791
uint8_t CGGameUI::m_reloadUIRequested;

// STATIC: 0x00BD0793
uint8_t CGGameUI::m_loggingIn;

// STATIC: 0x00BD079C
uint32_t CGGameUI::m_cursorMoney;

// STATIC: 0x00BD07A0
WOWGUID CGGameUI::m_currentObjectTrack;

// STATIC: 0x00BD07A8
WOWGUID CGGameUI::m_interactTarget;

// STATIC: 0x00BD07B0
WOWGUID CGGameUI::m_lockedTarget;

// STATIC: 0x00BD07B8
WOWGUID CGGameUI::m_lastTarget;

// STATIC: 0x00BD07C0
WOWGUID CGGameUI::m_lastEnemyTarget;

// STATIC: 0x00BD07C8
WOWGUID CGGameUI::m_lastFriendTarget;

// STATIC: 0x00BD07D0
WOWGUID CGGameUI::unk_BD07D0;

// STATIC: 0x00BD07E0
int32_t CGGameUI::m_cursorSpell;

// STATIC: 0x00BD07E4
int32_t CGGameUI::m_cursorPetAction;

// STATIC: 0x00BD07E8
int32_t CGGameUI::m_cursorMacro;

// STATIC: 0x00BD07EC
int32_t CGGameUI::m_cursorPet;

// STATIC: 0x00BD07F0
int32_t CGGameUI::unk_BD07F0;

// STATIC: 0x00BD07F4
int32_t CGGameUI::unk_BD07F4;

// STATIC: 0x00BD07F8
float CGGameUI::m_aspect;

// STATIC: 0x00BD07FC
int32_t CGGameUI::m_inCinematic;

// STATIC: 0x00BD0800
int32_t CGGameUI::m_inMovie;

// STATIC: 0x00BD0828
WOWGUID CGGameUI::m_corpseGUID;

// STATIC: 0x00BD0848
uint32_t CGGameUI::m_repopTime;

// STATIC: 0x00BD084C
uint8_t CGGameUI::m_deadNoRepopTimer;

// STATIC: 0x00BD084D
uint8_t CGGameUI::m_comboPoints;

// STATIC: 0x00BD0850
uint32_t CGGameUI::m_corpseReclaimDelay;

// STATIC: 0x00BD0854
uint32_t CGGameUI::m_instanceBootTime;

// STATIC: 0x00BD0858
uint32_t CGGameUI::m_instanceLockTime;

// STATIC: 0x00BD085C
uint32_t CGGameUI::m_instanceLockCompletedMask;

// STATIC: 0x00BD0860
uint32_t CGGameUI::m_instanceLockExtending;

// STATIC: 0x00BD0864
uint32_t CGGameUI::m_summonConfirmTime;

// STATIC: 0x00BD086C
uint32_t CGGameUI::m_billingExpireTime;

// STATIC: 0x00BD0878
int32_t CGGameUI::unk_BD0878;

// STATIC: 0x00BD087C
int32_t CGGameUI::unk_BD087C;

// STATIC: 0x00BD0880
int32_t CGGameUI::m_initialized;

// STATIC: 0x00BD0884
uint32_t CGGameUI::m_uiLastInstanceTime;

// STATIC: 0x00BD0888
int32_t CGGameUI::m_iMapID;

// STATIC: 0x00BD088C
int32_t CGGameUI::m_iCurrentMapID;

// STATIC: 0x00BD0894
uint32_t CGGameUI::m_instanceDifficulty;

// STATIC: 0x00BD0898
uint32_t CGGameUI::m_dungeonDifficulty;

// STATIC: 0x00BD08A0
uint32_t CGGameUI::m_lastRegenTime;

// STATIC: 0x00BD08A8
WOWGUID CGGameUI::m_comboTarget;

// STATIC: 0x00BD08C0
int32_t CGGameUI::m_threatWarningEnabled;

// STATIC: 0x00BD08C4
uint16_t CGGameUI::unk_BD08C4;

// STATIC: 0x00BD08CC
int32_t CGGameUI::unk_BD08CC;

// STATIC: 0x00BD0B00
TotemInfo CGGameUI::m_totems[4];

// STATIC: 0x00BD0B80
MirrorTimerInfo CGGameUI::m_mirrorTimerInfo[3];

// VERIFY: 2d4b8add manual
// OFFSET: 0x005120E0
void LoadScriptFunctions() {
    RegisterSimpleFrameScriptMethods();
    CGTooltip::CreateScriptMetaTable();
    CGCooldown::CreateScriptMetaTable();
    CGMinimapFrame::CreateScriptMetaTable();
    CGCharacterModelBase::CreateScriptMetaTable();
    CGDressUpModelFrame::CreateScriptMetaTable();
    CGTabardModelFrame::CreateScriptMetaTable();
    CGQuestPOIFrame::CreateScriptMetaTable();

    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }

    UIBindingsRegisterScriptFunctions();
    UIMacrosRegisterScriptFunctions();
    InputControlRegisterScriptFunctions();
    CameraRegisterScriptFunctions();
    SI2::RegisterScriptFunctions();
    SpellRegisterScriptFunctions();
    ScriptEventsRegisterFunctions();
    MinimapRegisterScriptFunctions();
    UnitCombatLogRegisterScriptFunctions();
    ActionBarRegisterScriptFunctions();
    CGPartyInfo::RegisterScriptFunctions();
    ChatRegisterScriptFunctions();
    AutoCompleteRegisterScriptFunctions();
    SpellBookRegisterScriptFunctions();
    CharacterInfoRegisterScriptFunctions();
    LootInfoRegisterScriptFunctions();
    ItemTextRegisterScriptFunctions();
    GossipInfoRegisterScriptFunctions();
    QuestInfoRegisterScriptFunctions();
    QuestLogRegisterScriptFunctions();
    CGTaxiMap::RegisterScriptFunctions();
    ClassTrainerRegisterScriptFunctions();
    TradeSkillRegisterScriptFunctions();
    MerchantRegisterScriptFunctions();
    TradeInfoRegisterScriptFunctions();
    ContainerRegisterScriptFunctions();
    BankRegisterScriptFunctions();
    FriendList::RegisterScriptFunctions();
    PetInfoRegisterScriptFunctions();
    WorldMapRegisterScriptFunctions();
    ReputationInfoRegisterScriptFunctions();
    TabardCreationRegisterScriptFunctions();
    GuildRegistrarRegisterScriptFunctions();
    DuelInfoRegisterScriptFunctions();
    TutorialRegisterScriptFunctions();
    PetitionInfoRegisterScriptFunctions();
    SkillInfoRegisterScriptFunctions();
    GuildInfoRegisterScriptFunctions();
    MailInfoRegisterScriptFunctions();
    BattlefieldInfoRegisterScriptFunctions();
    TalentInfoRegisterScriptFunctions();
    AuctionHouseRegisterScriptFunctions();
    StableInfoRegisterScriptFunctions();
    RaidInfoRegisterScriptFunctions();
    WorldStateInfoRegisterScriptFunctions();
    MinigameInfoRegisterScriptFunctions();
    KnowledgeBase_RegisterScriptFunctions();
    ItemSocketInfoRegisterScriptFunctions();
    PetitionVendorRegisterScriptFunctions();
    ArenaTeamInfoRegisterScriptFunctions();
    LookingForGroupRegisterScriptFunctions();
    VoiceRegisterScriptFunctions();
    CalendarRegisterScriptFunctions();
    CGCommentator::RegisterScriptFunctions();
    CGBarberShop::RegisterScriptFunctions();
    CGDanceStudio::RegisterScriptFunctions();
    GuildBankRegisterScriptFunctions();
    GlyphInfoRegisterScriptFunctions();
    AchievementInfoRegisterScriptFunctions();
    CurrencyTypesRegisterScriptFunctions();
    CGVideoOptions::RegisterScriptFunctions();
    EquipmentManagerRegisterScriptFunctions();
    GMTicketInfoRegisterScriptFunctions();
    BattlenetUI_RegisterScriptFunctions();
}

// VERIFY: a784a12c auto
// OFFSET: 0x005126E0
void CGGameUI::SetAspect() {
    CGGameUI::m_aspect = Screen::GetAspectRatio();
    CoordinateSetAspectRatio(CGGameUI::m_aspect);
}

// VERIFY: 6c643fe3 manual
// OFFSET: 0x00512900
void OnGxRestartClearWorld() {
    if (CGWorldFrame::s_currentWorldFrame) {
        CWorldScene::ClearObjectFadeoutList();
    }

    UpdateAllPortraits();
}

// VERIFY: ff48c834 auto
// OFFSET: 0x00512A60
WOWGUID CGGameUI::GetPartyMember(uint32_t index) {
    return GetPartyMemberGUID(index);
}

// VERIFY: 3634a2e6 auto
// OFFSET: 0x00512A70
WOWGUID CGGameUI::GetPartyMemberPet(uint32_t index) {
    return GetPartyPetGUID(index);
}

// VERIFY: 62eb235c auto
// OFFSET: 0x00512A80
WOWGUID GetRaidMemberGUID(uint32_t index) {
    if (index >= CGRaidInfo::m_numMembers || !CGRaidInfo::m_members[index]) {
        return WOWGUID();
    }

    return CGRaidInfo::m_members[index]->m_guid;
}

// VERIFY: b87793f3 auto
// OFFSET: 0x00512C20
void CGGameUI::SetActiveCorpse(WOWGUID guid) {
    CGGameUI::m_corpseGUID = guid;
}

// VERIFY: 6ad55878 manual
// OFFSET: 0x00518CE0
WOWGUID CGGameUI::GetRaidMemberPet(uint32_t index) {
    if (index >= CGRaidInfo::m_numMembers) {
        return WOWGUID();
    }

    CGRaidMember* member = CGRaidInfo::m_members[index];

    if (!member) {
        return WOWGUID();
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(member->m_guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!unit) {
        return member->m_stats.m_pet.m_guid;
    }

    const WOWGUID& pet = unit->m_unit->m_unitFieldCharm ? unit->m_unit->m_unitFieldCharm : unit->m_unit->m_unitFieldSummon;
    return pet;
}

// VERIFY: 23e6422e auto
// OFFSET: 0x00513530
void CGGameUI::ShowBlockedActionFeedback(FrameScript_Object* object, uint32_t type) {
    // STATIC: 0x00BD0C68
    static int32_t showing;

    if (showing) {
        return;
    }

    showing = 1;

    lua_State* L = FrameScript_GetContext();
    char function[1024];
    FrameScript_GetCurrentFunction(L, function, sizeof(function));
    SStrPack(function, "()", sizeof(function));

    if (type) {
        if (lua_tainted == g_macroTaint) {
            FrameScript_SignalEvent(427, "%s", function);
        } else {
            FrameScript_SignalEvent(428, "%s%s", lua_tainted, function);
        }
    } else if (lua_tainted == g_macroTaint) {
        FrameScript_SignalEvent(425, "%s", function);
    } else {
        FrameScript_SignalEvent(426, "%s%s", lua_tainted, function);
    }

    if (lua_tainteventcallback) {
        if (type == 2) {
            lua_tainteventcallback(L, 4, nullptr, lua_tainted);
        } else {
            lua_tainteventcallback(L, 3, nullptr, lua_tainted);
        }
    }

    showing = 0;
}

// VERIFY: 3c6e7130 auto
// OFFSET: 0x005138D0
uint32_t CGGameUI::GetCurrentDungeonDifficulty() {
    if (CGPartyInfo::m_members[0] && !CGRaidInfo::m_numMembers) {
        return CGPartyInfo::m_dungeonDifficulty;
    }

    return CGGameUI::m_dungeonDifficulty;
}

// VERIFY: 6a43ca34 auto
// OFFSET: 0x005195C0
void CGGameUI::SetLastInstanceTime(uint32_t time, int32_t mapId, int32_t currentMapId) {
    if (mapId == currentMapId) {
        return;
    }

    MapRec* map = g_mapDB.GetRecord(mapId);

    if (map && (map->m_instanceType == 1 || map->m_instanceType == 2)) {
        uint32_t difficulty = CGGameUI::GetCurrentDungeonDifficulty();

        if (difficulty > 3 || !GetMapResetTime(map->m_id, difficulty)) {
            CGGameUI::m_uiLastInstanceTime = time;
            CGGameUI::m_iMapID = mapId;
        }
    }

    CGGameUI::m_iCurrentMapID = currentMapId;
}

// VERIFY: 9da22ed9 auto
// OFFSET: 0x005191C0
int32_t CGGameUI::CanPerformAction(uint32_t action) {
    if (lua_tainted) {
        switch (action) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 16:
            case 17:
                CGGameUI::ShowBlockedActionFeedback(nullptr, 0);
                return 0;

            case 6:
            case 7:
            case 8:
            case 9:
            case 15:
            case 18:
            case 19:
            case 20:
            case 21:
            case 23:
                if (!CGGameUI::m_simpleTop) {
                    return 1;
                }

                if (!CGGameUI::m_simpleTop->unk_1250) {
                    CGGameUI::ShowBlockedActionFeedback(nullptr, 1);
                    return 0;
                }

                break;

            case 11:
            case 12:
            case 13:
            case 14:
            case 22:
                if (!CGGameUI::m_simpleTop) {
                    return 1;
                }

                if (!CGGameUI::m_simpleTop->m_allowProtectedActions) {
                    CGGameUI::ShowBlockedActionFeedback(nullptr, 2);
                    return 0;
                }

                break;

            default:
                break;
        }
    }

    if (CGGameUI::m_simpleTop && !action) {
        CGGameUI::m_simpleTop->unk_1250 = 0;
    }

    return 1;
}

// VERIFY: d5e3b07e manual
// OFFSET: 0x0052A980
void CGGameUI::InitializeFrameXML() {
    CGChat::LeaveWorld();

    g_taintLogCVar = CVar::Register("taintLog", "Whether taint logging is enabled", 0, "0", SetTaintLogCallback, 5, false, nullptr, false);
    g_scriptProfileCVar = CVar::Register("scriptProfile", "Whether or not script profiling is enabled", 0, "0", nullptr, 5, false, nullptr, false);

    CRect windowRect = { 0.0f, 0.0f, 0.0f, 0.0f };

    CGGameUI::m_loggingIn = 1;
    CGGameUI::m_reloadUIRequested = 0;
    CGGameUI::m_repopTime = 0;
    CGGameUI::m_deadNoRepopTimer = 0;
    CGGameUI::m_corpseReclaimDelay = 0;
    CGGameUI::m_instanceBootTime = 0;
    CGGameUI::m_instanceLockTime = 0;
    CGGameUI::m_instanceLockCompletedMask = 0;
    CGGameUI::m_instanceLockExtending = 0;
    CGGameUI::m_summonConfirmTime = 0;

    g_theGxDevicePtr->CapsWindowSizeInScreenCoords(windowRect);
    CGGameUI::unk_BD07F0 = 0;
    CGGameUI::unk_BD07F4 = 0;
    CGGameUI::SetAspect();

    CGGameUI::m_simpleTop = new (__FILE__, __LINE__) CSimpleTop();
    CGGameUI::m_simpleTop->m_mouseButtonCallback = CGGameUI::FilterMouseDown;
    CGGameUI::m_simpleTop->m_mouseMoveCallback = CGGameUI::FilterMouseMotion;
    CGGameUI::m_simpleTop->m_displaySizeCallback = CGGameUI::HandleDisplaySizeChanged;
    CGGameUI::m_simpleTop->m_focusChangedCallback = CGGameUI::HandleFocusChanged;
    CGGameUI::m_simpleTop->m_allowProtectedActions = 1;
    CGGameUI::m_simpleTop->m_actionBlockedCallback = CGGameUI::ShowBlockedFrameFeedback;

    CursorInitialize();

    CGInputControl* input = CGInputControl::GetActive();

    if (!input) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    input->m_mouseModeFlags = 3;
    input->m_forceCursorOn = 0;
    input->UpdateMouseMode(1);

    FrameScript_Destroy();
    FrameScript_Initialize(g_scriptProfileCVar->m_intValue);
    LoadScriptFunctions();
    FrameScript_CreateEvents(g_scriptEvents, NUM_SCRIPT_EVENTS);
    CGGameUI::RegisterGameCVars();
    CGUIBindings::Initialize();
    CGWorldFrame::RegisterFrames();

    if (GetDataInterfaceVersion() != GetCodeInterfaceVersion()) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    OsCreateDirectory("Logs", 0);

    CWOWClientStatus status("Logs\\FrameXML.log");

    LoadAddOnEnableState(ClientServices::GetCharacterName());

    MD5_CTX md5;
    MD5Init(&md5);

    uint8_t signature[16];

    switch (FrameXML_CheckSignature("Interface\\FrameXML\\FrameXML.toc", "Interface\\FrameXML\\Bindings.xml", InterfaceKey, signature)) {
        case 0:
            status.Add(STATUS_WARNING, "FrameXML missing signature");
            ClientPostClose(10);
            break;

        case 1:
            status.Add(STATUS_WARNING, "FrameXML has corrupt signature");
            ClientPostClose(10);
            break;

        case 2:
            status.Add(STATUS_WARNING, "FrameXML is modified or corrupt");
            ClientPostClose(10);
            break;

        case 3:
            break;

        default:
            ClientPostClose(10);
            break;
    }

    uint32_t numFiles = 0;
    void* toc;

    if (SFile::ReadFile(nullptr, "Interface\\FrameXML\\FrameXML.toc", &toc, nullptr, 1, 1, nullptr)) {
        numFiles = FrameXML_GuessNumFiles(static_cast<const char*>(toc));
        SFile::Unload(toc);
    }

    FrameXML_RegisterLoadProgressCallback(LoadingScreenXMLCallback, nullptr, LoadAddOnFileCount() + numFiles);
    FrameXML_FreeHashNodes();
    FrameXML_CreateFrames("Interface\\FrameXML\\FrameXML.toc", nullptr, &md5, &status);

    if (SFile::FileExistsEx("Interface\\FrameXML\\Bindings.xml", 1)) {
        CGUIBindings::s_bindings->Load("Interface\\FrameXML\\Bindings.xml", &md5, &status);
    }

    uint8_t digest[16];
    MD5Final(digest, &md5);

    if (memcmp(signature, digest, sizeof(digest))) {
        ClientPostClose(10);
    }

    CGGameUI::m_UISimpleParent = static_cast<CSimpleFrame*>(CScriptObject::GetScriptObjectByName("UIParent", CSimpleFrame::GetObjectType()));

    if (!CGGameUI::m_UISimpleParent) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    CGGameUI::m_gameTooltip = reinterpret_cast<CGTooltip*>(CScriptObject::GetScriptObjectByName("GameTooltip", CGTooltip::GetObjectType()));

    if (!CGGameUI::m_gameTooltip) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    LoadAddOns(&status);
    FrameXML_RegisterLoadProgressCallback(nullptr, nullptr, 0);
    CGGameUI::Initialize();
    LoadSavedVariables(&status);
    LoadAccountData(6, LoadPlacedFrames, nullptr);
    LoadAccountVariables();
    CGUIBindings::LoadBindings();
    CGUIMacros::Initialize();
    CGUIMacros::LoadMacros();
    CGChat::LoadChatSettings();
    CSimpleScriptManager::Create();

    CSizeEvent sizeEvent;
    sizeEvent.m_data.m_w = static_cast<int32_t>(windowRect.maxX - windowRect.minX);
    sizeEvent.m_data.m_h = static_cast<int32_t>(windowRect.maxY - windowRect.minY);
    CGGameUI::HandleDisplaySizeChanged(sizeEvent);

    CGGameUI::m_initialized = 1;
    EventRegister(EVENT_ID_IDLE, CGGameUI::Idle);
    g_theGxDevicePtr->AddDeviceRestoredCallback(UpdateAllPortraits);
    g_theGxDevicePtr->AddTextureRecreationCallback(UpdateAllPortraits);

    if (ClntObjMgrGetActivePlayer()) {
        CGGameUI::EnterWorld();
    }

    CGChat::Resume();
    CGGameUI::m_currentlyReloadingUI = 0;
}

// VERIFY: aea57f10 auto
// OFFSET: 0x0052B550
void CGGameUI::InitializeGame() {
    ClientServices::SetMessageHandler(SMSG_PVP_CREDIT, CGGameUI::MessageHandler, nullptr);
    ClientServices::SetMessageHandler(MSG_CORPSE_QUERY, CGGameUI::MessageHandler, nullptr);
    ClientServices::SetMessageHandler(SMSG_CORPSE_RECLAIM_DELAY, CGGameUI::MessageHandler, nullptr);
    ClientServices::SetMessageHandler(SMSG_GM_PLAYER_INFO, CGGameUI::MessageHandler, nullptr);
    ClientServices::SetMessageHandler(CMSG_GM_REQUEST_PLAYER_INFO, CGGameUI::MessageHandler, nullptr);
    ClientServices::SetMessageHandler(SMSG_PLAY_MUSIC, CGGameUI::MessageHandler, nullptr);
    ClientServices::SetMessageHandler(SMSG_PLAY_SOUND, CGGameUI::MessageHandler, nullptr);
    ClientServices::SetMessageHandler(SMSG_PLAY_OBJECT_SOUND, CGGameUI::MessageHandler, nullptr);
    ClientServices::SetMessageHandler(SMSG_TRIGGER_MOVIE, CGGameUI::MessageHandler, nullptr);
    ClientServices::SetMessageHandler(SMSG_AREA_TRIGGER_MESSAGE, CGGameUI::MessageHandler, nullptr);
    ClientServices::SetMessageHandler(SMSG_AREA_TRIGGER_NO_CORPSE, CGGameUI::MessageHandler, nullptr);
    ClientServices::SetMessageHandler(SMSG_INIT_WORLD_STATES, CGGameUI::MessageHandler, nullptr);
    ClientServices::SetMessageHandler(SMSG_UPDATE_WORLD_STATE, CGGameUI::MessageHandler, nullptr);
    ClientServices::SetMessageHandler(SMSG_AREA_SPIRIT_HEALER_TIME, CGGameUI::MessageHandler, nullptr);
    ClientServices::SetMessageHandler(SMSG_INVALID_PROMOTION_CODE, CGGameUI::MessageHandler, nullptr);
    ClientServices::SetMessageHandler(SMSG_WEATHER, CGGameUI::MessageHandler, nullptr);
    ClientServices::SetMessageHandler(SMSG_OVERRIDE_LIGHT, CGGameUI::MessageHandler, nullptr);
    ClientServices::SetMessageHandler(SMSG_INSTANCE_DIFFICULTY, CGGameUI::MessageHandler, nullptr);
    ClientServices::SetMessageHandler(MSG_SET_DUNGEON_DIFFICULTY, CGGameUI::MessageHandler, nullptr);
    ClientServices::SetMessageHandler(MSG_SET_RAID_DIFFICULTY, CGGameUI::MessageHandler, nullptr);
    ClientServices::SetMessageHandler(SMSG_UPDATE_COMBO_POINTS, CGGameUI::MessageHandler, nullptr);
    ClientServices::SetMessageHandler(SMSG_PHASE_SHIFT_CHANGE, CGGameUI::MessageHandler, nullptr);
    ClientServices::SetMessageHandler(SMSG_TOTEM_CREATED, CGGameUI::MessageHandler, nullptr);
    ClientServices::SetMessageHandler(SMSG_START_MIRROR_TIMER, CGGameUI::OnMirrorTimerEvent, nullptr);
    ClientServices::SetMessageHandler(SMSG_PAUSE_MIRROR_TIMER, CGGameUI::OnMirrorTimerEvent, nullptr);
    ClientServices::SetMessageHandler(SMSG_STOP_MIRROR_TIMER, CGGameUI::OnMirrorTimerEvent, nullptr);
    ClientServices::SetMessageHandler(SMSG_PROPOSE_LEVEL_GRANT, CGGameUI::MessageHandler, nullptr);
    ClientServices::SetMessageHandler(SMSG_REFER_A_FRIEND_FAILURE, CGGameUI::MessageHandler, nullptr);
    ClientServices::SetMessageHandler(SMSG_REFER_A_FRIEND_EXPIRED, CGGameUI::MessageHandler, nullptr);
    ClientServices::SetMessageHandler(SMSG_BREAK_TARGET, CGGameUI::MessageHandler, nullptr);
    ClientServices::SetMessageHandler(SMSG_CORPSE_MAP_POSITION_QUERY_RESPONSE, CGGameUI::MessageHandler, nullptr);
    ClientServices::SetMessageHandler(SMSG_REMOVED_FROM_PVP_QUEUE, CGGameUI::MessageHandler, nullptr);
    ClientServices::SetMessageHandler(SMSG_UI_TIME, CGGameUI::MessageHandler, nullptr);
    ClientServices::SetMessageHandler(SMSG_TALENTS_INVOLUNTARILY_RESET, CGGameUI::MessageHandler, nullptr);

    do {
        CGGameUI::unk_BD08C4++;
    } while (!CGGameUI::unk_BD08C4);

    for (uint32_t i = 0; i < sizeof(s_readOnlyCVars) / sizeof(s_readOnlyCVars[0]); i++) {
        CVar* var = CVar::Lookup(s_readOnlyCVars[i]);

        if (var) {
            var->SetReadOnly(true);
        }
    }

    DBCache_ClearTemporaryCaches();
    PortraitInitialize();
    CGMinimapFrame::InitializeGame();
    CGChat::InitializeGame();
    CGPartyInfo::InitializeGame();
    CGSpellBook::InitializeGame();
    CGActionBar::InitializeGame();
    CGLootInfo::InitializeGame();
    CGItemText::InitializeGame();
    CGGossipInfo::InitializeGame();
    CGQuestLog::InitializeGame();
    CGTaxiMap::InitializeGame();
    CGClassTrainer::InitializeGame();
    CGPetInfo::InitializeGame();
    CGWorldMap::InitializeGame();
    CGReputationInfo::InitializeGame();
    CGDuelInfo::InitializeGame();
    CGTutorial::InitializeGame();
    CGSkillInfo::InitializeGame();
    CGGuildInfo::InitializeGame();
    CGMailInfo::InitializeGame();
    CGBattlefieldInfo::InitializeGame();
    CGAuctionHouse::InitializeGame();
    CGStableInfo::InitializeGame();
    CGRaidInfo::InitializeGame();
    CGChatBubbleFrame::InitializeGame();
    CGCharacterInfo::InitializeGame();
    CGWorldStateInfo::InitializeGame();
    CGMinigameInfo::InitializeGame();
    CGArenaTeamInfo::InitializeGame();
    CGLookingForGroup::InitializeGame();
    CGPetitionInfo::InitializeGame();
    CGCommentator::InitializeGame();
    CGCalendar::InitializeGame();
    CGDanceStudio::InitializeGame();
    CGBarberShop::InitializeGame();
    CGGuildBankInfo::InitializeGame();
    CGAchievementInfo::InitializeGame();
    CGRuneInfo::InitializeGame();
    CGCurrencyTypes::InitializeGame();
    CGEquipmentManager::InitializeGame();
    CGGMTicketInfo::InitializeGame();
    CGGMSurveyInfo::InitializeGame();

    CGGameUI::m_hasControl = 1;
    CGGameUI::m_inCinematic = 0;
    CGGameUI::m_billingExpireTime = 0;

    uint32_t billingMinutes = ClientServices::Connection()->m_billingTimeRemaining;

    if (billingMinutes <= 4320 && !(ClientServices::Connection()->m_billingFlags & 0x2) && billingMinutes) {
        CGGameUI::m_billingExpireTime = 60000 * billingMinutes + OsGetAsyncTimeMs();
        CGGameUI::unk_BD0878 = 0;
        CGGameUI::unk_BD087C = 0;
    }

    CGGameUI::unk_AC80A4 = 1;
    CGGameUI::m_instanceDifficulty = 0;
    CGGameUI::m_dungeonDifficulty = 0;
    CGGameUI::m_lastRegenTime = 0;
    CGGameUI::m_comboTarget = WOWGUID();
    CGGameUI::m_comboPoints = 0;

    for (int32_t i = 0; i < 4; i++) {
        TotemInfo& totem = CGGameUI::m_totems[i];
        totem.m_slot = i;
        totem.m_guid = WOWGUID();
        totem.m_name = nullptr;
        totem.unk_0014 = 0;
        totem.unk_0018 = 0;
        totem.unk_001C = 0;
    }

    for (int32_t i = 0; i < 3; i++) {
        MirrorTimerInfo& timer = CGGameUI::m_mirrorTimerInfo[i];
        timer.unk_0000 = 3;
        timer.unk_0018 = 0;
        timer.unk_0014 = 0;
        timer.unk_0010 = 0;
        timer.unk_000C = 0;
        timer.unk_0008 = 0;
        timer.unk_0004 = 0;
    }

    if (SFile::IsTrial()) {
        SetActiveDataSource(0);
        int32_t race = StreamingGetRace();

        while (!StreamingIsRaceDataReady(race)) {
            LoadingScreenSpellUpdateProgress(StreamingGetLoadFraction(race));
            OsSleep(10);
        }

        SetActiveDataSource(1);
        StreamingSetState(3);
    }

    CGGameUI::InitializeFrameXML();

    if (CGInputControl::GetActive()->WowMouseEnabled() && !CGInputControl::GetActive()->WowMouseConnected()) {
        FrameScript_SignalEvent(SCRIPT_EVENT_WOW_MOUSE_NOT_FOUND, nullptr);
    }
}

// VERIFY: d38c16ee manual
// OFFSET: 0x0052AF40
int32_t CGGameUI::Idle(const void* data, void* param) {
    if (g_theGxDevicePtr->StereoEnabled()) {
        float convergence = g_theGxDevicePtr->StereoGetConvergence();

        if (CGGameUI::m_UISimpleParent) {
            CGGameUI::m_UISimpleParent->SetFrameDepth(convergence - CGGameUI::m_UISimpleParent->m_parentDepth, false);
        }
    }

    lua_State* L = FrameScript_GetContext();
    lua_taintexpected = 0;

    if (lua_gettop(FrameScript_GetContext())) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && !(player->m_unit->m_unitFieldFlags & 0x80000)) {
        lua_gc(L, LUA_GCSTEP, 1);
    }

    if (CGGameUI::m_reloadUIRequested && !CGGameUI::m_inCinematic) {
        CGGameUI::m_currentlyReloadingUI = 1;
        CoordinateSetAspectRatio(Screen::GetAspectRatio());
        LoadingScreenEnable(CGGameUI::m_iCurrentMapID, false);
        ClientServices::Connection()->CharacterAbortLogout();
        CGGameUI::Shutdown();
        CGGameUI::InitializeFrameXML();
        CVehiclePassenger_C::OnReloadUI();
        LoadingScreenDisable();
        return 1;
    }

    if (CGGameUI::m_billingExpireTime) {
        uint32_t now = OsGetAsyncTimeMs();
        uint32_t remaining = CGGameUI::m_billingExpireTime - now;
        uint32_t minutes = static_cast<uint32_t>(ceil(remaining * (1.0f / 60000.0f)));
        int32_t igr = (ClientServices::Connection()->m_billingFlags >> 3) & 1;

        if (!CGGameUI::unk_BD0878) {
            if (igr) {
                if (minutes <= 600) {
                    FrameScript_SignalEvent(SCRIPT_EVENT_IGR_BILLING_NAG_DIALOG, nullptr);
                    CGGameUI::unk_BD087C = 1;
                }
            } else if (minutes <= 30) {
                FrameScript_SignalEvent(SCRIPT_EVENT_BILLING_NAG_DIALOG, "%d", minutes);
                CGGameUI::unk_BD087C = 1;
            }

            CGGameUI::unk_BD0878 = now;
        } else if (static_cast<int32_t>(now - CGGameUI::unk_BD0878) >= 0 && !igr) {
            if (minutes <= 30) {
                char text[3000];
                SStrPrintf(text, sizeof(text), FrameScript_GetText("BILLING_NAG_WARNING", -1, GENDER_NOT_APPLICABLE), minutes);
                CGChat::AddChatMessage(text, 0, 0, 0, 0, 0, nullptr, WOWGUID(), 0, WOWGUID(), 0, 0, 0);
            }

            if (static_cast<int32_t>(now - CGGameUI::m_billingExpireTime) < 0) {
                if (minutes <= 5) {
                    CGGameUI::unk_BD0878 = now + remaining % 60000;
                } else {
                    CGGameUI::unk_BD0878 = now + remaining % 300000;
                }
            } else {
                CGGameUI::m_billingExpireTime = 0;
            }
        }

        if (igr) {
            if (minutes <= 600 && !CGGameUI::unk_BD087C) {
                FrameScript_SignalEvent(SCRIPT_EVENT_IGR_BILLING_NAG_DIALOG, nullptr);
                CGGameUI::unk_BD087C = 1;
            }
        } else if (minutes <= 30 && !CGGameUI::unk_BD087C) {
            FrameScript_SignalEvent(SCRIPT_EVENT_BILLING_NAG_DIALOG, "%d", minutes);
            CGGameUI::unk_BD087C = 1;
        }
    }

    CGGameUI::DoPowerRegen();
    CGGameUI::AutoMouseOver();
    CGChat::ProcessThrottledChat();
    CGQuestLog::UpdateWatchTimers();
    CGUIMacros::UpdateDynamicMacros();
    CKBPage::UpdateLoadingQueue();
    g_capturePointUIManager.Update();
    DBCache_Idle();
    Script_SendUniqueSignals();

    int32_t idleTime = OsGetAsyncTimeMs() - CSimpleTop::s_eventTime;

    if (idleTime - 300000 >= 0) {
        if (idleTime - 1800000 < 0) {
            if (player) {
                if (!player->m_unit->m_unitFieldBytes1[0] && !(player->m_unit->m_unitFieldFlags & 0x80000) && !player->IsMounted() && !player->IsAutoTracking()) {
                    player->ChangeStandState(1);
                }

                if (!(player->m_unit->m_unitFieldFlags & 0x100000) && !CGChat::m_afk) {
                    player->SetAFK(nullptr);
                }
            }
        } else if (!ClientServices::Connection()->CharacterLoggingOut()) {
            CGChat::AddChatMessage(FrameScript_GetText("IDLE_MESSAGE", -1, GENDER_NOT_APPLICABLE), 0, 0, 0, 0, 0, nullptr, WOWGUID(), 0, WOWGUID(), 0, 0, 0);
            CGLCD::IdleDisconnectWarning();
            ClientServices::Connection()->CharacterLogout(false, false);
        }
    }

    return 1;
}

// VERIFY: 53c66aae manual
// OFFSET: 0x00512CD0
int32_t CGGameUI::FilterMouseMotion(CMouseEvent* evt) {
    if (evt->m_id != 0x400500CB) {
        return 0;
    }

    CGInputControl::GetActive()->OnMouseMoveRel(*evt);
    return 1;
}

// VERIFY: a4222caa manual
// OFFSET: 0x00520F70
void CGGameUI::UpdateActivePlayer() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    if (player->m_unit->m_unitFieldHealth > 0) {
        FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_ALIVE, nullptr);
    } else {
        FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_DEAD, nullptr);
        CGGameUI::ClearCursor(1, 1);
    }
}

// VERIFY: 874cc04c auto
// OFFSET: 0x00512D20
int32_t CGGameUI::IsKeyDown(KEY key) {
    if (key <= KEY_RALT && CGUIBindings::s_bindings->m_keyStateOverride) {
        return ((1 << key) & CGUIBindings::s_bindings->m_heldModifiers) != 0;
    }

    return EventIsKeyDown(key);
}

// VERIFY: 5cf4150f auto
// OFFSET: 0x00513C30
CGPartyMemberState* CGPartyMemberStateRepository::GetState(const WOWGUID& guid) {
    if (guid == WOWGUID()) {
        return nullptr;
    }

    CGPartyMemberState* state = CGPartyInfo::GetRemoteStats(guid);

    if (!state) {
        state = CGRaidInfo::GetRemoteStats(guid);

        if (!state) {
            return CGBattlefieldInfo::GetRemoteStats(guid);
        }
    }

    return state;
}

// VERIFY: 2883a09c auto
// OFFSET: 0x00513CB0
CGPartyPetState* CGPartyMemberStateRepository::FindPetState(const WOWGUID& guid) {
    if (guid == WOWGUID()) {
        return nullptr;
    }

    CGPartyPetState* state = CGPartyInfo::GetRemotePetStats(guid);

    if (!state) {
        state = CGRaidInfo::GetRemotePetStats(guid);

        if (!state) {
            return CGBattlefieldInfo::GetRemotePetStats(guid);
        }
    }

    return state;
}

// VERIFY: bb4e411b auto
// OFFSET: 0x0051FB80
void CGGameUI::ScaleToScreenSize(int32_t width, int32_t height, int32_t force) {
    if (!force && CGGameUI::unk_BD07F0 > 0 && CGGameUI::unk_BD07F0 == width && CGGameUI::unk_BD07F4 > 0 && CGGameUI::unk_BD07F4 == height) {
        return;
    }

    CGGameUI::unk_BD07F0 = width;
    CGGameUI::unk_BD07F4 = height;

    float aspect = Screen::GetAspectRatio();
    float scale = height > 768 ? 768.0f / height : 1.0f;

    if (aspect * 3.0f < 4.0f && aspect * 0.75f <= scale) {
        scale = aspect * 0.75f;
    }

    if (scale <= 0.89999998f) {
        scale = 0.89999998f;
    }

    if (scale <= 0.63999999f) {
        scale = 0.63999999f;
    }

    if (CGGameUI::m_UISimpleParent) {
        CGGameUI::m_UISimpleParent->SetFrameScale(scale, true);
        CLayoutFrame::ResizePending();
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_FLOATING_CHAT_WINDOWS, nullptr);
    }

    if (fabs(CGGameUI::m_aspect - aspect) >= 0.001f) {
        if (lua_tainted && CGGameUI::m_simpleTop && !CGGameUI::m_simpleTop->unk_1250) {
            CGGameUI::ShowBlockedActionFeedback(nullptr, 1);
            CGGameUI::m_aspect = aspect;
        } else {
            CGGameUI::m_aspect = aspect;
            CGGameUI::m_reloadUIRequested = 1;
        }
    }
}

// VERIFY: 7d99c1aa auto
// OFFSET: 0x005240E0
void CGGameUI::HandleDisplaySizeChanged(const CSizeEvent& evt) {
    if (g_useUiScaleCVar->m_intValue) {
        float scale = g_uiScaleCVar->m_floatValue;

        if (3.0f * CGGameUI::m_aspect < 4.0f && CGGameUI::m_aspect * 0.75f <= scale) {
            scale = CGGameUI::m_aspect * 0.75f;
        }

        if (scale >= 0.63999999f) {
            CGGameUI::SetUIScale(scale);
            float aspect = Screen::GetAspectRatio();

            if (fabs(CGGameUI::m_aspect - aspect) >= 0.001f) {
                CGGameUI::Reload();
                CGGameUI::m_aspect = aspect;
            }

            return;
        }
    }

    CGGameUI::ScaleToScreenSize(evt.m_data.m_w, evt.m_data.m_h, 0);
    FrameScript_SignalEvent(SCRIPT_EVENT_DISPLAY_SIZE_CHANGED, nullptr);
}

// VERIFY: 0347ae4f auto
// OFFSET: 0x00512D60
void CGGameUI::OnMouseModeRelative() {
    CGWorldFrame* worldFrame = CGWorldFrame::s_currentWorldFrame;

    if (!worldFrame) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    worldFrame->OnMouseModeRelative();

    if (!CSimpleTop::s_instance) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    CSimpleTop::s_instance->SetMouseFocus(worldFrame);
    g_theGxDevicePtr->CursorSetVisible(0);
    EventSetMouseMode(MOUSE_MODE_RELATIVE, 0);
}

// VERIFY: 2abcfd33 auto
// OFFSET: 0x00512DC0
void CGGameUI::OnMouseModeNormal() {
    if (!CGWorldFrame::s_currentWorldFrame) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    CGWorldFrame::s_currentWorldFrame->OnMouseModeNormal();

    if (!CSimpleTop::s_instance) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    CSimpleTop::s_instance->m_checkFocus = 1;

    if (CGInputControl::GetActive()->m_mouseModeFlags & 0x1) {
        g_theGxDevicePtr->CursorSetVisible(1);
    }

    EventSetMouseMode(MOUSE_MODE_NORMAL, 0);
}

// VERIFY: 68209d6d auto
// OFFSET: 0x0051FB60
void CGGameUI::HandleSpriteTrack(const CObjectTrackEvent& evt) {
    CGGameUI::HandleObjectTrackChange(evt.m_guid, evt.m_prevGuid);
}

// VERIFY: b02d0e37 manual
// OFFSET: 0x005277B0
int32_t CGGameUI::OnSpriteRightClick(WOWGUID guid) {
    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return 1;
    }

    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!object) {
        return 0;
    }

    CGGameUI::Target(guid);
    object->OnRightClick();
    return 1;
}

// VERIFY: a9387b84 auto
// OFFSET: 0x00527830
int32_t CGGameUI::HandleTerrainClick(const CTerrainClickEvent& evt) {
    if (evt.m_button == MOUSE_BUTTON_RIGHT || CGGameUI::m_cursorVirtualID) {
        CGGameUI::ClearCursor(1, 1);
    }

    CGGameUI::OnTerrainClick(evt);
    return 1;
}

// VERIFY: cdcd662a auto
// OFFSET: 0x00527870
int32_t CGGameUI::HandleSpriteClick(const CSpriteClickEvent& evt) {
    if (CGGameUI::m_cursorVirtualID) {
        CGGameUI::ClearCursor(1, 1);
    }

    if (evt.m_button == MOUSE_BUTTON_LEFT) {
        CGGameUI::OnSpriteLeftClick(evt.m_guid);
    } else {
        CGGameUI::OnSpriteRightClick(evt.m_guid);
    }

    return 1;
}

// VERIFY: 3b71543e manual
// OFFSET: 0x005278C0
int32_t CGGameUI::HandleWorldClick(const CWorldClickEvent& evt) {
    int32_t noCursorItem = CGGameUI::m_cursorItemType == 0;
    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_PLAYER, __FILE__, __LINE__));
    C3Vector direction(evt.m_segEnd.x - evt.m_segStart.x, evt.m_segEnd.y - evt.m_segStart.y, evt.m_segEnd.z - evt.m_segStart.z);

    if (noCursorItem && direction.x * direction.x + direction.z * direction.z + direction.y * direction.y != 0.0f && mover && mover->CanAutoInteract() && evt.m_button == MOUSE_BUTTON_RIGHT) {
        mover->SetTrackingDirection(direction);
        return 1;
    }

    if (evt.m_button == MOUSE_BUTTON_RIGHT || !CGGameUI::m_cursorItem || !CGGameUI::m_cursorItemContainer) {
        CGGameUI::ClearCursor(1, 1);
    }

    if (evt.m_button == MOUSE_BUTTON_LEFT) {
        if (CGGameUI::m_cursorItem && CGGameUI::m_cursorItemContainer) {
            CGGameUI::DeleteCursorItem();
            return 1;
        }

        if (noCursorItem && g_deselectOnClickCVar->m_intValue) {
            CGGameUI::unk_BD08CC = 0;

            if (CGGameUI::m_lockedTarget) {
                CGGameUI::m_lastTarget = CGGameUI::m_lockedTarget;
                CGGameUI::ClearTarget(CGGameUI::m_lockedTarget, 1);

                if (Spell_C_GetAutoRepeatingSpell()) {
                    Spell_C_CancelAutoRepeat(true);
                }
            }
        }
    }

    return 1;
}

// VERIFY: e196a0f1 auto
// OFFSET: 0x00513680
void CGGameUI::GetCursorItem(WOWGUID& item, WOWGUID& container, uint32_t& slot) {
    item = CGGameUI::m_cursorItem;
    container = CGGameUI::m_cursorItemContainer;
    slot = CGGameUI::m_cursorItemSlot;
}

// VERIFY: 9a21a87d manual
// OFFSET: 0x00513770
void CGGameUI::UnlockItem(WOWGUID guid) {
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (item) {
        item->Unlock();
    }

    CGActionBar::UpdateUsable();
}

// VERIFY: 7503e0be manual
// OFFSET: 0x005186A0
void CGGameUI::ResetCamera(WOWGUID guid) {
    WOWGUID target = guid;

    if (!guid) {
        target = ClntObjMgrGetActivePlayer();
    }

    CGObject_C* object = ClntObjMgrObjectPtr(target, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!object || !CGWorldFrame::s_currentWorldFrame) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    CGWorldFrame::s_currentWorldFrame->SetCameraTarget(object, 0);
}

// VERIFY: 21eb442b manual
// OFFSET: 0x00518DC0
void CGGameUI::SendTarget(WOWGUID guid) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_SET_SELECTION));
    msg.Put(guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: ade1e048 manual
// OFFSET: 0x00519280
void CGGameUI::ClearCursor(int32_t a1, int32_t a2) {
    if (CGPlayer_C::IsGiftWrapping()) {
        CGPlayer_C::CancelGiftWrap();
    }

    CursorDropItem();

    switch (CGGameUI::m_cursorItemType) {
        case 1: {
            if (a1 && CGGameUI::m_cursorItem) {
                CGGameUI::UnlockItem(CGGameUI::m_cursorItem);
            }

            CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(CGGameUI::m_cursorItem, TYPEMASK_ITEM, __FILE__, __LINE__));

            if (item) {
                SndInterfacePlayItemSound(1, item);
            }

            CGGameUI::m_cursorItem = WOWGUID();
            CGGameUI::m_cursorItemContainer = WOWGUID();
            CGGameUI::m_cursorItemSlot = 0;
            break;
        }

        case 2:
            CGGameUI::m_cursorMoney = 0;
            SI2::PlaySoundKit("LOOTWINDOWCOINSOUND", nullptr, nullptr, nullptr);

            if (a2) {
                FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_MONEY, nullptr);
            }

            break;

        case 3:
            CGGameUI::m_cursorSpell = 0;
            SI2::PlaySoundKit("INTERFACESOUND_CURSORDROPOBJECT", nullptr, nullptr, nullptr);
            break;

        case 4:
            CGGameUI::m_cursorPetAction = 0;
            SI2::PlaySoundKit("INTERFACESOUND_CURSORDROPOBJECT", nullptr, nullptr, nullptr);
            break;

        case 5:
        case 6:
        case 7:
        case 9:
            SI2::PlayItemSound(1, CGGameUI::m_cursorVirtualDisplay);
            CGGameUI::m_cursorVirtualID = 0;
            CGGameUI::m_cursorVirtualDisplay = 0;
            CGGameUI::m_cursorVirtualSlot = 0;
            SI2::PlaySoundKit("INTERFACESOUND_CURSORDROPOBJECT", nullptr, nullptr, nullptr);
            break;

        case 8:
            CGGameUI::m_cursorMacro = 0;
            SI2::PlaySoundKit("INTERFACESOUND_CURSORDROPOBJECT", nullptr, nullptr, nullptr);
            break;

        case 10:
            CGGameUI::m_cursorPet = 0;
            SI2::PlaySoundKit("INTERFACESOUND_CURSORDROPOBJECT", nullptr, nullptr, nullptr);
            break;

        case 11:
            SI2::PlayItemSound(1, CGGameUI::m_cursorVirtualDisplay);

            if (a1 && CGGameUI::m_cursorVirtualID) {
                CGGuildBankInfo::Unlock(CGGameUI::m_cursorVirtualID, CGGameUI::m_cursorVirtualSlot);
                FrameScript_SignalEvent(SCRIPT_EVENT_GUILDBANK_ITEM_LOCK_CHANGED, nullptr);
            }

            CGGameUI::m_cursorVirtualID = 0;
            CGGameUI::m_cursorVirtualDisplay = 0;
            CGGameUI::m_cursorVirtualSlot = 0;
            break;

        case 12:
            CGGameUI::m_cursorMoney = 0;
            SI2::PlaySoundKit("LOOTWINDOWCOINSOUND", nullptr, nullptr, nullptr);

            if (a2) {
                FrameScript_SignalEvent(SCRIPT_EVENT_GUILDBANK_UPDATE_MONEY, nullptr);
            }

            break;

        case 13:
            CGGameUI::m_cursorEquipmentSet = -1;
            SI2::PlaySoundKit("INTERFACESOUND_CURSORDROPOBJECT", nullptr, nullptr, nullptr);
            break;

        default:
            break;
    }

    if (CGGameUI::m_cursorHasAction) {
        if (CGGameUI::m_cursorItemType == 4) {
            CGPetInfo::HideGrid();
        } else {
            CGActionBar::HideGrid();
        }

        CGGameUI::m_cursorHasAction = 0;
    }

    CGGameUI::m_cursorItemType = 0;
    FrameScript_SignalEvent(SCRIPT_EVENT_CURSOR_UPDATE, nullptr);
}

// VERIFY: 405325d4 manual
// OFFSET: 0x0051F790
void CGGameUI::HandleObjectTrackChange(WOWGUID guid, WOWGUID prevGuid) {
    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    WOWGUID previous = prevGuid;

    if (previous || (previous = CGGameUI::m_currentObjectTrack)) {
        CGGameUI::m_currentObjectTrack = WOWGUID();
        CGObject_C* object = ClntObjMgrObjectPtr(previous, TYPEMASK_OBJECT, __FILE__, __LINE__);
        CGGameUI::m_gameTooltip->FadeOut();

        if (object) {
            object->HideHighlightType(1);
        }
    }

    CGGameUI::m_currentObjectTrack = guid;
    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!object) {
        return;
    }

    if (CGGameUI::m_visible || (g_unitHighlightsCVar && g_unitHighlightsCVar->m_intValue)) {
        object->ShowHighlightType(1);
    }

    switch (static_cast<uint32_t>(object->m_obj->m_type)) {
        case TYPEMASK_OBJECT | TYPEMASK_UNIT:
        case TYPEMASK_OBJECT | TYPEMASK_UNIT | TYPEMASK_PLAYER: {
            if (CSimpleTop::GetInstance()->m_mouseFocus == CGWorldFrame::s_currentWorldFrame) {
                CGGameUI::m_gameTooltip->SetDefaultAnchor();
                WOWGUID none;
                CGGameUI::m_gameTooltip->SetUnit(guid, 1, none, 0);
            }

            FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_MOUSEOVER_UNIT, nullptr);
            CGUnit_C* unit = static_cast<CGUnit_C*>(object);

            if (unit->IsHatedTowards(player)) {
                SI2::PlaySoundKit("GAMEHIGHLIGHTHOSTILEUNIT", nullptr, nullptr, nullptr);
            } else if (unit->IsFriendly(player)) {
                SI2::PlaySoundKit("GAMEHIGHLIGHTFRIENDLYUNIT", nullptr, nullptr, nullptr);
            } else {
                SI2::PlaySoundKit("GAMEHIGHLIGHTNEUTRALUNIT", nullptr, nullptr, nullptr);
            }

            break;
        }

        case TYPEMASK_OBJECT | TYPEMASK_GAMEOBJECT:
            if (CSimpleTop::GetInstance()->m_mouseFocus == CGWorldFrame::s_currentWorldFrame) {
                if (object->FloatingTooltip()) {
                    CGGameUI::m_gameTooltip->SetOwner(CGGameUI::m_UISimpleParent, ANCHOR_CURSOR, 0.0f, 0.0f);
                } else {
                    CGGameUI::m_gameTooltip->SetDefaultAnchor();
                }

                CGGameUI::m_gameTooltip->SetObject(guid);
            }

            SI2::PlaySoundKit("GAMEHIGHLIGHTNEUTRALUNIT", nullptr, nullptr, nullptr);
            break;

        case TYPEMASK_OBJECT | TYPEMASK_CORPSE:
            if (CSimpleTop::GetInstance()->m_mouseFocus == CGWorldFrame::s_currentWorldFrame) {
                CGGameUI::m_gameTooltip->SetDefaultAnchor();
                CGGameUI::m_gameTooltip->SetCorpse(guid);
            }

            SI2::PlaySoundKit("GAMEHIGHLIGHTNEUTRALUNIT", nullptr, nullptr, nullptr);
            break;

        default:
            break;
    }
}

// VERIFY: ecd66915 manual
// OFFSET: 0x005206E0
void CGGameUI::UpdateCombatMode() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    CGUIBindings* bindings = CGUIBindings::s_bindings;

    if (!bindings) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    if ((player->m_unit->m_unitFieldFlags & 0x80000) || (player->m_combatModeFlags & 0x2) || player->CGUnit_C::IsInMelee()) {
        bindings->EnableBindingMode(BINDING_MODE_1, 1);
    } else {
        bindings->EnableBindingMode(BINDING_MODE_1, 0);
    }
}

// VERIFY: c5227502 auto
// OFFSET: 0x00522270
bool CanTriggerTutorial(uint32_t tutorial) {
    return CGTutorial::m_tutorialShown.m_count && !(CGTutorial::m_tutorialShown.m_bits.m_data[tutorial >> 5] & (1 << (tutorial & 0x1F)));
}

// VERIFY: d61f5660 auto
// OFFSET: 0x005222B0
bool IsTutorialFlagged(uint32_t tutorial) {
    return CGTutorial::m_tutorialFlags.m_count && (CGTutorial::m_tutorialFlags.m_bits.m_data[tutorial >> 5] & (1 << (tutorial & 0x1F)));
}

// VERIFY: 9f2b88f1 manual
// OFFSET: 0x005241B0
void CGGameUI::ClearTarget(WOWGUID guid, int32_t a2) {
    int32_t noMgr = ClntObjMgrGetCurrent() == nullptr;

    if (!CGGameUI::m_lockedTarget || (guid && guid != CGGameUI::m_lockedTarget)) {
        return;
    }

    if (CGLootInfo::m_lootObj == CGGameUI::m_lockedTarget) {
        CGGameUI::CloseLoot(true, false, false);
    }

    if (!noMgr) {
        CGObject_C* object = ClntObjMgrObjectPtr(CGGameUI::m_lockedTarget, TYPEMASK_OBJECT, __FILE__, __LINE__);

        if (object) {
            object->HideHighlightType(0);

            if (object->m_obj->m_type & TYPEMASK_UNIT) {
                CGUnit_C* unit = static_cast<CGUnit_C*>(object);
                PlayerNameTriggerColorUpdate(unit->m_nameDesc);
                unit->UnregisterScript(0x1000);
                unit->StopNamePlateSpellbar();
            }
        }
    }

    CGGameUI::m_lockedTarget = WOWGUID();

    if (noMgr) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->SetCombatMode();

        if (player->GetPossessedUnit()) {
            CGPetInfo::PetClearTarget();
        }
    }

    if (a2) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_SET_SELECTION));
        msg.Put(CGGameUI::m_lockedTarget.m_guid);
        msg.Finalize();
        ClientServices::Send2(&msg);
        CGSpellBook::UpdateUsable();
        FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_TARGET_CHANGED, nullptr);
    }
}

// VERIFY: 5ed1fb2e manual
// OFFSET: 0x00524650
void CGGameUI::DeleteCursorItem() {
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(CGGameUI::m_cursorItem, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (!item || item->m_item->m_itemFieldOwner != ClntObjMgrGetActivePlayer()) {
        return;
    }

    if (item->GetItemStaticFlags(0) & 0x20) {
        CGGameUI::DisplayError(ERR_ITEM_CANT_BE_DESTROYED);
        return;
    }

    WOWGUID none;
    ItemStats_C* stats = g_itemDBCache.GetRecord(item->m_obj->m_entryID, none, nullptr, nullptr, false);

    if (stats) {
        FrameScript_SignalEvent(SCRIPT_EVENT_DELETE_ITEM_CONFIRM, "%s%d", stats->m_name[0], stats->m_quality);
    }
}

// VERIFY: 07730430 manual
// OFFSET: 0x00524BF0
void CGGameUI::Target(WOWGUID guid) {
    if (!guid) {
        CGGameUI::unk_BD08CC = 0;

        if (CGGameUI::m_lockedTarget) {
            CGGameUI::m_lastTarget = CGGameUI::m_lockedTarget;
            CGGameUI::ClearTarget(CGGameUI::m_lockedTarget, 1);

            if (Spell_C_GetAutoRepeatingSpell()) {
                Spell_C_CancelAutoRepeat(true);
            }
        }

        return;
    }

    if (!ClntObjMgrGetCurrent() || CGGameUI::m_currentlyReloadingUI) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    if (lua_tainted) {
        CGGameUI::ShowBlockedActionFeedback(nullptr, 0);
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->m_unit->m_unitFieldCharmedby) {
        return;
    }

    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (object) {
        if (Spell_C_IsTargeting()) {
            Spell_C_HandleSpriteClick(object, nullptr);
            return;
        }

        if (!object->CanBeTargetted() || guid == CGGameUI::m_lockedTarget) {
            return;
        }

        if (CGGameUI::m_visible || (g_unitHighlightsCVar && g_unitHighlightsCVar->m_intValue)) {
            object->ShowHighlightType(0);
        }

        if (object->m_obj->m_type & TYPEMASK_UNIT) {
            CGUnit_C* unit = static_cast<CGUnit_C*>(object);
            PlayerNameTriggerColorUpdate(unit->m_nameDesc);
            unit->RegisterScript(0x1000);

            SpellRec spell;

            if ((unit->unk_0A6C && g_spellDB.GetRecord(unit->unk_0A6C, &spell)) || (unit->unk_0A78[2] && g_spellDB.GetRecord(unit->unk_0A78[2], &spell))) {
                unit->ProcessCastNotInterruptible(&spell);
            }

            unit->UpdateSpellCastBars(OsGetAsyncTimeMs());

            if (player) {
                if (player->CanAttackNow(unit)) {
                    CGGameUI::m_lastEnemyTarget = guid;
                } else if (player->CanAssist(unit, false)) {
                    CGGameUI::m_lastFriendTarget = guid;
                }

                if ((unit->m_obj->m_type & TYPEMASK_PLAYER) && unit->m_obj->m_guid != ClntObjMgrGetActivePlayer() && CanTriggerTutorial(18)) {
                    CGTutorial::TriggerTutorial(18);
                } else if ((unit->m_obj->m_type & TYPEMASK_PLAYER) && player->CanCooperate(unit) && static_cast<int32_t>(player->m_unit->m_unitFieldLevel) >= 5) {
                    CGTutorial::TriggerTutorial(17);
                } else if (player->CanAttack(unit)) {
                    CGTutorial::TriggerTutorial(4);
                } else if (unit->m_unit->m_unitNpcFlags & 0x2000) {
                    CGTutorial::TriggerTutorial(34);
                } else if (unit->m_questStatus == 1) {
                    CGTutorial::TriggerTutorial(42);
                }
            }
        }
    } else {
        if (guid == CGGameUI::m_lockedTarget) {
            return;
        }

        SI2::PlaySoundKit("igCharacterSelect", nullptr, nullptr, nullptr);
    }

    bool inMelee = player && player->IsInMelee();
    CGGameUI::m_lastTarget = CGGameUI::m_lockedTarget;
    CGGameUI::ClearTarget(CGGameUI::m_lockedTarget, 0);
    CGGameUI::m_lockedTarget = guid;
    CGGameUI::SendTarget(guid);
    CGSpellBook::UpdateUsable();
    FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_TARGET_CHANGED, nullptr);

    if (g_stopAutoAttackOnTargetChangeCVar->m_intValue) {
        if (Spell_C_GetAutoRepeatingSpell()) {
            Spell_C_CancelAutoRepeat(true);
        }
    } else if (inMelee) {
        player->CombatModeEnter(CGGameUI::m_lockedTarget, 0, 0);
    }
}

// VERIFY: fbb342ce manual
// OFFSET: 0x00527360
int32_t CGGameUI::OnTerrainClick(const CTerrainClickEvent& evt) {
    if (evt.m_button == MOUSE_BUTTON_LEFT) {
        if (Spell_C_HandleTerrainClick(evt)) {
            return 1;
        }

        if (evt.m_button == MOUSE_BUTTON_LEFT) {
            if (CGGameUI::m_cursorItem && CGGameUI::m_cursorItemContainer) {
                CGGameUI::DeleteCursorItem();
                return 1;
            }

            if (g_deselectOnClickCVar->m_intValue) {
                CGGameUI::unk_BD08CC = 0;

                if (CGGameUI::m_lockedTarget) {
                    CGGameUI::m_lastTarget = CGGameUI::m_lockedTarget;
                    CGGameUI::ClearTarget(CGGameUI::m_lockedTarget, 1);

                    if (Spell_C_GetAutoRepeatingSpell()) {
                        Spell_C_CancelAutoRepeat(true);
                    }
                }
            }

            return 1;
        }
    }

    CGPlayer_C* mover = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!mover) {
        return 1;
    }

    C3Vector point = evt.m_point;

    if (evt.m_guid) {
        CGObject_C* object = ClntObjMgrObjectPtr(evt.m_guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

        if (!object) {
            return 1;
        }

        point = evt.m_point * object->GetMatrix();
    }

    mover->CTMClickTerrain(mover->m_movement->GetRawPosition(point));
    return 1;
}

// VERIFY: e20bf7fa manual
// OFFSET: 0x005274F0
int32_t CGGameUI::OnSpriteLeftClick(WOWGUID guid) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!object) {
        return 0;
    }

    if ((object->m_obj->m_type & TYPEMASK_PLAYER) && ((CGGameUI::m_cursorItem && CGGameUI::m_cursorItemContainer) || CGGameUI::m_cursorMoney) && !Spell_C_IsTargeting()) {
        int32_t bound = 0;

        if (CGGameUI::m_cursorItem) {
            CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(CGGameUI::m_cursorItem, TYPEMASK_ITEM, __FILE__, __LINE__));

            if (item) {
                if (item->IsBound()) {
                    bound = 1;
                }

                if (item->m_obj->m_type & TYPEMASK_CONTAINER) {
                    CGBag_C* bag = item->GetBag();

                    if (bag) {
                        if (bag->GetNumItems()) {
                            CGGameUI::DisplayError(ERR_TRADE_BAG);
                            goto npc;
                        }

                        if (static_cast<uint32_t>(player->GetBag()->FindItemSlot(item) - 19) <= 3) {
                            CGGameUI::DisplayError(ERR_TRADE_EQUIPPED_BAG);
                            goto npc;
                        }
                    }
                }
            }
        } else if (!CGGameUI::m_cursorMoney) {
            goto npc;
        }

        if (object->m_obj->m_type & TYPEMASK_PLAYER) {
            if (player->IsDeadOrGhost()) {
                CGGameUI::DisplayError(ERR_PLAYER_DEAD);
            } else if (!player->CanCooperate(static_cast<CGUnit_C*>(object))) {
                CGGameUI::DisplayError(ERR_PLAYER_WRONG_FACTION);
            } else if (!CGTradeInfo::m_tradingPlayer) {
                CGTradeInfo::InitiateTrade(guid, 1);
            } else if (CGTradeInfo::m_tradingPlayer == guid) {
                CGTradeInfo::AddCursorItem(bound);
            } else {
                CGGameUI::DisplayError(ERR_ALREADY_TRADING);
            }
        }
    }

npc:
    if (object->m_obj->m_type & TYPEMASK_UNIT) {
        if (guid == CGPetInfo::GetPet(0) && CGGameUI::m_cursorItem) {
            CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(CGGameUI::m_cursorItem, TYPEMASK_ITEM, __FILE__, __LINE__));

            if (item && Spell_C_HandleItemDrop(item, static_cast<CGUnit_C*>(object))) {
                CGGameUI::ClearCursor(1, 1);
            }
        }

        static_cast<CGUnit_C*>(object)->HandleNPCFeedback();
    }

    if ((object->m_obj->m_type & TYPEMASK_GAMEOBJECT) && !object->CanBeTargetted()) {
        WOWGUID item;
        WOWGUID container;
        uint32_t slot;
        CGGameUI::GetCursorItem(item, container, slot);

        if (item && container) {
            CGGameUI::DeleteCursorItem();
        }
    }

    CGGameUI::Target(guid);
    return 1;
}

// VERIFY: 26c1a885 manual
// OFFSET: 0x005216F0
void CGGameUI::DisplayError(UIErrors error, ...) {
    if (error >= ERR_MAX_VALUE) {
        return;
    }

    if (error == ERR_BAG_FULL) {
        CGTutorial::TriggerTutorial(57);
    }

    const UIErrorInfo& info = s_errorInfo[error];

    if (info.m_vocalError == 68) {
        if (SStrCmpI(info.m_sound, "NONE", STORM_MAX_STR)) {
            SI2::PlaySoundKit(s_errorInfo[error].m_sound, nullptr, nullptr, nullptr);
        }
    } else {
        SI2::PlayVocalErrorSound(info.m_vocalError);
    }

    const char* token = s_errorInfo[error].m_token;

    if (!token || !*token) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    const char* text = player ? player->GetFrameScriptText(token, -1) : FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE);

    char buffer[3000];
    va_list args;
    va_start(args, error);
    SStrVPrintf(buffer, sizeof(buffer), text, args);
    va_end(args);
    SStrCopy(CGGameUI::m_lastErrorMessage, buffer, sizeof(CGGameUI::m_lastErrorMessage));

    switch (s_errorInfo[error].m_type) {
        case 0:
            CGChat::AddChatMessage(buffer, s_errorInfo[error].m_chatType, 0, 0, 0, 0, nullptr, WOWGUID(), 0, WOWGUID(), 0, 0, 0);
            break;

        case 1:
            if (buffer[0]) {
                FrameScript_SignalEvent(SCRIPT_EVENT_UI_INFO_MESSAGE, "%s", buffer);
            }

            break;

        case 2:
            if (buffer[0]) {
                FrameScript_SignalEvent(SCRIPT_EVENT_UI_ERROR_MESSAGE, "%s", buffer);
            }

            break;

        case 3:
            ConsoleWriteA(buffer, static_cast<COLOR_T>(3));
            break;

        default:
            break;
    }
}

// VERIFY: 7aa17106 auto
// OFFSET: 0x00513920
uint8_t CGGameUI::GetComboPoints(WOWGUID target) {
    if (!target) {
        target = CGGameUI::m_lockedTarget;
    }

    if (target == CGGameUI::m_comboTarget) {
        return CGGameUI::m_comboPoints;
    }

    return 0;
}

// VERIFY: 8e70952b auto
// OFFSET: 0x00512A00
int32_t CGGameUI::IsPartyMember(const WOWGUID& guid) {
    if (CGPartyInfo::IsMember(guid)) {
        return 1;
    }

    if (IsGuidInMyRaid(guid)) {
        return 1;
    }

    return 0;
}

// STATIC: 0x00BD080C
int32_t CGGameUI::m_areaID;

// STATIC: 0x00BD0890
int32_t CGGameUI::m_bOwnsInstance;

// VERIFY: 2911c4f3 auto
// OFFSET: 0x005138A0
void CGGameUI::ClearInstanceResetMap() {
    CGGameUI::m_uiLastInstanceTime = 0;
    CGGameUI::m_iMapID = 0;
}

// VERIFY: 0a7b477a auto
// OFFSET: 0x005138B0
void UpdateInstanceOwnershipPacket(int32_t owns) {
    CGGameUI::m_bOwnsInstance = owns;
}

// STATIC: 0x00BD0770
int32_t CGGameUI::m_stackSplit;

// VERIFY: 82442556 auto
// OFFSET: 0x00513660
WOWGUID CGGameUI::GetCursorItem() {
    if (CGGameUI::m_cursorItemType == 1) {
        return CGGameUI::m_cursorItem;
    }

    return WOWGUID();
}

// VERIFY: 6cbd8480 auto
// OFFSET: 0x005136C0
int32_t CGGameUI::GetCursorSpell() {
    return CGGameUI::m_cursorSpell;
}

// VERIFY: 6e14b9f3 auto
// OFFSET: 0x005136D0
int32_t CGGameUI::GetCursorVirtualItem() {
    return CGGameUI::m_cursorVirtualID;
}

// VERIFY: a3eaa352 auto
// OFFSET: 0x00520960
void CGGameUI::SetCursorSpell(int32_t spellId) {
    SpellRec spell;

    if (!g_spellDB.GetRecord(spellId, &spell)) {
        return;
    }

    const char* texture = nullptr;

    if (spell.m_effect[0] == 78) {
        texture = CGActionBar::GetAttackTexture();
    } else if ((spell.m_attributes & 0x2) && (spell.m_attributesExB & 0x20)) {
        texture = CGActionBar::GetRangedTexture();
    }

    if (!texture || !*texture) {
        SpellIconRec* icon = g_spellIconDB.GetRecord(spell.m_spellIconID);

        if (icon) {
            texture = icon->m_textureFilename;
        }
    }

    if (texture && *texture) {
        CGGameUI::ClearCursor(1, 1);
        CGGameUI::m_cursorSpell = spellId;
        CGGameUI::m_cursorItemType = 3;
        SI2::PlaySoundKit("INTERFACESOUND_CURSORGRABOBJECT", nullptr, nullptr, nullptr);
        CursorGrabSpell(texture);
        CGActionBar::ShowGrid();
        CGGameUI::m_cursorHasAction = 1;
    }
}

// VERIFY: 4a12ec9b auto
// OFFSET: 0x00520A60
void CGGameUI::DropCursorSpell() {
    if (CGGameUI::m_cursorItemType == 3) {
        CGGameUI::ClearCursor(1, 1);
    }
}

// VERIFY: 9c436377 auto
// OFFSET: 0x00520BE0
void CGGameUI::SetCursorMacro(int32_t macroId) {
    char texture[260];
    CGUIMacros::GetFullIconPath(macroId, texture, sizeof(texture));

    if (texture[0]) {
        CGGameUI::ClearCursor(1, 1);
        CGGameUI::m_cursorMacro = macroId;
        CGGameUI::m_cursorItemType = 8;
        SI2::PlaySoundKit("INTERFACESOUND_CURSORGRABOBJECT", nullptr, nullptr, nullptr);
        CursorGrabSpell(texture);
        CGActionBar::ShowGrid();
        CGGameUI::m_cursorHasAction = 1;
    }
}

// VERIFY: f5e488ca auto
// OFFSET: 0x00520D30
void CGGameUI::SetCursorVirtualItem(int32_t id, int32_t displayId, int32_t slot, int32_t type, int32_t stackSplit) {
    if (!CGGameUI::m_hasControl && type != 7) {
        return;
    }

    CGGameUI::ClearCursor(1, 1);

    if (!id) {
        return;
    }

    CGGameUI::m_cursorVirtualID = id;
    CGGameUI::m_cursorVirtualDisplay = displayId;
    CGGameUI::m_cursorVirtualSlot = slot;
    CGGameUI::m_cursorItemType = type;
    CGGameUI::m_stackSplit = stackSplit;

    if (type == 7) {
        CGActionBar::ShowGrid();
        CGGameUI::m_cursorHasAction = 1;
    }

    CursorGrabVirtualItem(displayId);
    SI2::PlayItemSound(0, displayId);
    FrameScript_SignalEvent(SCRIPT_EVENT_CURSOR_UPDATE, nullptr);
}

// VERIFY: c7528314 auto
// OFFSET: 0x00520DC0
void CGGameUI::SetCursorEquipmentSet(int32_t setId) {
    if (!CGEquipmentManager::GetSetByID(setId)) {
        return;
    }

    char texture[260];
    CGEquipmentManager::GetFullIconPath(setId, texture, sizeof(texture));
    CGGameUI::ClearCursor(1, 1);
    CGGameUI::m_cursorEquipmentSet = setId;
    CGGameUI::m_cursorItemType = 13;
    SI2::PlaySoundKit("INTERFACESOUND_CURSORGRABOBJECT", nullptr, nullptr, nullptr);
    CursorGrabSpell(texture);
    CGActionBar::ShowGrid();
    CGGameUI::m_cursorHasAction = 1;
}

// VERIFY: 1e03cef0 auto
// OFFSET: 0x005844A0
int32_t CGGameUI::GetCursorVirtualItemOfType(int32_t type) {
    if (CGGameUI::m_cursorItemType == type) {
        return CGGameUI::GetCursorVirtualItem();
    }

    return 0;
}

// VERIFY: 248bafd2 auto
// OFFSET: 0x00513880
void CGGameUI::InitClientControlState(const WOWGUID& guid) {
    CGGameUI::m_hasControl = 1;
    CGUnit_C::InitActiveMover(guid);
}

// VERIFY: 556ea2c6 auto
// OFFSET: 0x00520FE0
void CGGameUI::OnClientControlChanged(int32_t control) {
    if (control == CGGameUI::m_hasControl) {
        return;
    }

    CGGameUI::m_hasControl = control;

    if (control) {
        FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_CONTROL_GAINED, nullptr);
    } else {
        CGGameUI::ClearCursor(1, 1);
        Spell_C_CancelPlayerSpells();
        FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_CONTROL_LOST, nullptr);
    }

    CGSpellBook::UpdateUsable();
}

// STATIC: 0x00BD0792
uint8_t CGGameUI::s_inWorld;

// STATIC: 0x00BD08A4
uint32_t CGGameUI::unk_BD08A4;

// STATIC: 0x00BD08B0
WOWGUID CGGameUI::unk_BD08B0;

// STATIC: 0x00BD0818
int32_t CGGameUI::m_corpseMapID;

// STATIC: 0x00BD081C
int32_t CGGameUI::m_corpseActualMapID;

// STATIC: 0x00BD0820
int32_t CGGameUI::unk_BD0820;

// STATIC: 0x00BD0830
WOWGUID CGGameUI::m_corpseTransportGUID;

// STATIC: 0x00BD0844
int32_t CGGameUI::m_deathReleaseMapID;

// STATIC: 0x00BD0A58
C3Vector CGGameUI::m_corpsePos;

// STATIC: 0x00BD0A64
C3Vector CGGameUI::m_deathReleaseLoc;

// VERIFY: 9b8cc5da manual
// OFFSET: 0x00519DF0
void CGGameUI::UpdateThreatWarningEnabled(int32_t value) {
    if (value == 0) {
        CGGameUI::m_threatWarningEnabled = 0;
    } else if (value == 1) {
        MapRec* map = g_mapDB.GetRecord(CGGameUI::m_iCurrentMapID);
        CGGameUI::m_threatWarningEnabled = map && (map->m_instanceType == 1 || map->m_instanceType == 2);
    } else if (value == 2 && !CGPartyInfo::m_members[0] && !CGRaidInfo::m_numMembers) {
        CGGameUI::m_threatWarningEnabled = 0;
    } else {
        CGGameUI::m_threatWarningEnabled = 1;
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_UNIT_THREAT_SITUATION_UPDATE, nullptr);
    FrameScript_SignalEvent(SCRIPT_EVENT_UNIT_THREAT_LIST_UPDATE, nullptr);
}

// VERIFY: 83b33865 manual
// OFFSET: 0x0051F690
void CGGameUI::SetDeathReleaseLocation(int32_t mapId, const C3Vector& position) {
    CGGameUI::m_deathReleaseMapID = mapId;
    CGGameUI::m_deathReleaseLoc = position;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && (mapId == player->GetNewContinentID() || mapId == -1)) {
        MinimapSetSpiritHealerPOI(position.x, position.y);
    }
}

// VERIFY: 322ddfb3 manual
// OFFSET: 0x00523DB0
void CGGameUI::SetCorpseLocation(int32_t mapId, const C3Vector& position, int32_t actualMapId, int32_t transport) {
    CGGameUI::m_corpseMapID = mapId;
    CGGameUI::m_corpsePos = position;
    CGGameUI::m_corpseActualMapID = actualMapId;

    if (transport) {
        CGGameUI::m_corpseTransportGUID.m_guid = static_cast<int64_t>(transport) | 0x1FC0000000000000ULL;
    } else {
        CGGameUI::m_corpseTransportGUID = WOWGUID();
    }

    C3Vector corpse = { 0.0f, 0.0f, 0.0f };
    CGGameUI::GetCorpsePosition(corpse);
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && mapId == player->GetNewContinentID()) {
        MinimapSetCorpsePOI(corpse.x, corpse.y);
    } else {
        MinimapSetCorpsePOI(0.0f, 0.0f);
    }
}

// VERIFY: fc42d909 manual
// OFFSET: 0x00524A30
void CGGameUI::UpdateCorpseLocation() {
    C3Vector position = { 0.0f, 0.0f, 0.0f };
    CGGameUI::SetCorpseLocation(-1, position, -1, 0);

    if (CGGameUI::unk_BD0820) {
        CGGameUI::unk_BD0820 = 0;

        if (CGBattlefieldInfo::m_instanceType != 4) {
            FrameScript_SignalEvent(SCRIPT_EVENT_CORPSE_OUT_OF_RANGE, nullptr);
        }
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && (player->m_player->m_playerFlags & 0x10) && CGBattlefieldInfo::m_instanceType != 4) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(MSG_CORPSE_QUERY));
        msg.Finalize();
        ClientServices::Send2(&msg);
    }
}

// VERIFY: c2b6eb19 manual
// OFFSET: 0x00528010
void CGGameUI::EnterWorld() {
    if (CGGameUI::s_inWorld) {
        return;
    }

    CGGameUI::s_inWorld = 1;
    CGChat::LeaveWorld();
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    CM2Model* model = player ? player->GetObjectModel() : nullptr;

    if (!model) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    model->IsLoaded(1, 1);
    CCharacterComponent* component = player->m_characterComponent;

    if (component) {
        component->RenderPrep(1);

        if (component->m_flags & 0x40) {
            player->TurnOffGuildTabardPurchase();
        }
    }

    if (player->GetFarSightGuid() && (player->m_combatModeFlags & 0x1)) {
        WOWGUID farSight = { player->GetFarSightGuid() };
        CGGameUI::ResetCamera(farSight);
    } else {
        CGGameUI::ResetCamera(WOWGUID());
    }

    CGInputControl::GetActive()->Reset();
    FrameScript_MemoryCleanup();

    if (!CGGameUI::m_currentlyReloadingUI) {
        for (uint32_t i = 0; i < sizeof(CGGameUI::m_totems) / sizeof(CGGameUI::m_totems[0]); i++) {
            CGGameUI::m_totems[i].m_guid = WOWGUID();
            CGGameUI::m_totems[i].m_name = nullptr;
            CGGameUI::m_totems[i].unk_0014 = 0;
            CGGameUI::m_totems[i].unk_0018 = 0;
            CGGameUI::m_totems[i].unk_001C = 0;
        }
    }

    CGGameUI::unk_BD08B0 = WOWGUID();
    CGGameUI::unk_BD08A4 = 0;

    if (CGGameUI::m_loggingIn) {
        CGGameUI::m_loggingIn = 0;
        FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_LOGIN, nullptr);
        CGLCD::Login();
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_ENTERING_WORLD, nullptr);
    CGMinimapFrame::EnterWorld();
    CGChat::EnterWorld();
    CGActionBar::EnterWorld();
    CGCharacterInfo::EnterWorld();
    CGContainerInfo::EnterWorld();
    CGQuestInfo::EnterWorld();
    CGQuestLog::EnterWorld();
    CGClassTrainer::EnterWorld();
    CGMerchantInfo::EnterWorld();
    CGTradeInfo::EnterWorld();
    CGBankInfo::EnterWorld();
    SendRequestPetInfo();
    CGWorldMap::EnterWorld();
    CGReputationInfo::EnterWorld();
    CGTabardCreationFrame::EnterWorld();
    CGGuildRegistrar::EnterWorld();
    CGPetitionInfo::EnterWorld();
    PlayerSignalSkillStatChanges();
    CGMailInfo::EnterWorld();
    CGBattlefieldInfo::EnterWorld();
    CGTalentInfo::EnterWorld();
    CGTalentInfo_Inspect::EnterWorld();
    CGAuctionHouse::EnterWorld();
    CGStableInfo::EnterWorld();
    CGRaidInfo::EnterWorld();
    CGItemSocketInfo::EnterWorld();
    CGPetitionVendorInfo::EnterWorld();
    CGLCD::EnterWorld();
    CGLookingForGroup::EnterWorld();
    CGArenaTeamInfo::EnterWorld();
    g_capturePointUIManager.unk_000C = OsGetAsyncTimeMs();
    CGGuildBankInfo::EnterWorld();
    CGGuildInfo::EnterWorld();
    CGCalendar::EnterWorld();
    CGBarberShop::EnterWorld();
    CGRuneInfo::InitializeGame();
    CGCurrencyTypes::EnterWorld();
    CVehicle_C::EnterWorld();
    CGEquipmentManager::EnterWorld();
    CGGMTicketInfo::EnterWorld();
    CGInstanceEncounter_C::EnterWorld();

    WOWGUID lockedTarget = CGGameUI::m_lockedTarget;

    if (lockedTarget) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_SET_SELECTION));
        msg.Put(lockedTarget.m_guid);
        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    if (player->m_unit->m_unitFieldFlags & 0x80000) {
        FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_REGEN_DISABLED, nullptr);
        CGGameUI::m_simpleTop->m_allowProtectedActions = 0;
        CGGameUI::UpdateCombatMode();
    }

    CGGameUI::UpdateCorpseLocation();
    C3Vector releaseLoc = { 0.0f, 0.0f, 0.0f };
    CGGameUI::SetDeathReleaseLocation(-1, releaseLoc);
    CGTutorial::TriggerTutorial(41);
    CGTutorial::TriggerTutorial(1);
    CGTutorial::TriggerTutorial(2);
    CGTutorial::TriggerTutorial(0);
    CGGameUI::UpdateThreatWarningEnabled(g_threatWarningCVar->m_intValue);
    CGChat::Resume();

    if (CGGameUI::m_inCinematic) {
        EnableFadingScreen(0.25f, CGGameUI::BeginCinematicInternal, nullptr);
    }

    MapRec* map = g_mapDB.GetRecord(CGGameUI::m_iCurrentMapID);

    if (map && (map->m_instanceType == 3 || map->m_instanceType == 4)) {
        FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_ENTERING_BATTLEGROUND, nullptr);
    }

    if (player->m_unit->m_unitFieldHealth <= 0) {
        player->HandleRepopRequest(true);
    }

    int32_t voiceChat = s_cvarEnableVoiceChat ? s_cvarEnableVoiceChat->m_intValue : 0;
    int32_t microphone = s_cvarEnableMicrophone ? s_cvarEnableMicrophone->m_intValue : 0;

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_VOICE_SESSION_ENABLE));
    msg.Put(static_cast<uint8_t>(voiceChat != 0));
    msg.Put(static_cast<uint8_t>(microphone != 0));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// VERIFY: 719a57ef auto
// OFFSET: 0x00512B50
void CGGameUI::UnitModelUpdate(const WOWGUID& guid, uint32_t flags) {
    UpdateModelForUI(guid, flags);
}


// STATIC: 0x00AC8074
static const char* s_instanceTypeTokens[5] = { "none", "party", "raid", "pvp", "arena" };

// STATIC: 0x00BD077C
const char* CGGameUI::m_minimapZoneText;

// STATIC: 0x00BD0784
const char* CGGameUI::m_subZoneText;

// STATIC: 0x00BD0810
int32_t CGGameUI::m_subzoneID;

// VERIFY: c6c85ff5 auto
// OFFSET: 0x005155D0
int32_t Script_GetSubZoneText(lua_State* L) {
    lua_pushstring(L, CGGameUI::m_subZoneText ? CGGameUI::m_subZoneText : "");
    return 1;
}

// VERIFY: 88423b87 auto
// OFFSET: 0x00515600
int32_t Script_GetMinimapZoneText(lua_State* L) {
    lua_pushstring(L, CGGameUI::m_minimapZoneText ? CGGameUI::m_minimapZoneText : "");
    return 1;
}

// VERIFY: db7fa982 auto
// OFFSET: 0x005156A0
int32_t Script_IsInInstance(lua_State* L) {
    MapRec* map = g_mapDB.GetRecord(CGGameUI::m_iCurrentMapID);

    if (!map) {
        lua_pushnil(L);
        lua_pushstring(L, s_instanceTypeTokens[0]);
        return 2;
    }

    if (map->m_instanceType) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    uint32_t type = map->m_instanceType;
    lua_pushstring(L, type < sizeof(s_instanceTypeTokens) / sizeof(s_instanceTypeTokens[0]) ? s_instanceTypeTokens[type] : s_instanceTypeTokens[0]);
    return 2;
}

// VERIFY: 0120d5f2 manual
// OFFSET: 0x00516BF0
int32_t Script_GetMouseFocus(lua_State* L) {
    if (!CSimpleTop::s_instance) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    CSimpleFrame* focus = CSimpleTop::s_instance->m_mouseFocus;

    if (!focus) {
        lua_pushnil(L);
        return 1;
    }

    if (!focus->m_luaRegistered) {
        focus->RegisterScriptObject(nullptr);
    }

    lua_rawgeti(L, LUA_REGISTRYINDEX, focus->m_luaObjectRef);
    return 1;
}

// VERIFY: 734ee317 manual
// OFFSET: 0x0051BA50
int32_t Script_GetZonePVPInfo(lua_State* L) {
    int32_t flags = -1;
    int32_t subzone = 0;
    AreaTableRec* subzoneRec = g_areaTableDB.GetRecord(CGGameUI::m_subzoneID);

    if (subzoneRec) {
        flags = subzoneRec->m_flags;
        subzone = 1;
    }

    AreaTableRec* area = g_areaTableDB.GetRecord(CGGameUI::m_areaID);

    if (flags == -1) {
        subzone = 0;
        flags = area ? area->m_flags : 0;
    }

    const char* type = nullptr;

    if (flags & 0x800) {
        type = "sanctuary";
    } else if (flags & 0x80) {
        type = "arena";
    } else if (flags & 0x1000000) {
        type = "combat";
    }

    if (type) {
        lua_pushstring(L, type);

        if (subzone) {
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnil(L);
        }

        return 2;
    }

    int32_t contested = (flags & 0x40000) != 0;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!area || !player || (!s_isPvPRealm && !(area->m_flags & 0x10))) {
        return 0;
    }

    if (!contested) {
        FactionTemplateRec* faction = g_factionTemplateDB.GetRecord(player->m_unit->m_unitFieldFactiontemplate);

        if (!faction || (area->m_factionGroupMask & faction->m_friendGroup)) {
            type = "friendly";
        } else if (area->m_factionGroupMask & faction->m_enemyGroup) {
            type = "hostile";
        }
    }

    lua_pushstring(L, type ? type : "contested");
    const char* factionName = "";

    for (int32_t i = 0; i < g_factionGroupDB.m_numRecords; i++) {
        FactionGroupRec* group = i >= 0 && i < g_factionGroupDB.m_numRecords ? &g_factionGroupDB.m_records[i] : nullptr;

        if (((1 << group->m_maskID) & area->m_factionGroupMask) && *group->m_name) {
            factionName = group->m_name;
            break;
        }
    }

    lua_pushnil(L);
    lua_pushstring(L, factionName);
    return 3;
}

// VERIFY: e54b0005 auto
// OFFSET: 0x0051FA50
int32_t CGGameUI::FilterMouseDown(CMouseEvent* evt) {
    if (evt->m_id == 0x400500C8 && evt->m_data.m_button == 4) {
        if (CGGameUI::m_cursorItem || CGGameUI::m_cursorVirtualID || CGGameUI::m_cursorMoney || CGGameUI::m_cursorSpell || CGGameUI::m_cursorPetAction || CGGameUI::m_cursorMacro || CGGameUI::m_cursorPet || CGPlayer_C::IsGiftWrapping()) {
            CGGameUI::ClearCursor(1, 1);
            return 1;
        }
    }

    if (evt->m_data.m_mode != 1) {
        return 0;
    }

    if (evt->m_id == 0x400500C8) {
        CGWorldFrame::s_currentWorldFrame->OnLayerMouseDown(*evt, nullptr);
    } else {
        CGWorldFrame::s_currentWorldFrame->OnLayerMouseUp(*evt, nullptr);
    }

    return 1;
}

// VERIFY: 27d36d58 auto
// OFFSET: 0x0051FB00
int32_t CGGameUI::HandleMouseDown(const CMouseEvent& evt) {
    if (evt.m_data.m_button != 4) {
        return 0;
    }

    if (Spell_C_IsTargeting()) {
        if (Spell_C_IsCursorWorldObjectHousing()) {
            Spell_C_CursorWorldObjectRotate();
        } else {
            Spell_C_StopTargeting();
        }
    }

    if (CGPlayer_C::IsGiftWrapping()) {
        CGGameUI::ClearCursor(1, 1);
    }

    if (CursorGetResetMode() == 17) {
        CursorSetResetMode(1);
        CursorSetMode(1);
    }

    return 0;
}
