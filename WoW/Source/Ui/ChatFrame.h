#ifndef WOW_SOURCE_UI_CHATFRAME_H
#define WOW_SOURCE_UI_CHATFRAME_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/WGUID.h"

struct lua_State;
class CDataStore;
class CGPlayer_C;

enum SLASH_COMMAND_ID {
    CHAT_MSG_SYSTEM = 0,
    CHAT_MSG_SAY = 1,
    CHAT_MSG_PARTY = 2,
    CHAT_MSG_RAID = 3,
    CHAT_MSG_GUILD = 4,
    CHAT_MSG_OFFICER = 5,
    CHAT_MSG_YELL = 6,
    CHAT_MSG_WHISPER = 7,
    CHAT_MSG_WHISPER_FOREIGN = 8,
    CHAT_MSG_WHISPER_INFORM = 9,
    CHAT_MSG_EMOTE = 10,
    CHAT_MSG_TEXT_EMOTE = 11,
    CHAT_MSG_MONSTER_SAY = 12,
    CHAT_MSG_MONSTER_PARTY = 13,
    CHAT_MSG_MONSTER_YELL = 14,
    CHAT_MSG_MONSTER_WHISPER = 15,
    CHAT_MSG_MONSTER_EMOTE = 16,
    CHAT_MSG_CHANNEL = 17,
    CHAT_MSG_CHANNEL_JOIN = 18,
    CHAT_MSG_CHANNEL_LEAVE = 19,
    CHAT_MSG_CHANNEL_LIST = 20,
    CHAT_MSG_CHANNEL_NOTICE = 21,
    CHAT_MSG_CHANNEL_NOTICE_USER = 22,
    CHAT_MSG_AFK = 23,
    CHAT_MSG_DND = 24,
    CHAT_MSG_IGNORED = 25,
    CHAT_MSG_SKILL = 26,
    CHAT_MSG_LOOT = 27,
    CHAT_MSG_MONEY = 28,
    CHAT_MSG_OPENING = 29,
    CHAT_MSG_TRADESKILLS = 30,
    CHAT_MSG_PET_INFO = 31,
    CHAT_MSG_COMBAT_MISC_INFO = 32,
    CHAT_MSG_COMBAT_XP_GAIN = 33,
    CHAT_MSG_COMBAT_HONOR_GAIN = 34,
    CHAT_MSG_COMBAT_FACTION_CHANGE = 35,
    CHAT_MSG_BG_SYSTEM_NEUTRAL = 36,
    CHAT_MSG_BG_SYSTEM_ALLIANCE = 37,
    CHAT_MSG_BG_SYSTEM_HORDE = 38,
    CHAT_MSG_RAID_LEADER = 39,
    CHAT_MSG_RAID_WARNING = 40,
    CHAT_MSG_RAID_BOSS_EMOTE = 41,
    CHAT_MSG_RAID_BOSS_WHISPER = 42,
    CHAT_MSG_FILTERED = 43,
    CHAT_MSG_BATTLEGROUND = 44,
    CHAT_MSG_BATTLEGROUND_LEADER = 45,
    CHAT_MSG_RESTRICTED = 46,
    CHAT_MSG_BATTLENET = 47,
    CHAT_MSG_ACHIEVEMENT = 48,
    CHAT_MSG_GUILD_ACHIEVEMENT = 49,
    CHAT_MSG_ARENA_POINTS = 50,
    CHAT_MSG_PARTY_LEADER = 51,
    CHAT_MSG_TARGETICONS = 52,
    CHAT_MSG_BN_WHISPER = 53,
    CHAT_MSG_BN_WHISPER_INFORM = 54,
    CHAT_MSG_BN_CONVERSATION = 55,
    CHAT_MSG_BN_CONVERSATION_NOTICE = 56,
    CHAT_MSG_BN_CONVERSATION_LIST = 57,
    CHAT_MSG_BN_INLINE_TOAST_ALERT = 58,
    CHAT_MSG_BN_INLINE_TOAST_BROADCAST = 59,
    CHAT_MSG_BN_INLINE_TOAST_BROADCAST_INFORM = 60,
    CHAT_MSG_BN_INLINE_TOAST_CONVERSATION = 61,
    CHAT_MSG_MAX = 62,
};

struct ChatConversation {
    /* 0000 */ uint8_t m_index;
    /* 0004 */ uint32_t m_presenceID;
    /* 0008 */ uint8_t m_inactive;
};

