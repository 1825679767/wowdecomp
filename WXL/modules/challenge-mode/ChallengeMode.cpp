// Native source-owned port of extensions/wxl-challenge-mode/ChallengeMode.cpp.
// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#include "ChallengeMode.hpp"
#include "ChallengeBootstrap.hpp"
#include "NativeFeatures.hpp"
#include "NativeProtocol.hpp"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "common/Log.hpp"
#include <windows.h>
#include <array>
#include <bit>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <deque>
#include <limits>
#include <mutex>
#include <span>
#include <string>
#include <utility>
#include <vector>
namespace wxl::native::challenge_mode {
namespace {
static_assert(std::endian::native==std::endian::little);
bool Sync(lua_State* state);
bool changed=false,rewardChanged=false,registered=false;
uint64_t session=0,player=0;
uint32_t currentMap=0;
namespace network {
template<size_t Size>bool Send(uint16_t opcode,const std::array<uint8_t,Size>& payload){
 return Sync(FrameScript_GetContext())&&protocol::Send(opcode,payload.data(),uint32_t(Size));
}
}
namespace framescript {
bool Execute(const char* name,const char*) {
 if(name&&std::strstr(name,"display-toast"))rewardChanged=true;else changed=true;
 return true;
}
bool RegisterFunction(const char* name,lua_CFunction function){FrameScript_RegisterFunction(name,function);return true;}
}
    namespace opcodes
    {
        constexpr uint16_t CmsgChallengeModeRequestState = 0x052B;
        constexpr uint16_t SmsgChallengeModeState = 0x052C;
        constexpr uint16_t CmsgStartChallengeMode = 0x052D;
        constexpr uint16_t SmsgChallengeModeBossInfo = 0x052E;
        constexpr uint16_t SmsgChallengeModeKeystoneInfo = 0x052F;
        constexpr uint16_t SmsgChallengeModeComplete = 0x0530;
        constexpr uint16_t CmsgChallengeModeRequestHistory = 0x0531;
        constexpr uint16_t SmsgChallengeModeHistory = 0x0532;
        constexpr uint16_t CmsgSetLootSpecialization = 0x0533;
        constexpr uint16_t SmsgLootSpecialization = 0x0534;
        constexpr uint16_t SmsgDisplayToast = 0x0535;
    }

    constexpr uint8_t kStateProtocolVersion = 1;
    constexpr uint8_t kTransitionalStateProtocolVersion = 2;
    constexpr uint8_t kClientCapabilityVersion = 12;
    constexpr uint8_t kHistoryRatingCapabilityVersion = 13;
    constexpr uint8_t kBossInfoVersion = 3;
    constexpr uint8_t kKeystoneInfoVersion = 2;
    constexpr uint8_t kCompletionInfoVersion = 2;
    constexpr size_t kSnapshotV1Bytes = 52;
    constexpr size_t kSnapshotV2Bytes = 56;
    constexpr size_t kBossInfoHeaderBytes = 18;
    constexpr size_t kKeystoneInfoV1Bytes = 19;
    constexpr size_t kKeystoneInfoV2HeaderBytes = 25;
    constexpr size_t kCompletionInfoV1HeaderBytes = 50;
    constexpr size_t kCompletionInfoV2HeaderBytes = 79;
    constexpr uint8_t kMaxCompletionMembers = 5;
    constexpr uint8_t kMaxBossObjectives = 32;
    constexpr uint8_t kMaxHistoryRuns = 20;
    constexpr uint16_t kMaxBossNameBytes = 255;
    constexpr uint16_t kMaxMapNameBytes = 255;
    constexpr uint32_t kRequestThrottleMs = 250;
    constexpr uint8_t kHistoryInfoVersion = 2;
    constexpr uint8_t kHistoryInfoLegacyVersion = 1;
    constexpr size_t kHistoryInfoHeaderBytes = 9;
    constexpr uint8_t kLootSpecializationVersion = 1;
    constexpr uint8_t kDisplayToastVersion = 1;

    enum class ClientAction : uint8_t
    {
        RequestSnapshot = 0,
        StartChallengeMode = 1,
        DevelopmentSetKeystone = 2,
        RerollKeystone = 3,
        AbandonChallengeMode = 4,
        AcceptKeystoneUpgrade = 5,
    };

    struct Snapshot
    {
        uint8_t state = 0;
        uint8_t reason = 0;
        uint8_t flags = 0;
        uint32_t sequence = 0;
        uint32_t mapId = 0;
        uint32_t instanceId = 0;
        uint16_t level = 0;
        uint16_t bossesKilled = 0;
        uint16_t bossesTotal = 0;
        uint16_t deaths = 0;
        uint32_t timeLimitMs = 0;
        uint32_t elapsedMs = 0;
        uint32_t deathPenaltyMs = 0;
        uint32_t forcesEarned = 0;
        uint32_t forcesTotal = 0;
        uint16_t damagePercent = 0;
        uint16_t healthPercent = 0;
        uint32_t countdownMs = 0;
        uint32_t generation = 0;
    };

    struct BossInfo
    {
        uint32_t sequence = 0;
        uint32_t mapId = 0;
        uint32_t instanceId = 0;
        uint32_t completedMask = 0;
        uint16_t seasonId = 0;
        uint16_t weekIndex = 0;
        uint8_t affixCount = 0;
        std::array<uint16_t, 4> affixIds{};
        uint32_t personalBestTimeMs = 0;
        uint32_t realmBestTimeMs = 0;
        std::vector<std::string> names;
        std::vector<uint32_t> completionTimesMs;
        uint32_t generation = 0;
    };

    struct KeystoneInfo
    {
        uint8_t flags = 0;
        uint32_t mapId = 0;
        uint16_t level = 0;
        uint16_t seasonId = 0;
        uint8_t affixCount = 0;
        std::array<uint16_t, 4> affixIds{};
        uint32_t timeLimitMs = 0;
        std::string mapName;
        uint32_t generation = 0;
    };

    struct CompletionInfo
    {
        struct Member
        {
            uint32_t guid = 0;
            uint8_t classId = 0;
            std::string name;
            uint32_t ratingBefore = 0;
            uint32_t ratingAfter = 0;
            int32_t ratingChange = 0;
            uint32_t bestTimeMs = 0;
            uint8_t flags = 0;
        };

