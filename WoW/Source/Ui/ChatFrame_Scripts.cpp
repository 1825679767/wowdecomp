#include "ChatFrame.h"
#include <cstdint>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "WoW/Source/Game/ValidateName.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/Types.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/Ui/RaidInfo.h"
#include "WoW/Source/Ui/Tutorial.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00AC7A58
static FrameScript_Method s_ScriptFunctions[] = {
    { "SendChatMessage", &Script_SendChatMessage },
    { "SendAddonMessage", &Script_SendAddonMessage },
    { "SendSystemMessage", &Script_SendSystemMessage },
    { "GetNumLanguages", &Script_GetNumLanguages },
    { "GetLanguageByIndex", &Script_GetLanguageByIndex },
    { "GetDefaultLanguage", &Script_GetDefaultLanguage },
    { "DoEmote", &Script_DoEmote },
    { "LoggingChat", &Script_LoggingChat },
    { "LoggingCombat", &Script_LoggingCombat },
    { "JoinChannelByName", &Script_JoinChannelByName },
    { "JoinTemporaryChannel", &Script_JoinTemporaryChannel },
    { "JoinPermanentChannel", &Script_JoinPermanentChannel },
    { "LeaveChannelByName", &Script_LeaveChannelByName },
    { "ListChannelByName", &Script_ListChannelByName },
    { "ListChannels", &Script_ListChannels },
    { "GetChannelList", &Script_GetChannelList },
    { "SetChannelPassword", &Script_SetChannelPassword },
    { "SetChannelOwner", &Script_SetChannelOwner },
    { "DisplayChannelOwner", &Script_DisplayChannelOwner },
    { "GetChannelName", &Script_GetChannelName },
    { "ChannelModerator", &Script_ChannelModerator },
    { "ChannelUnmoderator", &Script_ChannelUnmoderator },
    { "ChannelMute", &Script_ChannelMute },
    { "ChannelUnmute", &Script_ChannelUnmute },
    { "ChannelInvite", &Script_ChannelInvite },
    { "ChannelKick", &Script_ChannelKick },
    { "ChannelBan", &Script_ChannelBan },
    { "ChannelUnban", &Script_ChannelUnban },
    { "ChannelToggleAnnouncements", &Script_ChannelToggleAnnouncements },
    { "ChannelSilenceVoice", &Script_ChannelSilenceVoice },
    { "ChannelSilenceAll", &Script_ChannelSilenceAll },
    { "ChannelUnSilenceVoice", &Script_ChannelUnSilenceVoice },
    { "ChannelUnSilenceAll", &Script_ChannelUnSilenceAll },
    { "ChangeChatColor", &Script_ChangeChatColor },
    { "ResetChatColors", &Script_ResetChatColors },
    { "GetChatTypeIndex", &Script_GetChatTypeIndex },
    { "GetChatWindowInfo", &Script_GetChatWindowInfo },
    { "GetChatWindowSavedPosition", &Script_GetChatWindowSavedPosition },
    { "GetChatWindowSavedDimensions", &Script_GetChatWindowSavedDimensions },
    { "GetChatWindowMessages", &Script_GetChatWindowMessages },
    { "GetChatWindowChannels", &Script_GetChatWindowChannels },
    { "AddChatWindowMessages", &Script_AddChatWindowMessages },
    { "RemoveChatWindowMessages", &Script_RemoveChatWindowMessages },
    { "AddChatWindowChannel", &Script_AddChatWindowChannel },
    { "RemoveChatWindowChannel", &Script_RemoveChatWindowChannel },
    { "SetChatWindowName", &Script_SetChatWindowName },
    { "SetChatWindowSize", &Script_SetChatWindowSize },
    { "SetChatWindowColor", &Script_SetChatWindowColor },
    { "SetChatWindowAlpha", &Script_SetChatWindowAlpha },
    { "SetChatWindowLocked", &Script_SetChatWindowLocked },
    { "SetChatWindowDocked", &Script_SetChatWindowDocked },
    { "SetChatWindowUninteractable", &Script_SetChatWindowUninteractable },
    { "SetChatWindowShown", &Script_SetChatWindowShown },
    { "SetChatWindowSavedPosition", &Script_SetChatWindowSavedPosition },
    { "SetChatWindowSavedDimensions", &Script_SetChatWindowSavedDimensions },
    { "EnumerateServerChannels", &Script_EnumerateServerChannels },
    { "RequestRaidInfo", &Script_RequestRaidInfo },
    { "GetNumSavedInstances", &Script_GetNumSavedInstances },
    { "GetSavedInstanceInfo", &Script_GetSavedInstanceInfo },
    { "SetSavedInstanceExtend", &Script_SetSavedInstanceExtend },
    { "ResetChatWindows", &Script_ResetChatWindows },
    { "CanComplainChat", &Script_CanComplainChat },
    { "ComplainChat", &Script_ComplainChat },
    { "GetNumDisplayChannels", &Script_GetNumDisplayChannels },
    { "GetChannelDisplayInfo", &Script_GetChannelDisplayInfo },
    { "GetSelectedDisplayChannel", &Script_GetSelectedDisplayChannel },
    { "SetSelectedDisplayChannel", &Script_SetSelectedDisplayChannel },
    { "GetChannelRosterInfo", &Script_GetChannelRosterInfo },
    { "GetNumChannelMembers", &Script_GetNumChannelMembers },
    { "SetActiveVoiceChannel", &Script_SetActiveVoiceChannel },
    { "GetActiveVoiceChannel", &Script_GetActiveVoiceChannel },
    { "CollapseChannelHeader", &Script_CollapseChannelHeader },
    { "ExpandChannelHeader", &Script_ExpandChannelHeader },
    { "ChannelVoiceOn", &Script_ChannelVoiceOn },
    { "ChannelVoiceOff", &Script_ChannelVoiceOff },
    { "DisplayChannelVoiceOn", &Script_DisplayChannelVoiceOn },
    { "DisplayChannelVoiceOff", &Script_DisplayChannelVoiceOff },
    { "IsDisplayChannelOwner", &Script_IsDisplayChannelOwner },
    { "IsDisplayChannelModerator", &Script_IsDisplayChannelModerator },
    { "IsVoiceChatEnabled", &Script_IsVoiceChatEnabled },
    { "IsVoiceChatAllowed", &Script_IsVoiceChatAllowed },
    { "IsVoiceChatAllowedByServer", &Script_IsVoiceChatAllowedByServer },
    { "IsSilenced", &Script_IsSilenced },
    { "GetMuteStatus", &Script_GetMuteStatus },
    { "UnitIsSilenced", &Script_UnitIsSilenced },
    { "SetChannelWatch", &Script_SetChannelWatch },
    { "ClearChannelWatch", &Script_ClearChannelWatch },
    { "DeclineInvite", &Script_DeclineInvite },
    { "SetChatColorNameByClass", &Script_SetChatColorNameByClass },
};