struct PENDINGCHAT {
    /* 0000 */ TSLink<PENDINGCHAT> m_link;
    /* 0008 */ int32_t m_type;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ WOWGUID m_senderGuid;
    /* 0018 */ uint32_t m_senderUnk;
    /* 001C */ char* m_text;
    /* 0020 */ char* m_senderName;
    /* 0024 */ char* m_targetName;
    /* 0028 */ uint32_t m_language;
    /* 002C */ int32_t m_queued;
    /* 0030 */ int32_t unk_0030;
    /* 0034 */ char m_channelName[128];
    /* 00B4 */ uint32_t unk_00B4;
    /* 00B8 */ WOWGUID m_targetGuid;
    /* 00C0 */ char m_flags[5];
    /* 00C8 */ int32_t m_achievementID;
    /* 00CC */ bool m_isGM;
    /* 00D0 */ ChatConversation* m_conversation;
    /* 00D4 */ uint32_t unk_00D4;

    PENDINGCHAT() : m_text(nullptr), m_senderName(nullptr), m_conversation(nullptr) {
    }

    ~PENDINGCHAT();
};

struct THROTTLEDCHAT {
    /* 0000 */ TSLink<THROTTLEDCHAT> m_link;
    /* 0008 */ int32_t m_type;
    /* 000C */ uint32_t m_language;
    /* 0010 */ char* m_target;
    /* 0014 */ char* m_text;

    ~THROTTLEDCHAT();
};

class CHATTHROTTLE : public TSHashObject<CHATTHROTTLE, HASHKEY_STR> {
    public:
    /* 0018 */ uint32_t m_index;
    /* 001C */ uint32_t m_times[10];
};

class SPAMCHECK : public TSHashObject<SPAMCHECK, HASHKEY_STR> {
    public:
    /* 0018 */ bool m_filtered;
    /* 001C */ char* m_filteredText;
};

class PROFANITYCHECK : public TSHashObject<PROFANITYCHECK, HASHKEY_STR> {
    public:
    /* 0018 */ bool m_filtered;
    /* 001C */ char* m_filteredText;
};

struct PENDINGUSERLIST : public TSLinkedNode<PENDINGUSERLIST> {
    /* 0008 */ WOWGUID m_guid;
    /* 0010 */ uint8_t m_flags;
    /* 0014 */ uint32_t unk_0014;
};

class ChatChannel {
    public:
    /* 0000 */ int32_t m_channelNumber;
    /* 0004 */ char m_name[128];
    /* 0084 */ TSExplicitList<PENDINGUSERLIST, 0> m_pendingUsers;
    /* 0090 */ uint8_t m_flags;
    /* 0094 */ uint32_t m_zoneChannelID;
    /* 0098 */ uint32_t m_unk98;
    /* 009C */ int32_t m_unk9C;
    /* 00A0 */ int32_t m_unkA0;
    /* 00A4 */ uint8_t m_unkA4;
    /* 00A5 */ uint8_t m_unkA5;
};

struct DisplayChannel {
    /* 0000 */ int32_t m_channelIndex;
    /* 0004 */ int32_t m_headerIndex;
    /* 0008 */ int32_t m_type;
    /* 000C */ int32_t m_collapsed;
    /* 0010 */ int32_t m_enabled;
    /* 0014 */ int32_t m_voiceEnabled;
    /* 0018 */ uint32_t m_flags;
    /* 001C */ int32_t m_count;
};

struct PENDINGTEXTEMOTE {
    /* 0000 */ TSLink<PENDINGTEXTEMOTE> m_link;
    /* 0008 */ WOWGUID m_guid;
    /* 0010 */ uint32_t m_emote;
    /* 0014 */ char* m_target;
    /* 0018 */ int32_t m_queued;
    /* 001C */ uint32_t m_emoteNum;

    PENDINGTEXTEMOTE() : m_target(nullptr) {
    }

    ~PENDINGTEXTEMOTE();
};

struct HASHKEY_LANGUAGE {
    /* 0000 */ uint32_t m_length;
    /* 0004 */ uint32_t m_unk04;

    bool operator==(const HASHKEY_LANGUAGE& key) const {
        return this->m_length == key.m_length && this->m_unk04 == key.m_unk04;
    }
};

class LanguageWordsRec;

struct WORDLIST : public TSHashObject<WORDLIST, HASHKEY_LANGUAGE> {
    /* 001C */ TSGrowableArray<const LanguageWordsRec*> m_words;
};

struct ChatMessageGroup {
    /* 0000 */ const char* m_name;
    /* 0004 */ bool m_default;
    /* 0008 */ int32_t m_type;
};