        uint64_t completionId = 0;
        uint32_t mapId = 0;
        uint32_t instanceId = 0;
        uint16_t level = 0;
        uint32_t completionTimeMs = 0;
        uint32_t timeLimitMs = 0;
        uint32_t deathPenaltyMs = 0;
        uint16_t deaths = 0;
        uint8_t completedOnTime = 0;
        uint8_t keystoneUpgradeLevels = 0;
        uint32_t oldKeystoneMapId = 0;
        uint16_t oldKeystoneLevel = 0;
        uint32_t newKeystoneMapId = 0;
        uint16_t newKeystoneLevel = 0;
        uint16_t seasonId = 0;
        uint32_t previousRealmBestTimeMs = 0;
        uint32_t realmBestTimeMs = 0;
        uint32_t previousPersonalBestTimeMs = 0;
        uint32_t personalBestTimeMs = 0;
        uint32_t ratingBefore = 0;
        uint32_t ratingAfter = 0;
        int32_t ratingChange = 0;
        uint8_t recordFlags = 0;
        std::vector<Member> members;
        uint32_t generation = 0;
    };

    struct HistoryInfo
    {
        struct Member
        {
            uint32_t guid = 0;
            uint8_t classId = 0;
            std::string name;
        };

        struct Split
        {
            std::string name;
            uint32_t completionTimeMs = 0;
        };

        struct Run
        {
            uint64_t id = 0;
            uint32_t mapId = 0;
            std::string mapName;
            uint16_t level = 0;
            uint32_t completionTimeMs = 0;
            uint32_t timeLimitMs = 0;
            uint16_t deaths = 0;
            uint8_t completedOnTime = 0;
            uint32_t completedAt = 0;
            uint8_t affixCount = 0;
            std::array<uint16_t, 4> affixIds{};
            std::vector<Member> members;
            std::vector<Split> splits;
        };

        uint8_t scope = 0;
        uint16_t seasonId = 0;
        uint32_t requestedMapId = 0;
        uint32_t overallRating = 0;
        std::vector<Run> runs;
        uint32_t generation = 0;
    };

    struct LootSpecializationInfo
    {
        uint8_t selection = 0;
        uint32_t generation = 0;
    };

    struct RewardToast
    {
        uint8_t type = 0;
        uint8_t method = 0;
        uint8_t flags = 0;
        uint8_t quality = 0;
        uint32_t itemId = 0;
        uint32_t quantity = 0;
        std::string name;
    };

    struct Reader
    {
        const uint8_t* data = nullptr;
        size_t size = 0;
        size_t cursor = 0;

        template <typename T>
        bool Read(T& value)
        {
            if (!data || cursor > size || sizeof(T) > size - cursor)
                return false;
            std::memcpy(&value, data + cursor, sizeof(T));
            cursor += sizeof(T);
            return true;
        }

        bool ReadString(uint16_t bytes, std::string& value)
        {
            if (cursor > size || bytes > size - cursor)
                return false;
            value.assign(reinterpret_cast<char const*>(data + cursor), bytes);
            cursor += bytes;
            return true;
        }
    };

    std::mutex g_snapshotMutex;
    Snapshot g_snapshot;
    BossInfo g_bossInfo;
    KeystoneInfo g_keystoneInfo;
    CompletionInfo g_completionInfo;
    HistoryInfo g_historyInfo;
    LootSpecializationInfo g_lootSpecializationInfo;
    std::deque<RewardToast> g_rewardToasts;
    uint32_t g_lastRequestMs = 0;

    bool ParseSnapshot(std::span<const uint8_t> payload)
    {
        Reader reader{payload.data(), payload.size()};
        uint8_t version = 0;
        Snapshot replacement;
        if (!reader.Read(version) ||
            !((version == kStateProtocolVersion &&
                    payload.size() == kSnapshotV1Bytes) ||
                (version == kTransitionalStateProtocolVersion &&
                    payload.size() == kSnapshotV2Bytes)))
        {
            return false;
        }

        if (!reader.Read(replacement.state) ||
            !reader.Read(replacement.reason) ||
            !reader.Read(replacement.flags) ||
            !reader.Read(replacement.sequence) ||
            !reader.Read(replacement.mapId) ||
            !reader.Read(replacement.instanceId) ||
            !reader.Read(replacement.level) ||
            !reader.Read(replacement.bossesKilled) ||
            !reader.Read(replacement.bossesTotal))
        {
            return false;
        }

        // Transitional protocol v2 put the objective mask inside the fixed
        // state packet. Consume it for compatibility with a server updated in
        // the opposite order; current servers send it through boss info.
        uint32_t transitionalBossMask = 0;
        if (version == kTransitionalStateProtocolVersion &&
            !reader.Read(transitionalBossMask))
            return false;

        if (!reader.Read(replacement.deaths) ||
            !reader.Read(replacement.timeLimitMs) ||
            !reader.Read(replacement.elapsedMs) ||
            !reader.Read(replacement.deathPenaltyMs) ||
            !reader.Read(replacement.forcesEarned) ||
            !reader.Read(replacement.forcesTotal) ||
            !reader.Read(replacement.damagePercent) ||
            !reader.Read(replacement.healthPercent) ||
            !reader.Read(replacement.countdownMs) || replacement.state > 4 ||
            replacement.level > 100 ||
            replacement.bossesTotal > kMaxBossObjectives ||
            replacement.bossesKilled > replacement.bossesTotal ||
            reader.cursor != reader.size)
        {
            return false;
        }

        const std::lock_guard lock(g_snapshotMutex);
        replacement.generation = g_snapshot.generation + 1;
        if (!replacement.generation)
            replacement.generation = 1;
        if (replacement.state == 0 && g_snapshot.state == 3)
        {
            CompletionInfo cleared;
            cleared.generation = g_completionInfo.generation + 1;
            if (!cleared.generation)
                cleared.generation = 1;
            g_completionInfo = std::move(cleared);
        }
        g_snapshot = replacement;
        return true;
    }

    void OnSnapshot(std::span<const uint8_t> payload, void*)
    {
        if (!ParseSnapshot(payload))
        {
            WLOG_WARN(
                "mythic-plus: rejected malformed snapshot bytes=%zu",
                payload.size());
            return;
        }

        framescript::Execute(
            "mythic-plus-update",
            "if wxlwow and wxlwow.challenge_mode and "
            "wxlwow.challenge_mode._NativeChanged then "
            "wxlwow.challenge_mode._NativeChanged() end");
    }