// VERIFY: 19fef056 auto
// OFFSET: 0x004FCEF0
void ChatRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// VERIFY: b73141ed manual
// OFFSET: 0x0050D170
int32_t Script_SendChatMessage(lua_State* L) {
    if (!CGGameUI::CanPerformAction(10)) {
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: SendChatMessage(text [,type] [,language] [,targetPlayer])");
    }

    const char* text = lua_tolstring(L, 1, nullptr);
    int32_t type = CHAT_MSG_SAY;

    if (lua_isstring(L, 2)) {
        if (!StringToChatType(lua_tolstring(L, 2, nullptr), &type)) {
            luaL_error(L, "SendChatMessage(): Unknown chat type");
        }
    }

    if ((!text || !*text) && type != CHAT_MSG_DND && type != CHAT_MSG_AFK) {
        return 0;
    }

    if (!ValidateStringIsUTF8(text)) {
        luaL_error(L, "SendChatMessage(): Chat message must be UTF-8 text");
    }

    const char* target = nullptr;
    uint32_t language = player->GetDefaultLanguage();

    if (lua_isstring(L, 3)) {
        if (!StringToLanguage(lua_tolstring(L, 3, nullptr), &language)) {
            luaL_error(L, "SendChatMessage(): Unknown language");
        }

        uint32_t skill;

        if (!player->GetLanguageSkill(language, skill)) {
            CGGameUI::DisplayError(ERR_CANT_SPEAK_LANGAGE);
            return 0;
        }
    }

    if (lua_isstring(L, 4)) {
        target = lua_tolstring(L, 4, nullptr);
    }

    if (type == CHAT_MSG_WHISPER) {
        if (!target || !*target) {
            luaL_error(L, "SendChatMessage(): Whisper message missing target player!");
        }
    } else if (type == CHAT_MSG_PARTY) {
        if (!CGPartyInfo::InParty() && (!CGRaidInfo::m_numMembers || !CGRaidInfo::IsPVPRaid())) {
            CGGameUI::DisplayError(ERR_NOT_IN_GROUP);
            return 0;
        }

        if (CGPartyInfo::m_leader.m_guid == player->m_obj->m_guid.m_guid) {
            type = CHAT_MSG_PARTY_LEADER;
        }
    } else if (type == CHAT_MSG_RAID_WARNING) {
        if (!CGRaidInfo::m_numMembers) {
            if (!CGPartyInfo::InParty()) {
                CGGameUI::DisplayError(ERR_NOT_IN_GROUP);
                return 0;
            }
        } else {
            WOWGUID guid = player->m_obj->m_guid;

            if (!CGRaidInfo::IsPlayerRaidOfficer(guid)) {
                CGGameUI::DisplayError(ERR_NOT_LEADER);
                return 0;
            }
        }
    } else if (type == CHAT_MSG_RAID || type == CHAT_MSG_BATTLEGROUND) {
        if (!CGRaidInfo::m_numMembers) {
            CGGameUI::DisplayError(type != CHAT_MSG_BATTLEGROUND ? ERR_NOT_IN_RAID : ERR_NOT_IN_BATTLEGROUND);
            return 0;
        }

        if (type == CHAT_MSG_RAID) {
            if (CGPartyInfo::m_leader.m_guid == player->m_obj->m_guid.m_guid) {
                type = CHAT_MSG_RAID_LEADER;
            }
        } else if (CGPartyInfo::m_leader.m_guid == player->m_obj->m_guid.m_guid) {
            type = CHAT_MSG_BATTLEGROUND_LEADER;
        }
    } else if (type == CHAT_MSG_CHANNEL) {
        if (!target || !*target) {
            luaL_error(L, "SendChatMessage(): Channel send missing channel number");
        }

        int32_t number = SStrToInt(target);

        if (number < 1 || number > static_cast<int32_t>(CGChat::s_channels.Count())) {
            return 0;
        }

        ChatChannel* channel = &CGChat::s_channels.m_data[number - 1];

        if (channel->m_channelNumber != number || channel->m_unk9C) {
            return 0;
        }

        target = channel->m_name;
    } else if (type == CHAT_MSG_AFK) {
        if (!CGChat::m_afk && !*text) {
            text = FrameScript_GetText("DEFAULT_AFK_MESSAGE", -1, GENDER_NOT_APPLICABLE);
        }

        if (*text) {
            player->SetAFK(text);
        } else {
            CGPlayer_C::ClearAFK(1);
        }

        return 0;
    }

    if (CGChat::m_afk) {
        CGPlayer_C::ClearAFK(0);
    }

    if (type == CHAT_MSG_DND) {
        if (!(player->m_player->m_playerFlags & 0x4) && !*text) {
            text = FrameScript_GetText("DEFAULT_DND_MESSAGE", -1, GENDER_NOT_APPLICABLE);
        }

        if (*text) {
            char buffer[3000];
            SStrPrintf(buffer, sizeof(buffer), FrameScript_GetText("MARKED_DND", -1, GENDER_NOT_APPLICABLE), text);
            CGChat::AddChatMessage(buffer, CHAT_MSG_SYSTEM, nullptr, 0, nullptr, nullptr, nullptr, WOWGUID(), 0, WOWGUID(), 0, false, nullptr);
        } else {
            CGChat::AddChatMessage(FrameScript_GetText("CLEARED_DND", -1, GENDER_NOT_APPLICABLE), CHAT_MSG_SYSTEM, nullptr, 0, nullptr, nullptr, nullptr, WOWGUID(), 0, WOWGUID(), 0, false, nullptr);
        }
    }

    if (player->IsDeadOrGhost() && type != CHAT_MSG_WHISPER && type != CHAT_MSG_PARTY && type != CHAT_MSG_PARTY_LEADER && type != CHAT_MSG_RAID && type != CHAT_MSG_RAID_LEADER && type != CHAT_MSG_RAID_WARNING && type != CHAT_MSG_GUILD && type != CHAT_MSG_OFFICER && type != CHAT_MSG_CHANNEL && type != CHAT_MSG_DND && type != CHAT_MSG_BATTLEGROUND && type != CHAT_MSG_BATTLEGROUND_LEADER) {
        CGGameUI::DisplayError(ERR_CHAT_WHILE_DEAD);
        return 0;
    }

    CGTutorial::FlagTutorial(22);

    char parsed[256];
    ChatParserParseText(text, parsed, sizeof(parsed));
    char* message = parsed;
    char drunk[256];

    if (player->IsDrunk()) {
        InebriatedParseText(parsed, drunk, sizeof(drunk), player->GetDrunknessTurnValue());
        message = drunk;
    }

    TruncateNewLines(message);

    for (char* escape = SStrChr(message, '|'); escape;) {
        char* next;

        if (escape[1] == '|') {
            next = escape + 2;
        } else {
            if (escape[1] != 'c') {
                luaL_error(L, "SendChatMessage(): Invalid escape code in chat message");
            }

            char* end = SStrStr(escape, "|r");

            if (!end) {
                *escape = 0;
                break;
            }

            next = end + 2;
        }

        escape = SStrChr(next, '|');
    }

    if ((type == CHAT_MSG_WHISPER || type == CHAT_MSG_CHANNEL || type == CHAT_MSG_BATTLENET) && language != 0xFFFFFFFF) {
        CGChat::ProcessThrottledChat();

        if (type == CHAT_MSG_WHISPER || type == CHAT_MSG_BATTLENET) {
            if (!CGChat::IsInWhisperSourceHistory(target)) {
                uint32_t time = OsGetAsyncTimeMs();

                if (CGChat::IsWhisperThrottled(time)) {
                    CGChat::AddThrottledChat(type, language, target, message);
                    return 0;
                }

                CGChat::AddWhisperThrottled(time);
            }
        } else {
            uint32_t time = OsGetAsyncTimeMs();

            if (CGChat::IsChannelThrottled(target, time)) {
                CGChat::AddThrottledChat(type, language, target, message);
                return 0;
            }

            CGChat::AddChannelThrottled(target, time);
        }
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_MESSAGECHAT));
    msg.Put(static_cast<uint32_t>(type));
    msg.Put(language);

    if (type == CHAT_MSG_WHISPER || type == CHAT_MSG_CHANNEL) {
        msg.PutString(target);
    }

    msg.PutString(message);
    msg.Finalize();
    ClientServices::Send2(&msg);

    return 0;
}