class ChatWindowInfo {
    public:
    /* 0000 */ char m_name[128];
    /* 0080 */ uint8_t m_messageGroups[46];
    /* 00B0 */ TSGrowableArray<uint32_t> m_unkB0;
    /* 00C0 */ TSGrowableArray<int32_t> m_unkC0;
    /* 00D0 */ uint32_t m_unkD0;
    /* 00D4 */ CImVector m_color;
    /* 00D8 */ uint32_t m_unkD8;
    /* 00DC */ uint32_t m_docked;
    /* 00E0 */ uint32_t m_shown;
    /* 00E4 */ uint32_t m_unkE4;
    /* 00E8 */ uint32_t m_unkE8;
    /* 00EC */ float m_unkEC;
    /* 00F0 */ float m_unkF0;
    /* 00F4 */ uint8_t m_unkF4;
    /* 00F8 */ float m_width;
    /* 00FC */ float m_height;

    void Init();
};

struct ChatRecord {
    /* 0000 */ WOWGUID m_senderGuid;
    /* 0008 */ uint32_t m_senderUnk;
    /* 000C */ char m_senderName[48];
    /* 003C */ char m_description[3000];
    /* 0BF4 */ char m_text[3000];
    /* 17AC */ int32_t m_type;
    /* 17B0 */ uint32_t m_unk17B0;
    /* 17B4 */ uint32_t m_lineID;
    /* 17B8 */ uint32_t m_time;
    /* 17BC */ uint32_t unk_17BC;
};

struct ChatColorInfo {
    /* 0000 */ char m_name[64];
    /* 0040 */ uint8_t m_color[4];
    /* 0044 */ int32_t m_unk44;
    /* 0048 */ uint8_t m_unk48;
};

class CGChat {
    public:
    static void ProcessPendingChatQueue();
    static void LogReactionChanged(int32_t faction, int32_t reaction);
    static void LogFactionChanged(int32_t faction, int32_t delta, int32_t generic, float bonus);
    static void EnterWorld();
    static void InitializeGame();
    static void LoadChatSettings();
    static int32_t IsChatLogging(int32_t type);
    static void AddCombatMessage(const char* text);
    static int32_t m_afk;
    static int32_t m_paused;
    static int32_t m_voiceChatSystemStatus;
    static ChatRecord m_chatHistory[60];
    static uint32_t m_currentChatIndex;
    static WOWGUID m_whisperDstHistory[128];
    static WOWGUID m_whisperSrcHistory[16];
    static TSExplicitList<THROTTLEDCHAT, 0> s_throttledChat;
    static TSExplicitList<PENDINGCHAT, 0> s_pendingChat;
    static TSExplicitList<PENDINGTEXTEMOTE, 0> s_pendingTextEmotes;