    bool ParseBossInfo(std::span<const uint8_t> payload)
    {
        if (payload.size() < kBossInfoHeaderBytes || payload.size() > 8192)
            return false;

        Reader reader{payload.data(), payload.size()};
        uint8_t version = 0;
        BossInfo replacement;
        if (!reader.Read(version) ||
            !reader.Read(replacement.sequence) ||
            !reader.Read(replacement.mapId) ||
            !reader.Read(replacement.instanceId) ||
            !reader.Read(replacement.completedMask) ||
            (version < 1 || version > kBossInfoVersion))
        {
            return false;
        }

        if (version >= 2)
        {
            if (!reader.Read(replacement.seasonId) ||
                !reader.Read(replacement.weekIndex) ||
                !reader.Read(replacement.affixCount) ||
                replacement.affixCount > replacement.affixIds.size())
                return false;
            for (uint16_t& affixId : replacement.affixIds)
                if (!reader.Read(affixId))
                    return false;
        }

        if (version >= 3 &&
            (!reader.Read(replacement.personalBestTimeMs) ||
                !reader.Read(replacement.realmBestTimeMs)))
        {
            return false;
        }

        uint8_t count = 0;
        if (!reader.Read(count) || count > kMaxBossObjectives)
            return false;

        replacement.names.reserve(count);
        replacement.completionTimesMs.reserve(count);
        for (uint8_t index = 0; index < count; ++index)
        {
            uint16_t nameBytes = 0;
            std::string name;
            if (!reader.Read(nameBytes) || nameBytes > kMaxBossNameBytes ||
                !reader.ReadString(nameBytes, name) || name.empty())
            {
                return false;
            }
            replacement.names.push_back(std::move(name));
            uint32_t completionTimeMs = 0;
            if (version >= 3 && !reader.Read(completionTimeMs))
                return false;
            replacement.completionTimesMs.push_back(completionTimeMs);
        }
        if (reader.cursor != reader.size)
            return false;

        const uint32_t validMask = count == 32
            ? std::numeric_limits<uint32_t>::max()
            : ((uint32_t(1) << count) - 1);
        if (replacement.completedMask & ~validMask)
            return false;

        const std::lock_guard lock(g_snapshotMutex);
        replacement.generation = g_bossInfo.generation + 1;
        if (!replacement.generation)
            replacement.generation = 1;
        g_bossInfo = std::move(replacement);
        return true;
    }

    void OnBossInfo(std::span<const uint8_t> payload, void*)
    {
        if (!ParseBossInfo(payload))
        {
            WLOG_WARN(
                "mythic-plus: rejected malformed boss info bytes=%zu",
                payload.size());
            return;
        }

        framescript::Execute(
            "mythic-plus-boss-info",
            "if wxlwow and wxlwow.challenge_mode and "
            "wxlwow.challenge_mode._NativeChanged then "
            "wxlwow.challenge_mode._NativeChanged() end");
    }

    bool ParseKeystoneInfo(std::span<const uint8_t> payload)
    {
        if (payload.size() < kKeystoneInfoV1Bytes ||
            payload.size() > kKeystoneInfoV2HeaderBytes + kMaxMapNameBytes)
            return false;

        Reader reader{payload.data(), payload.size()};
        uint8_t version = 0;
        KeystoneInfo replacement;
        if (!reader.Read(version) || version < 1 ||
            version > kKeystoneInfoVersion ||
            !reader.Read(replacement.flags) ||
            !reader.Read(replacement.mapId) ||
            !reader.Read(replacement.level) ||
            !reader.Read(replacement.seasonId) ||
            !reader.Read(replacement.affixCount) ||
            replacement.affixCount > replacement.affixIds.size())
        {
            return false;
        }
        for (uint16_t& affixId : replacement.affixIds)
            if (!reader.Read(affixId))
                return false;
        if (version >= 2)
        {
            uint16_t mapNameBytes = 0;
            if (!reader.Read(replacement.timeLimitMs) ||
                !reader.Read(mapNameBytes) ||
                mapNameBytes > kMaxMapNameBytes ||
                !reader.ReadString(mapNameBytes, replacement.mapName))
            {
                return false;
            }
        }
        if (reader.cursor != reader.size ||
            (version == 1 && payload.size() != kKeystoneInfoV1Bytes) ||
            ((replacement.flags & 1u) == 0 &&
                (replacement.mapId || replacement.level ||
                    replacement.affixCount || replacement.timeLimitMs ||
                    !replacement.mapName.empty())) ||
            replacement.level > 30 ||
            (replacement.timeLimitMs &&
                (replacement.timeLimitMs < 60000 ||
                    replacement.timeLimitMs > 7200000)))
        {
            return false;
        }

        const std::lock_guard lock(g_snapshotMutex);
        replacement.generation = g_keystoneInfo.generation + 1;
        if (!replacement.generation)
            replacement.generation = 1;
        g_keystoneInfo = replacement;
        return true;
    }

    void OnKeystoneInfo(std::span<const uint8_t> payload, void*)
    {
        if (!ParseKeystoneInfo(payload))
        {
            WLOG_WARN(
                "mythic-plus: rejected malformed keystone info bytes=%zu",
                payload.size());
            return;
        }

        framescript::Execute(
            "mythic-plus-keystone-info",
            "if wxlwow and wxlwow.challenge_mode and "
            "wxlwow.challenge_mode._NativeKeystoneChanged then "
            "wxlwow.challenge_mode._NativeKeystoneChanged() end");
    }

    bool ParseCompletionInfo(std::span<const uint8_t> payload)
    {
        if (payload.size() < kCompletionInfoV1HeaderBytes ||
            payload.size() > 2048)
            return false;

        Reader reader{payload.data(), payload.size()};
        uint8_t version = 0;
        uint8_t memberCount = 0;
        CompletionInfo replacement;
        if (!reader.Read(version) ||
            (version != 1 && version != kCompletionInfoVersion) ||
            !reader.Read(replacement.completionId) ||
            !reader.Read(replacement.mapId) ||
            !reader.Read(replacement.instanceId) ||
            !reader.Read(replacement.level) ||
            !reader.Read(replacement.completionTimeMs) ||
            !reader.Read(replacement.timeLimitMs) ||
            !reader.Read(replacement.deathPenaltyMs) ||
            !reader.Read(replacement.deaths) ||
            !reader.Read(replacement.completedOnTime) ||
            !reader.Read(replacement.keystoneUpgradeLevels) ||
            !reader.Read(replacement.oldKeystoneMapId) ||
            !reader.Read(replacement.oldKeystoneLevel) ||
            !reader.Read(replacement.newKeystoneMapId) ||
            !reader.Read(replacement.newKeystoneLevel) ||
            !reader.Read(replacement.seasonId))
        {
            return false;
        }

        if (version >= 2 &&
            (!reader.Read(replacement.previousRealmBestTimeMs) ||
                !reader.Read(replacement.realmBestTimeMs) ||
                !reader.Read(replacement.previousPersonalBestTimeMs) ||
                !reader.Read(replacement.personalBestTimeMs) ||
                !reader.Read(replacement.ratingBefore) ||
                !reader.Read(replacement.ratingAfter) ||
                !reader.Read(replacement.ratingChange) ||
                !reader.Read(replacement.recordFlags)))
        {
            return false;
        }

        if (!reader.Read(memberCount) || memberCount > kMaxCompletionMembers ||
            !replacement.completionId ||
            (replacement.level != 0 && replacement.level < 2) ||
            replacement.level > 30 || replacement.newKeystoneLevel < 2 ||
            replacement.newKeystoneLevel > 30 ||
            replacement.completedOnTime > 1 ||
            replacement.keystoneUpgradeLevels > 3 ||
            replacement.recordFlags > 3 ||
            (version == 1 && payload.size() < kCompletionInfoV1HeaderBytes) ||
            (version >= 2 && payload.size() < kCompletionInfoV2HeaderBytes))
        {
            return false;
        }

        replacement.members.reserve(memberCount);
        for (uint8_t index = 0; index < memberCount; ++index)
        {
            CompletionInfo::Member member;
            uint8_t nameBytes = 0;
            if (!reader.Read(member.guid) || !reader.Read(member.classId) ||
                !reader.Read(nameBytes) || !nameBytes || nameBytes > 64 ||
                !reader.ReadString(nameBytes, member.name) ||
                member.classId < 1 || member.classId > 11)
            {
                return false;
            }
            if (version >= 2 &&
                (!reader.Read(member.ratingBefore) ||
                    !reader.Read(member.ratingAfter) ||
                    !reader.Read(member.ratingChange) ||
                    !reader.Read(member.bestTimeMs) ||
                    !reader.Read(member.flags) || member.flags > 1))
            {
                return false;
            }
            replacement.members.push_back(std::move(member));
        }
        if (reader.cursor != reader.size)
            return false;

        const std::lock_guard lock(g_snapshotMutex);
        replacement.generation = g_completionInfo.generation + 1;
        if (!replacement.generation)
            replacement.generation = 1;
        g_completionInfo = std::move(replacement);
        return true;
    }

    void OnCompletionInfo(std::span<const uint8_t> payload, void*)
    {
        if (!ParseCompletionInfo(payload))
        {
            WLOG_WARN(
                "mythic-plus: rejected malformed completion info bytes=%zu",
                payload.size());
            return;
        }

        framescript::Execute(
            "mythic-plus-completion-info",
            "if wxlwow and wxlwow.challenge_mode and "
            "wxlwow.challenge_mode._NativeCompletionChanged then "
            "wxlwow.challenge_mode._NativeCompletionChanged() end");
    }

    bool ParseHistoryInfo(std::span<const uint8_t> payload)
    {
        if (payload.size() < kHistoryInfoHeaderBytes ||
            payload.size() > 64u * 1024u)
            return false;

        Reader reader{payload.data(), payload.size()};
        uint8_t version = 0;
        uint8_t runCount = 0;
        HistoryInfo replacement;
        if (!reader.Read(version) ||
            (version != kHistoryInfoLegacyVersion &&
                version != kHistoryInfoVersion) ||
            !reader.Read(replacement.scope) || replacement.scope > 1 ||
            !reader.Read(replacement.seasonId) ||
            !reader.Read(replacement.requestedMapId))
        {
            return false;
        }
        if (version >= kHistoryInfoVersion &&
            !reader.Read(replacement.overallRating))
            return false;
        if (!reader.Read(runCount) || runCount > kMaxHistoryRuns)
            return false;

        replacement.runs.reserve(runCount);
        for (uint8_t runIndex = 0; runIndex < runCount; ++runIndex)
        {
            HistoryInfo::Run run;
            uint16_t mapNameBytes = 0;
            if (!reader.Read(run.id) || !reader.Read(run.mapId) ||
                !reader.Read(mapNameBytes) ||
                mapNameBytes > kMaxMapNameBytes ||
                !reader.ReadString(mapNameBytes, run.mapName) ||
                !reader.Read(run.level) || run.level > 30 ||
                !reader.Read(run.completionTimeMs) ||
                !reader.Read(run.timeLimitMs) ||
                !reader.Read(run.deaths) ||
                !reader.Read(run.completedOnTime) ||
                run.completedOnTime > 1 ||
                !reader.Read(run.completedAt) ||
                !reader.Read(run.affixCount) ||
                run.affixCount > run.affixIds.size())
            {
                return false;
            }
            for (uint16_t& affixId : run.affixIds)
                if (!reader.Read(affixId))
                    return false;

            uint8_t memberCount = 0;
            if (!reader.Read(memberCount) ||
                memberCount > kMaxCompletionMembers)
                return false;
            run.members.reserve(memberCount);
            for (uint8_t memberIndex = 0; memberIndex < memberCount;
                ++memberIndex)
            {
                HistoryInfo::Member member;
                uint8_t nameBytes = 0;
                if (!reader.Read(member.guid) ||
                    !reader.Read(member.classId) ||
                    !reader.Read(nameBytes) || nameBytes > 31 ||
                    !reader.ReadString(nameBytes, member.name))
                {
                    return false;
                }
                run.members.push_back(std::move(member));
            }

            uint8_t splitCount = 0;
            if (!reader.Read(splitCount) ||
                splitCount > kMaxBossObjectives)
                return false;
            run.splits.reserve(splitCount);
            for (uint8_t splitIndex = 0; splitIndex < splitCount;
                ++splitIndex)
            {
                HistoryInfo::Split split;
                uint8_t nameBytes = 0;
                if (!reader.Read(nameBytes) || nameBytes > 127 ||
                    !reader.ReadString(nameBytes, split.name) ||
                    !reader.Read(split.completionTimeMs))
                {
                    return false;
                }
                run.splits.push_back(std::move(split));
            }
            replacement.runs.push_back(std::move(run));
        }

        if (reader.cursor != reader.size)
            return false;

        const std::lock_guard lock(g_snapshotMutex);
        replacement.generation = g_historyInfo.generation + 1;
        if (!replacement.generation)
            replacement.generation = 1;
        g_historyInfo = std::move(replacement);
        return true;
    }