    static void AddChatMessage(const char* text, int32_t type, const char* senderName, uint32_t language, const char* channelName, const char* targetName, const char* flags, WOWGUID senderGuid, uint32_t senderUnk, WOWGUID targetGuid, int32_t achievementID, bool isGM, const ChatConversation* conversation);
    static int32_t OnMessageChat(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnGMMessageChat(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t ChatMessageReceived(CDataStore* msg, bool isGM);
    static void RecordWhisperSource(WOWGUID guid);
    static void RecordWhisperDest(WOWGUID guid);
    static int32_t IsInWhisperHistory(WOWGUID guid);
    static int32_t IsInWhisperSourceHistory(const char* name);
    static void SendIgnoredResponse(WOWGUID guid, uint8_t a2);
    static TSGrowableArray<ChatChannel> s_channels;
    static int32_t m_loaded;
    static int32_t m_registered;
    static int32_t m_complaintsSystemStatus;
    static DisplayChannel s_displayChannels[20];
    static char s_activeDisplayChannelName[128];
    static int32_t OnChannelNotify(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnChannelList(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnTextEmote(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnZoneUnderAttack(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnDefenseMessage(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnServerMessage(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnRaidInstanceInfo(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnRaidInstanceMessage(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnInstanceReset(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnInstanceResetFailed(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnUpdateInstance(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnUpdateInstanceOwnership(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnCheckReloadSpamFilter(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnTitleEarned(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnResetFailedNotify(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnLogXPGain(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnDurabilityDamageDeath(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnChannelMemberCount(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnComsatReconnectAttempt(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnComsatDisconnect(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnComsatConnectFail(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnVoiceStatusChanged(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnUserListAdded(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnUserListRemove(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnUserListUpdate(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnVoicesessionFull(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnServerFirstAchievement(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t GetChannelID(const char* name);
    static ChatChannel* GetChannelByChatChannelID(uint32_t id);
    static void JoinedChannel(uint32_t id, const char* name, uint32_t a3, uint8_t flags);
    static void LeftChannel(uint32_t id, const char* name);
    static void ResetChannel(uint32_t id, const char* name);
    static void OnChannelsUpdated();
    static void UpdateLanguages();
    static int32_t OnDisplayChannelList(CDataStore* msg);
    static void DisplayPendingUserList(ChatChannel* channel);
    static void AddTextEmoteMessage(const WOWGUID& guid, uint32_t emote, const char* target, uint32_t emoteNum);
    static void QueueTextEmote(const WOWGUID& guid, uint32_t emote, const char* target, int32_t queued, uint32_t emoteNum);
    static void TextEmoteNameQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
    static void HandleFlagsChanged(WOWGUID guid, uint8_t oldFlags, uint8_t newFlags, const char* channel);
    static void InviteNameQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
    static int32_t IsRecordedType(int32_t type);
    static void RecordChat(const char* text, int32_t type, const char* senderName, WOWGUID senderGuid, uint32_t senderUnk, uint32_t a6, const char* channelName, uint32_t lineID, const CGPlayer_C* player);
    static void QueueChatText(const char* text, int32_t type, const char* senderName, const char* targetName, uint32_t language, const char* channelName, const char* flags, int32_t queued, int32_t a9, WOWGUID senderGuid, uint32_t senderUnk, WOWGUID targetGuid, int32_t achievementID, bool isGM, const ChatConversation* conversation);
    static void NameQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
    static void TranslateMessage(uint32_t language, uint32_t skill, const char* text, char* buffer, uint32_t size, int32_t a6, int32_t a7);
    static void AddWhisperThrottled(uint32_t time);
    static int32_t IsWhisperThrottled(uint32_t time);
    static int32_t IsChannelThrottled(const char* channel, uint32_t time);
    static void AddChannelThrottled(const char* channel, uint32_t time);
    static void AddThrottledChat(int32_t type, uint32_t language, const char* target, const char* text);
    static int32_t GetActiveVoiceSession();
    static void SetGuildRecruitmentMode(int32_t mode, int32_t a2);
    static const char* GetChannelStringForDisplay(int32_t session, int32_t* type);
    static int32_t IsPaused();
    static int32_t IsVoiceChatAllowed();
    static void ProcessThrottledChat();
    static void LeaveWorld();
    static void Resume();
    static void SetActiveVoiceSession(const char* channel, int32_t type, int32_t index);
    static void UpdateGuildStatus();
};

int32_t StringToChatType(const char* string, int32_t* type);
int32_t StringToLanguage(const char* string, uint32_t* language);
ChatChannel* GetChannelByName(const char* name);
bool ProfanityFilter(char* text, bool cache, bool force);
void SetCurrentActiveDisplayChannel(const char* name, int32_t type);
uint32_t BuildPartyDisplayList();
uint32_t BuildRaidDisplayList();
void RebuildDisplayChannelList(int32_t a1);
bool SpamFilter(char* text, bool cache);
bool ChatParserParseText(const char* text, char* buffer, uint32_t size);
bool InebriatedParseText(const char* text, char* buffer, uint32_t size, float drunkness);
int32_t QuestParserParseText(const char* text, char* buffer, uint32_t size, const WOWGUID& guid, const char* name, int32_t a6);

int32_t Script_SendChatMessage(lua_State* L);
int32_t Script_SendAddonMessage(lua_State* L);
int32_t Script_SendSystemMessage(lua_State* L);
int32_t Script_GetNumLanguages(lua_State* L);
int32_t Script_GetLanguageByIndex(lua_State* L);
int32_t Script_GetDefaultLanguage(lua_State* L);
int32_t Script_DoEmote(lua_State* L);
int32_t Script_LoggingChat(lua_State* L);
int32_t Script_LoggingCombat(lua_State* L);
int32_t Script_JoinChannelByName(lua_State* L);
int32_t Script_JoinTemporaryChannel(lua_State* L);
int32_t Script_JoinPermanentChannel(lua_State* L);
int32_t Script_LeaveChannelByName(lua_State* L);
int32_t Script_ListChannelByName(lua_State* L);
int32_t Script_ListChannels(lua_State* L);
int32_t Script_GetChannelList(lua_State* L);
int32_t Script_SetChannelPassword(lua_State* L);
int32_t Script_SetChannelOwner(lua_State* L);
int32_t Script_DisplayChannelOwner(lua_State* L);
int32_t Script_GetChannelName(lua_State* L);
int32_t Script_ChannelModerator(lua_State* L);
int32_t Script_ChannelUnmoderator(lua_State* L);
int32_t Script_ChannelMute(lua_State* L);
int32_t Script_ChannelUnmute(lua_State* L);
int32_t Script_ChannelInvite(lua_State* L);
int32_t Script_ChannelKick(lua_State* L);
int32_t Script_ChannelBan(lua_State* L);
int32_t Script_ChannelUnban(lua_State* L);
int32_t Script_ChannelToggleAnnouncements(lua_State* L);
int32_t Script_ChannelSilenceVoice(lua_State* L);
int32_t Script_ChannelSilenceAll(lua_State* L);
int32_t Script_ChannelUnSilenceVoice(lua_State* L);
int32_t Script_ChannelUnSilenceAll(lua_State* L);
int32_t Script_ChangeChatColor(lua_State* L);
int32_t Script_ResetChatColors(lua_State* L);
int32_t Script_GetChatTypeIndex(lua_State* L);
int32_t Script_GetChatWindowInfo(lua_State* L);
int32_t Script_GetChatWindowSavedPosition(lua_State* L);
int32_t Script_GetChatWindowSavedDimensions(lua_State* L);
int32_t Script_GetChatWindowMessages(lua_State* L);
int32_t Script_GetChatWindowChannels(lua_State* L);
int32_t Script_AddChatWindowMessages(lua_State* L);
int32_t Script_RemoveChatWindowMessages(lua_State* L);
int32_t Script_AddChatWindowChannel(lua_State* L);
int32_t Script_RemoveChatWindowChannel(lua_State* L);
int32_t Script_SetChatWindowName(lua_State* L);
int32_t Script_SetChatWindowSize(lua_State* L);
int32_t Script_SetChatWindowColor(lua_State* L);
int32_t Script_SetChatWindowAlpha(lua_State* L);
int32_t Script_SetChatWindowLocked(lua_State* L);
int32_t Script_SetChatWindowDocked(lua_State* L);
int32_t Script_SetChatWindowUninteractable(lua_State* L);
int32_t Script_SetChatWindowShown(lua_State* L);
int32_t Script_SetChatWindowSavedPosition(lua_State* L);
int32_t Script_SetChatWindowSavedDimensions(lua_State* L);
int32_t Script_EnumerateServerChannels(lua_State* L);
int32_t Script_RequestRaidInfo(lua_State* L);
int32_t Script_GetNumSavedInstances(lua_State* L);
int32_t Script_GetSavedInstanceInfo(lua_State* L);
int32_t Script_SetSavedInstanceExtend(lua_State* L);
int32_t Script_ResetChatWindows(lua_State* L);
int32_t Script_CanComplainChat(lua_State* L);
int32_t Script_ComplainChat(lua_State* L);
int32_t Script_GetNumDisplayChannels(lua_State* L);
int32_t Script_GetChannelDisplayInfo(lua_State* L);
int32_t Script_GetSelectedDisplayChannel(lua_State* L);
int32_t Script_SetSelectedDisplayChannel(lua_State* L);
int32_t Script_GetChannelRosterInfo(lua_State* L);
int32_t Script_GetNumChannelMembers(lua_State* L);
int32_t Script_SetActiveVoiceChannel(lua_State* L);
int32_t Script_GetActiveVoiceChannel(lua_State* L);
int32_t Script_CollapseChannelHeader(lua_State* L);
int32_t Script_ExpandChannelHeader(lua_State* L);
int32_t Script_ChannelVoiceOn(lua_State* L);
int32_t Script_ChannelVoiceOff(lua_State* L);
int32_t Script_DisplayChannelVoiceOn(lua_State* L);
int32_t Script_DisplayChannelVoiceOff(lua_State* L);
int32_t Script_IsDisplayChannelOwner(lua_State* L);
int32_t Script_IsDisplayChannelModerator(lua_State* L);
int32_t Script_IsVoiceChatEnabled(lua_State* L);
int32_t Script_IsVoiceChatAllowed(lua_State* L);
int32_t Script_IsVoiceChatAllowedByServer(lua_State* L);
int32_t Script_IsSilenced(lua_State* L);
int32_t Script_GetMuteStatus(lua_State* L);
int32_t Script_UnitIsSilenced(lua_State* L);
int32_t Script_SetChannelWatch(lua_State* L);
int32_t Script_ClearChannelWatch(lua_State* L);
int32_t Script_DeclineInvite(lua_State* L);
int32_t Script_SetChatColorNameByClass(lua_State* L);
void ChatRegisterScriptFunctions();

#endif