    void OnHistoryInfo(std::span<const uint8_t> payload, void*)
    {
        if (!ParseHistoryInfo(payload))
        {
            WLOG_WARN(
                "mythic-plus: rejected malformed history info bytes=%zu",
                payload.size());
            return;
        }

        framescript::Execute(
            "mythic-plus-history-info",
            "if wxlwow and wxlwow.challenge_mode and "
            "wxlwow.challenge_mode._NativeHistoryChanged then "
            "wxlwow.challenge_mode._NativeHistoryChanged() end");
    }

    bool ParseLootSpecialization(std::span<const uint8_t> payload)
    {
        if (payload.size() != 2)
            return false;
        Reader reader{payload.data(), payload.size()};
        uint8_t version = 0;
        uint8_t selection = 0;
        if (!reader.Read(version) || version != kLootSpecializationVersion ||
            !reader.Read(selection) || reader.cursor != reader.size ||
            !(selection <= 3 || selection == 255))
            return false;
        const std::lock_guard lock(g_snapshotMutex);
        g_lootSpecializationInfo.selection = selection;
        ++g_lootSpecializationInfo.generation;
        if (!g_lootSpecializationInfo.generation)
            g_lootSpecializationInfo.generation = 1;
        return true;
    }

    void OnLootSpecialization(std::span<const uint8_t> payload, void*)
    {
        if (!ParseLootSpecialization(payload))
        {
            WLOG_WARN(
                "mythic-plus: rejected malformed loot specialization bytes=%zu",
                payload.size());
            return;
        }
        framescript::Execute(
            "loot-specialization",
            "if wxlwow and wxlwow.challenge_mode and "
            "wxlwow.challenge_mode._NativeLootSpecializationChanged then "
            "wxlwow.challenge_mode._NativeLootSpecializationChanged() end");
    }

    bool ParseDisplayToast(std::span<const uint8_t> payload)
    {
        if (payload.size() < 15 || payload.size() > 512)
            return false;

        Reader reader{payload.data(), payload.size()};
        uint8_t version = 0;
        uint16_t nameBytes = 0;
        RewardToast replacement;
        if (!reader.Read(version) || version != kDisplayToastVersion ||
            !reader.Read(replacement.type) ||
            !reader.Read(replacement.method) ||
            !reader.Read(replacement.flags) ||
            !reader.Read(replacement.quality) ||
            !reader.Read(replacement.itemId) ||
            !reader.Read(replacement.quantity) ||
            !reader.Read(nameBytes) || nameBytes > 255 ||
            !reader.ReadString(nameBytes, replacement.name) ||
            reader.cursor != reader.size || replacement.type > 3 ||
            !replacement.itemId || !replacement.quantity)
        {
            return false;
        }

        const std::lock_guard lock(g_snapshotMutex);
        g_rewardToasts.push_back(std::move(replacement));
        while (g_rewardToasts.size() > 16)
            g_rewardToasts.pop_front();
        return true;
    }

    void OnDisplayToast(std::span<const uint8_t> payload, void*)
    {
        if (!ParseDisplayToast(payload))
        {
            WLOG_WARN("mythic-plus: rejected malformed display toast bytes=%zu",
                payload.size());
            return;
        }

        framescript::Execute(
            "mythic-plus-display-toast",
            "if wxlwow and wxlwow.challenge_mode and "
            "wxlwow.challenge_mode._NativeRewardToastChanged then "
            "wxlwow.challenge_mode._NativeRewardToastChanged() end");
    }

    bool SendAction(ClientAction action, uint16_t level = 0)
    {
        const uint32_t now = GetTickCount();
        if (action == ClientAction::RequestSnapshot && g_lastRequestMs &&
            now - g_lastRequestMs < kRequestThrottleMs)
        {
            return true;
        }

        bool sent = false;
        if (action == ClientAction::RequestSnapshot)
        {
            const std::array<uint8_t, 1> payload{kClientCapabilityVersion};
            sent = network::Send(
                opcodes::CmsgChallengeModeRequestState, payload);
        }
        else
        {
            uint8_t wireAction = 1;
            if (action == ClientAction::StartChallengeMode)
                wireAction = 0;
            else if (action == ClientAction::RerollKeystone)
                wireAction = 2;
            else if (action == ClientAction::AbandonChallengeMode)
                wireAction = 3;
            else if (action == ClientAction::AcceptKeystoneUpgrade)
                wireAction = 4;
            const std::array<uint8_t, 4> payload{
                kClientCapabilityVersion,
                wireAction,
                static_cast<uint8_t>(level & 0xFFu),
                static_cast<uint8_t>((level >> 8) & 0xFFu),
            };
            sent = network::Send(opcodes::CmsgStartChallengeMode, payload);
        }
        if (sent && action == ClientAction::RequestSnapshot)
            g_lastRequestMs = now;
        return sent;
    }

    int LuaRequest(lua_State* state)
    {
        Sync(state);
        lua_pushboolean(
            state, SendAction(ClientAction::RequestSnapshot) ? 1 : 0);
        return 1;
    }

    int LuaRequestHistory(lua_State* state)
    {
        Sync(state);
        const double scopeValue = lua_tonumber(state, 1);
        const double mapValue = lua_tonumber(state, 2);
        const double seasonValue = lua_tonumber(state, 3);
        const bool valid = std::isfinite(scopeValue) &&
            std::floor(scopeValue) == scopeValue && scopeValue >= 0.0 &&
            scopeValue <= 1.0 && std::isfinite(mapValue) &&
            std::floor(mapValue) == mapValue && mapValue >= 0.0 &&
            mapValue <= double(std::numeric_limits<uint32_t>::max()) &&
            std::isfinite(seasonValue) &&
            std::floor(seasonValue) == seasonValue && seasonValue >= 0.0 &&
            seasonValue <= double(std::numeric_limits<uint16_t>::max());
        bool sent = false;
        if (valid)
        {
            const uint32_t mapId = static_cast<uint32_t>(mapValue);
            const uint16_t seasonId = static_cast<uint16_t>(seasonValue);
            const std::array<uint8_t, 8> payload{
                kHistoryRatingCapabilityVersion,
                static_cast<uint8_t>(scopeValue),
                static_cast<uint8_t>(mapId & 0xFFu),
                static_cast<uint8_t>((mapId >> 8) & 0xFFu),
                static_cast<uint8_t>((mapId >> 16) & 0xFFu),
                static_cast<uint8_t>((mapId >> 24) & 0xFFu),
                static_cast<uint8_t>(seasonId & 0xFFu),
                static_cast<uint8_t>((seasonId >> 8) & 0xFFu),
            };
            sent = network::Send(
                opcodes::CmsgChallengeModeRequestHistory, payload);
        }
        lua_pushboolean(state, sent ? 1 : 0);
        return 1;
    }

    int LuaStartChallengeMode(lua_State* state)
    {
        Sync(state);
        lua_pushboolean(state,
            SendAction(ClientAction::StartChallengeMode) ? 1 : 0);
        return 1;
    }

    int LuaDevelopmentSetKeystone(lua_State* state)
    {
        Sync(state);
        const double value = lua_tonumber(state, 1);
        const bool valid = std::isfinite(value) && value >= 2.0 &&
            value <= 30.0 && std::floor(value) == value;
        const bool sent = valid && SendAction(
            ClientAction::DevelopmentSetKeystone,
            static_cast<uint16_t>(value));
        lua_pushboolean(state, sent ? 1 : 0);
        return 1;
    }

    int LuaRerollKeystone(lua_State* state)
    {
        Sync(state);
        lua_pushboolean(state,
            SendAction(ClientAction::RerollKeystone) ? 1 : 0);
        return 1;
    }

    int LuaAbandonChallengeMode(lua_State* state)
    {
        Sync(state);
        lua_pushboolean(state,
            SendAction(ClientAction::AbandonChallengeMode) ? 1 : 0);
        return 1;
    }

    int LuaAcceptKeystoneUpgrade(lua_State* state)
    {
        Sync(state);
        lua_pushboolean(state,
            SendAction(ClientAction::AcceptKeystoneUpgrade) ? 1 : 0);
        return 1;
    }

    int LuaPopRewardToast(lua_State* state)
    {
        Sync(state);
        RewardToast toast;
        {
            const std::lock_guard lock(g_snapshotMutex);
            if (g_rewardToasts.empty())
                return 0;
            toast = std::move(g_rewardToasts.front());
            g_rewardToasts.pop_front();
        }

        const auto pushNumber = lua_pushnumber;
        const auto pushString = lua_pushstring;
        pushNumber(state, toast.type);
        pushNumber(state, toast.method);
        pushNumber(state, toast.flags);
        pushNumber(state, toast.quality);
        pushNumber(state, toast.itemId);
        pushNumber(state, toast.quantity);
        pushString(state, toast.name.c_str());
        return 7;
    }

    int LuaSetLootSpecialization(lua_State* state)
    {
        Sync(state);
        const double value = lua_tonumber(state, 1);
        const bool valid = std::isfinite(value) &&
            std::floor(value) == value && value >= 0.0 &&
            (value <= 3.0 || value == 255.0);
        bool sent = false;
        if (valid)
        {
            const std::array<uint8_t, 1> payload{
                static_cast<uint8_t>(value)};
            sent = network::Send(
                opcodes::CmsgSetLootSpecialization, payload);
        }
        lua_pushboolean(state, sent ? 1 : 0);
        return 1;
    }

    int LuaLootSpecializationInfo(lua_State* state)
    {
        Sync(state);
        LootSpecializationInfo info;
        {
            const std::lock_guard lock(g_snapshotMutex);
            info = g_lootSpecializationInfo;
        }
        const auto push = lua_pushnumber;
        push(state, info.generation);
        push(state, info.selection);
        return 2;
    }

    int LuaSnapshot(lua_State* state)
    {
        Sync(state);
        Snapshot snapshot;
        {
            const std::lock_guard lock(g_snapshotMutex);
            snapshot = g_snapshot;
        }

        const auto push = lua_pushnumber;
        push(state, snapshot.generation);
        push(state, snapshot.state);
        push(state, snapshot.reason);
        push(state, snapshot.flags);
        push(state, snapshot.sequence);
        push(state, snapshot.mapId);
        push(state, snapshot.instanceId);
        push(state, snapshot.level);
        push(state, snapshot.bossesKilled);
        push(state, snapshot.bossesTotal);
        push(state, snapshot.deaths);
        push(state, snapshot.timeLimitMs);
        push(state, snapshot.elapsedMs);
        push(state, snapshot.deathPenaltyMs);
        push(state, snapshot.forcesEarned);
        push(state, snapshot.forcesTotal);
        push(state, snapshot.damagePercent);
        push(state, snapshot.healthPercent);
        push(state, snapshot.countdownMs);
        return 19;
    }

    bool EnsureLuaResultStack(lua_State* state, size_t resultCount,
        char const* context)
    {
        if (resultCount > static_cast<size_t>((std::numeric_limits<int>::max)()))
        {
            WLOG_WARN("mythic-plus: rejected oversized Lua %s result count=%zu",
                context, resultCount);
            return false;
        }

        const auto checkStack = lua_checkstack;
        if (checkStack(state, static_cast<int>(resultCount)) != 0)
            return true;

        WLOG_WARN("mythic-plus: failed to reserve Lua %s result stack count=%zu",
            context, resultCount);
        return false;
    }

    int LuaBossInfo(lua_State* state)
    {
        Sync(state);
        BossInfo info;
        {
            const std::lock_guard lock(g_snapshotMutex);
            info = g_bossInfo;
        }

        const size_t resultCount = 15 + info.names.size() +
            info.completionTimesMs.size();
        if (!EnsureLuaResultStack(state, resultCount, "boss-info"))
            return 0;

        const auto pushNumber = lua_pushnumber;
        const auto pushString = lua_pushstring;
        pushNumber(state, info.generation);
        pushNumber(state, info.sequence);
        pushNumber(state, info.mapId);
        pushNumber(state, info.instanceId);
        pushNumber(state, info.completedMask);
        pushNumber(state, info.names.size());
        pushNumber(state, info.seasonId);
        pushNumber(state, info.weekIndex);
        pushNumber(state, info.affixCount);
        for (uint16_t affixId : info.affixIds)
            pushNumber(state, affixId);
        pushNumber(state, info.personalBestTimeMs);
        pushNumber(state, info.realmBestTimeMs);
        for (std::string const& name : info.names)
            pushString(state, name.c_str());
        for (uint32_t completionTimeMs : info.completionTimesMs)
            pushNumber(state, completionTimeMs);
        return static_cast<int>(resultCount);
    }

    int LuaKeystoneInfo(lua_State* state)
    {
        Sync(state);
        KeystoneInfo info;
        {
            const std::lock_guard lock(g_snapshotMutex);
            info = g_keystoneInfo;
        }

        const auto push = lua_pushnumber;
        const auto pushString = lua_pushstring;
        push(state, info.generation);
        push(state, info.flags);
        push(state, info.mapId);
        push(state, info.level);
        push(state, info.seasonId);
        push(state, info.affixCount);
        for (uint16_t affixId : info.affixIds)
            push(state, affixId);
        push(state, info.timeLimitMs);
        pushString(state, info.mapName.c_str());
        return 12;
    }

    int LuaCompletionInfo(lua_State* state)
    {
        Sync(state);
        CompletionInfo info;
        {
            const std::lock_guard lock(g_snapshotMutex);
            info = g_completionInfo;
        }

        const size_t resultCount = 24 + info.members.size() * 8;
        if (!EnsureLuaResultStack(state, resultCount, "completion-info"))
            return 0;

        const auto pushNumber = lua_pushnumber;
        const auto pushString = lua_pushstring;
        pushNumber(state, info.generation);
        pushNumber(state, info.mapId);
        pushNumber(state, info.instanceId);
        pushNumber(state, info.level);
        pushNumber(state, info.completionTimeMs);
        pushNumber(state, info.timeLimitMs);
        pushNumber(state, info.deathPenaltyMs);
        pushNumber(state, info.deaths);
        pushNumber(state, info.completedOnTime);
        pushNumber(state, info.keystoneUpgradeLevels);
        pushNumber(state, info.oldKeystoneMapId);
        pushNumber(state, info.oldKeystoneLevel);
        pushNumber(state, info.newKeystoneMapId);
        pushNumber(state, info.newKeystoneLevel);
        pushNumber(state, info.seasonId);
        pushNumber(state, info.previousRealmBestTimeMs);
        pushNumber(state, info.realmBestTimeMs);
        pushNumber(state, info.previousPersonalBestTimeMs);
        pushNumber(state, info.personalBestTimeMs);
        pushNumber(state, info.ratingBefore);
        pushNumber(state, info.ratingAfter);
        pushNumber(state, info.ratingChange);
        pushNumber(state, info.recordFlags);
        pushNumber(state, info.members.size());
        for (CompletionInfo::Member const& member : info.members)
        {
            pushNumber(state, member.guid);
            pushNumber(state, member.classId);
            pushString(state, member.name.c_str());
            pushNumber(state, member.ratingBefore);
            pushNumber(state, member.ratingAfter);
            pushNumber(state, member.ratingChange);
            pushNumber(state, member.bestTimeMs);
            pushNumber(state, member.flags);
        }
        return static_cast<int>(resultCount);
    }

    int LuaHistoryInfo(lua_State* state)
    {
        Sync(state);
        HistoryInfo info;
        {
            const std::lock_guard lock(g_snapshotMutex);
            info = g_historyInfo;
        }

        size_t resultCount = 6;
        for (HistoryInfo::Run const& run : info.runs)
        {
            resultCount += 16;
            resultCount += run.members.size() * 3;
            resultCount += run.splits.size() * 2;
        }
        if (!EnsureLuaResultStack(state, resultCount, "history-info"))
            return 0;

        const auto pushNumber = lua_pushnumber;
        const auto pushString = lua_pushstring;
        pushNumber(state, info.generation);
        pushNumber(state, info.scope);
        pushNumber(state, info.seasonId);
        pushNumber(state, info.requestedMapId);
        pushNumber(state, info.overallRating);
        pushNumber(state, info.runs.size());
        size_t pushed = 6;
        for (HistoryInfo::Run const& run : info.runs)
        {
            std::string const runId = std::to_string(run.id);
            pushString(state, runId.c_str());
            pushNumber(state, run.mapId);
            pushString(state, run.mapName.c_str());
            pushNumber(state, run.level);
            pushNumber(state, run.completionTimeMs);
            pushNumber(state, run.timeLimitMs);
            pushNumber(state, run.deaths);
            pushNumber(state, run.completedOnTime);
            pushNumber(state, run.completedAt);
            pushNumber(state, run.affixCount);
            for (uint16_t affixId : run.affixIds)
                pushNumber(state, affixId);
            pushNumber(state, run.members.size());
            pushed += 15;
            for (HistoryInfo::Member const& member : run.members)
            {
                pushNumber(state, member.guid);
                pushNumber(state, member.classId);
                pushString(state, member.name.c_str());
                pushed += 3;
            }
            pushNumber(state, run.splits.size());
            ++pushed;
            for (HistoryInfo::Split const& split : run.splits)
            {
                pushString(state, split.name.c_str());
                pushNumber(state, split.completionTimeMs);
                pushed += 2;
            }
        }
        if (pushed != resultCount)
        {
            WLOG_WARN(
                "mythic-plus: Lua history result count mismatch pushed=%zu expected=%zu",
                pushed, resultCount);
            return 0;
        }
        return static_cast<int>(pushed);
    }

template<class T>void ClearValue(T& value){const auto revision=value.generation+1;value={};value.generation=revision?revision:1;}
void ClearStores(){
 std::lock_guard lock(g_snapshotMutex);
 ClearValue(g_snapshot);ClearValue(g_bossInfo);ClearValue(g_keystoneInfo);ClearValue(g_completionInfo);ClearValue(g_historyInfo);ClearValue(g_lootSpecializationInfo);
 g_rewardToasts.clear();g_lastRequestMs=0;changed=true;rewardChanged=true;
}
void Reset(void*) noexcept {try{ClearStores();session=0;player=0;currentMap=0;}catch(...){}}
bool Sync(lua_State* state){
 const auto epoch=ChallengeModeEnabled()?protocol::SessionGeneration(state):0;
 const auto guid=epoch&&ClntObjMgrGetCurrent()?ClntObjMgrGetActivePlayer().m_guid:0;
 const auto map=guid?ClntObjMgrGetMapID():0;
 if(epoch!=session||guid!=player||map!=currentMap){ClearStores();session=epoch;player=guid;currentMap=map;}
 return epoch&&guid;
}
void Receive(uint16_t opcode,const uint8_t* data,uint32_t size,uint64_t epoch,void*) noexcept {
 if(!ChallengeModeEnabled()||!epoch||!data||!ClntObjMgrGetCurrent())return;
 const auto guid=ClntObjMgrGetActivePlayer().m_guid;if(!guid)return;
 try{
  const auto map=ClntObjMgrGetMapID();
  if(session!=epoch||player!=guid||currentMap!=map){ClearStores();session=epoch;player=guid;currentMap=map;}
  const std::span<const uint8_t> bytes(data,size);
  switch(opcode){
   case opcodes::SmsgChallengeModeState:OnSnapshot(bytes,nullptr);break;
   case opcodes::SmsgChallengeModeBossInfo:OnBossInfo(bytes,nullptr);break;
   case opcodes::SmsgChallengeModeKeystoneInfo:OnKeystoneInfo(bytes,nullptr);break;
   case opcodes::SmsgChallengeModeComplete:OnCompletionInfo(bytes,nullptr);break;
   case opcodes::SmsgChallengeModeHistory:OnHistoryInfo(bytes,nullptr);break;
   case opcodes::SmsgLootSpecialization:OnLootSpecialization(bytes,nullptr);break;
   case opcodes::SmsgDisplayToast:OnDisplayToast(bytes,nullptr);break;
  }
 }catch(...){Reset(nullptr);}
}
int Pump(lua_State* state){
 Sync(state);const bool update=std::exchange(changed,false),reward=std::exchange(rewardChanged,false);
 // Capture invokes handlers under protocol mutex; all Lua delivery occurs here later.
 if(update)FrameScript_Execute("if wxlwow and wxlwow.challenge_mode and wxlwow.challenge_mode._NativeChanged then wxlwow.challenge_mode._NativeChanged() end","native-challenge-update",nullptr);
 if(reward)FrameScript_Execute("if wxlwow and wxlwow.challenge_mode and wxlwow.challenge_mode._NativeRewardToastChanged then wxlwow.challenge_mode._NativeRewardToastChanged() end","native-challenge-toast",nullptr);
 return 0;
}
}
bool Install(){
 if(!registered){
  const protocol::NativeDeclaration ids[]={
   {opcodes::CmsgChallengeModeRequestState,true},{opcodes::CmsgStartChallengeMode,true},{opcodes::CmsgChallengeModeRequestHistory,true},{opcodes::CmsgSetLootSpecialization,true},
   {opcodes::SmsgChallengeModeState,false,Receive,Reset},{opcodes::SmsgChallengeModeBossInfo,false,Receive,Reset},{opcodes::SmsgChallengeModeKeystoneInfo,false,Receive,Reset},
   {opcodes::SmsgChallengeModeComplete,false,Receive,Reset},{opcodes::SmsgChallengeModeHistory,false,Receive,Reset},{opcodes::SmsgLootSpecialization,false,Receive,Reset},{opcodes::SmsgDisplayToast,false,Receive,Reset}};
  registered=protocol::RegisterNativeDispatch(ids,uint32_t(std::size(ids)));
 }
 if(!registered)return false;
 bool ok=true;
        ok &= framescript::RegisterFunction(
            "_WXLWOW_MYTHIC_REQUEST", &LuaRequest);
        ok &= framescript::RegisterFunction(
            "_WXLWOW_MYTHIC_HISTORY_REQUEST", &LuaRequestHistory);
        ok &= framescript::RegisterFunction(
            "_WXLWOW_MYTHIC_START", &LuaStartChallengeMode);
        ok &= framescript::RegisterFunction(
            "_WXLWOW_MYTHIC_DEVELOPMENT_SET_KEYSTONE",
            &LuaDevelopmentSetKeystone);
        ok &= framescript::RegisterFunction(
            "_WXLWOW_MYTHIC_REROLL_KEYSTONE", &LuaRerollKeystone);
        ok &= framescript::RegisterFunction(
            "_WXLWOW_MYTHIC_ABANDON", &LuaAbandonChallengeMode);
        ok &= framescript::RegisterFunction(
            "_WXLWOW_MYTHIC_ACCEPT_KEYSTONE_UPGRADE",
            &LuaAcceptKeystoneUpgrade);
        ok &= framescript::RegisterFunction(
            "_WXLWOW_MYTHIC_POP_REWARD_TOAST", &LuaPopRewardToast);
        ok &= framescript::RegisterFunction(
            "_WXLWOW_MYTHIC_SNAPSHOT", &LuaSnapshot);
        ok &= framescript::RegisterFunction(
            "_WXLWOW_MYTHIC_BOSS_INFO", &LuaBossInfo);
        ok &= framescript::RegisterFunction(
            "_WXLWOW_MYTHIC_KEYSTONE_INFO", &LuaKeystoneInfo);
        ok &= framescript::RegisterFunction(
            "_WXLWOW_MYTHIC_COMPLETION_INFO", &LuaCompletionInfo);
        ok &= framescript::RegisterFunction(
            "_WXLWOW_MYTHIC_HISTORY_INFO", &LuaHistoryInfo);
        ok &= framescript::RegisterFunction(
            "_WXLWOW_SET_LOOT_SPECIALIZATION", &LuaSetLootSpecialization);
        ok &= framescript::RegisterFunction(
            "_WXLWOW_LOOT_SPECIALIZATION_INFO", &LuaLootSpecializationInfo);

 FrameScript_RegisterFunction("WXL_NativeChallengePump",Pump);
 return ok;
}
void UiReady(){
 if(registered&&ChallengeModeEnabled())FrameScript_Execute(kBootstrap,"native-challenge-api",nullptr);
 InstallMythicUi();
}
void Destroy() noexcept {Reset(nullptr);DestroyPortals();}
}
