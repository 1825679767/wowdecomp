// Donor Lua API and compatibility policy, unchanged except deferred owner pump.
// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
namespace wxl::native::challenge_mode {
    constexpr char kBootstrap[] = R"lua(
do
    wxlwow = wxlwow or {}
    wxlwow.challenge_mode = wxlwow.challenge_mode or {}
    local CM = wxlwow.challenge_mode

    if _WXLWOW_MYTHIC_REQUEST then
        CM.Request = _WXLWOW_MYTHIC_REQUEST
        _WXLWOW_MYTHIC_REQUEST = nil
    end
    if _WXLWOW_MYTHIC_HISTORY_REQUEST then
        CM.RequestHistory = _WXLWOW_MYTHIC_HISTORY_REQUEST
        _WXLWOW_MYTHIC_HISTORY_REQUEST = nil
    end
    if _WXLWOW_MYTHIC_START then
        CM.StartChallengeMode = _WXLWOW_MYTHIC_START
        _WXLWOW_MYTHIC_START = nil
    end
    if _WXLWOW_MYTHIC_DEVELOPMENT_SET_KEYSTONE then
        CM.DevelopmentSetKeystone = _WXLWOW_MYTHIC_DEVELOPMENT_SET_KEYSTONE
        _WXLWOW_MYTHIC_DEVELOPMENT_SET_KEYSTONE = nil
    end
    if _WXLWOW_MYTHIC_REROLL_KEYSTONE then
        CM.RerollKeystone = _WXLWOW_MYTHIC_REROLL_KEYSTONE
        _WXLWOW_MYTHIC_REROLL_KEYSTONE = nil
    end
    if _WXLWOW_MYTHIC_ABANDON then
        CM.AbandonChallengeMode = _WXLWOW_MYTHIC_ABANDON
        _WXLWOW_MYTHIC_ABANDON = nil
    end
    if _WXLWOW_MYTHIC_ACCEPT_KEYSTONE_UPGRADE then
        CM.AcceptKeystoneUpgrade = _WXLWOW_MYTHIC_ACCEPT_KEYSTONE_UPGRADE
        _WXLWOW_MYTHIC_ACCEPT_KEYSTONE_UPGRADE = nil
    end
    if _WXLWOW_MYTHIC_POP_REWARD_TOAST then
        CM.NativePopRewardToast = _WXLWOW_MYTHIC_POP_REWARD_TOAST
        _WXLWOW_MYTHIC_POP_REWARD_TOAST = nil
    end
    if _WXLWOW_MYTHIC_SNAPSHOT then
        CM.NativeSnapshot = _WXLWOW_MYTHIC_SNAPSHOT
        _WXLWOW_MYTHIC_SNAPSHOT = nil
    end
    if _WXLWOW_MYTHIC_BOSS_INFO then
        CM.NativeBossInfo = _WXLWOW_MYTHIC_BOSS_INFO
        _WXLWOW_MYTHIC_BOSS_INFO = nil
    end
    if _WXLWOW_MYTHIC_KEYSTONE_INFO then
        CM.NativeKeystoneInfo = _WXLWOW_MYTHIC_KEYSTONE_INFO
        _WXLWOW_MYTHIC_KEYSTONE_INFO = nil
    end
    if _WXLWOW_MYTHIC_COMPLETION_INFO then
        CM.NativeCompletionInfo = _WXLWOW_MYTHIC_COMPLETION_INFO
        _WXLWOW_MYTHIC_COMPLETION_INFO = nil
    end
    if _WXLWOW_MYTHIC_HISTORY_INFO then
        CM.NativeHistoryInfo = _WXLWOW_MYTHIC_HISTORY_INFO
        _WXLWOW_MYTHIC_HISTORY_INFO = nil
    end
    if _WXLWOW_SET_LOOT_SPECIALIZATION then
        CM.SetLootSpecialization = _WXLWOW_SET_LOOT_SPECIALIZATION
        _WXLWOW_SET_LOOT_SPECIALIZATION = nil
    end
    if _WXLWOW_LOOT_SPECIALIZATION_INFO then
        CM.NativeLootSpecializationInfo = _WXLWOW_LOOT_SPECIALIZATION_INFO
        _WXLWOW_LOOT_SPECIALIZATION_INFO = nil
    end

    CM.revision = CM.revision or 0
    CM.state = CM.state or 0
    CM.reason = CM.reason or 0
    CM.flags = CM.flags or 0
    CM.sequence = CM.sequence or 0
    CM.bossRevision = CM.bossRevision or 0
    CM.bossesCompletedMask = CM.bossesCompletedMask or 0
    CM.bossNames = CM.bossNames or {}
    CM.bossCompletionTimesMs = CM.bossCompletionTimesMs or {}
    CM.personalBestTimeMs = CM.personalBestTimeMs or 0
    CM.realmBestTimeMs = CM.realmBestTimeMs or 0
    CM.affixIds = CM.affixIds or {}
    CM.seasonId = CM.seasonId or 0
    CM.weekIndex = CM.weekIndex or 0
    CM.receivedAt = CM.receivedAt or GetTime()
    CM.keystoneRevision = CM.keystoneRevision or 0
    CM.keystoneAffixIds = CM.keystoneAffixIds or {}
    CM.completionRevision = CM.completionRevision or 0
    CM.historyRevision = CM.historyRevision or 0
    CM.historyInfo = CM.historyInfo or { runs = {} }
    CM.lootSpecializationRevision = CM.lootSpecializationRevision or 0
    CM.lootSpecialization = CM.lootSpecialization or 0

    local rejectionText = {
        [1] = "Protocol mismatch",
        [2] = "GM access is required for development activation",
        [3] = "You must be inside a Mythic dungeon",
        [4] = "Only the keystone owner can do that",
        [5] = "The party must be together in this instance",
        [6] = "A party member is in combat",
        [7] = "This instance already has an active run",
        [8] = "The requested keystone level is invalid",
        [9] = "This dungeon is disabled or missing its Mythic+ configuration",
        [10] = "This instance already has defeated bosses",
        [11] = "You do not have a Mythic Keystone",
        [12] = "That keystone belongs to a different dungeon",
        [13] = "There is no in-progress Mythic+ run to abandon",
    }

    local function chat(message, r, g, b)
        if DEFAULT_CHAT_FRAME then
            DEFAULT_CHAT_FRAME:AddMessage(message, r or 1, g or 0.82, b or 0)
        end
    end

    local function emitRetailEvent(event, ...)
        local bridge = WXL_RetailEventBridge
        if bridge and bridge.Dispatch then
            return bridge:Dispatch(event, ...)
        end
        return false
    end
    CM.EmitRetailEvent = emitRetailEvent

    local function affixSignature(seasonId, weekIndex, affixIds)
        local parts = { tostring(seasonId or 0), tostring(weekIndex or 0) }
        for _, affixId in ipairs(affixIds or {}) do
            table.insert(parts, tostring(affixId))
        end
        return table.concat(parts, ":")
    end

    local function pullBossInfo(force)
        if not CM.NativeBossInfo then return end
        local values = { CM.NativeBossInfo() }
        local revision = values[1] or 0
        if not force and revision == CM.bossRevision then return end

        local oldRevision = CM.bossRevision or 0
        local oldAffixSignature = affixSignature(CM.bossInfoSeasonId,
            CM.bossInfoWeekIndex, CM.bossInfoAffixIds)
        CM.bossRevision = revision
        CM.bossInfoSequence = values[2] or 0
        CM.bossInfoMapId = values[3] or 0
        CM.bossInfoInstanceId = values[4] or 0
        CM.bossInfoCompletedMask = values[5] or 0
        local count = math.max(0, math.min(32, values[6] or 0))
        CM.bossInfoSeasonId = values[7] or 0
        CM.bossInfoWeekIndex = values[8] or 0
        local affixCount = math.max(0, math.min(4, values[9] or 0))
        CM.bossInfoAffixIds = {}
        for index = 1, affixCount do
            local affixId = values[9 + index]
            if affixId and affixId ~= 0 then
                table.insert(CM.bossInfoAffixIds, affixId)
            end
        end
        CM.bossInfoPersonalBestTimeMs = values[14] or 0
        CM.bossInfoRealmBestTimeMs = values[15] or 0
        CM.bossInfoNames = {}
        CM.bossInfoCompletionTimesMs = {}
        for index = 1, count do
            CM.bossInfoNames[index] = values[15 + index]
            CM.bossInfoCompletionTimesMs[index] = values[15 + count + index] or 0
        end

        if CM.state ~= 0 and CM.mapId == CM.bossInfoMapId and
            CM.instanceId == CM.bossInfoInstanceId and
            CM.sequence == CM.bossInfoSequence then
            CM.bossesCompletedMask = CM.bossInfoCompletedMask
            CM.bossNames = CM.bossInfoNames
            CM.bossCompletionTimesMs = CM.bossInfoCompletionTimesMs
            CM.personalBestTimeMs = CM.bossInfoPersonalBestTimeMs
            CM.realmBestTimeMs = CM.bossInfoRealmBestTimeMs
            CM.seasonId = CM.bossInfoSeasonId
            CM.weekIndex = CM.bossInfoWeekIndex
            CM.affixIds = CM.bossInfoAffixIds
        end

        local newAffixSignature = affixSignature(CM.bossInfoSeasonId,
            CM.bossInfoWeekIndex, CM.bossInfoAffixIds)
        if revision > 0 and revision ~= oldRevision and
            newAffixSignature ~= oldAffixSignature then
            emitRetailEvent("MYTHIC_PLUS_CURRENT_AFFIX_UPDATE")
        end
    end

    local function pull(force)
        if not CM.NativeSnapshot then return end
        local revision, state, reason, flags, sequence, mapId, instanceId,
            level, bossesKilled, bossesTotal, deaths, timeLimitMs,
            elapsedMs, deathPenaltyMs, forcesEarned, forcesTotal,
            damagePercent, healthPercent, countdownMs = CM.NativeSnapshot()
        if not force and revision == CM.revision then return end

        local oldRevision = CM.revision or 0
        local oldReason = CM.reason
        local oldState = CM.state or 0
        local oldSequence = CM.sequence or 0
        local oldMapId = CM.mapId
        local oldInstanceId = CM.instanceId
        local oldLevel = CM.level or 0
        CM.revision = revision or 0
        CM.state = state or 0
        CM.reason = reason or 0
        CM.flags = flags or 0
        CM.sequence = sequence or 0
        CM.mapId = mapId or 0
        CM.instanceId = instanceId or 0
        CM.level = level or 0
        CM.bossesKilled = bossesKilled or 0
        CM.bossesTotal = bossesTotal or 0
        CM.deaths = deaths or 0
        CM.timeLimitMs = timeLimitMs or 0
        CM.elapsedMs = elapsedMs or 0
        CM.deathPenaltyMs = deathPenaltyMs or 0
        CM.forcesEarned = forcesEarned or 0
        CM.forcesTotal = forcesTotal or 0
        CM.damagePercent = damagePercent or 0
        CM.healthPercent = healthPercent or 0
        CM.countdownMs = countdownMs or 0
        CM.receivedAt = GetTime()

        local sameInstance = oldMapId == CM.mapId and
            oldInstanceId == CM.instanceId
        local replacedRun = oldState ~= 0 and CM.state ~= 0 and sameInstance and
)lua"
R"lua(
            (oldLevel ~= CM.level or
                (oldState == 3 and CM.state ~= 3) or
                CM.sequence < oldSequence)

        if CM.state == 0 then
            CM.bossesCompletedMask = 0
            CM.bossNames = {}
            CM.bossCompletionTimesMs = {}
            CM.personalBestTimeMs = 0
            CM.realmBestTimeMs = 0
            CM.affixIds = {}
        elseif not sameInstance or replacedRun then
            CM.bossesCompletedMask = 0
            CM.bossNames = {}
            CM.bossCompletionTimesMs = {}
            CM.personalBestTimeMs = 0
            CM.realmBestTimeMs = 0
        end
        pullBossInfo(true)

        if CM.state == 0 and CM.reason ~= 0 and CM.reason ~= oldReason then
            chat("Mythic+: " .. (rejectionText[CM.reason] or "Request rejected"), 1, 0.25, 0.2)
        end

        if CM._AddonChanged then CM._AddonChanged() end
        if (revision or 0) > 0 and (revision or 0) ~= oldRevision and
            oldState ~= 0 and CM.state == 0 then
            emitRetailEvent("CHALLENGE_MODE_RESET")
        end
    end

    local function pullKeystone(force)
        if not CM.NativeKeystoneInfo then return end
        local values = { CM.NativeKeystoneInfo() }
        local revision = values[1] or 0
        if not force and revision == CM.keystoneRevision then return end

        local oldRevision = CM.keystoneRevision or 0
        local oldKeystoneSignature = table.concat({
            tostring(bit.band(CM.keystoneFlags or 0, 1)),
            tostring(CM.keystoneMapId or 0),
            tostring(CM.keystoneLevel or 0),
            affixSignature(CM.keystoneSeasonId, 0, CM.keystoneAffixIds),
        }, ":")
        CM.keystoneRevision = revision
        CM.keystoneFlags = values[2] or 0
        CM.keystoneMapId = values[3] or 0
        CM.keystoneLevel = values[4] or 0
        CM.keystoneSeasonId = values[5] or 0
        local count = math.max(0, math.min(4, values[6] or 0))
        CM.keystoneAffixIds = {}
        for index = 1, count do
            local affixId = values[6 + index]
            if affixId and affixId ~= 0 then
                table.insert(CM.keystoneAffixIds, affixId)
            end
        end
        CM.keystoneTimeLimitMs = values[11] or 0
        CM.keystoneMapName = values[12] or ""
        CM.challengeMapInfo = CM.challengeMapInfo or {}
        if CM.keystoneMapId ~= 0 and CM.keystoneMapName ~= "" then
            CM.challengeMapInfo[CM.keystoneMapId] = {
                CM.keystoneMapName, CM.keystoneTimeLimitMs
            }
        end
        if CM._AddonKeystoneChanged then CM._AddonKeystoneChanged() end
        local newKeystoneSignature = table.concat({
            tostring(bit.band(CM.keystoneFlags or 0, 1)),
            tostring(CM.keystoneMapId or 0),
            tostring(CM.keystoneLevel or 0),
            affixSignature(CM.keystoneSeasonId, 0, CM.keystoneAffixIds),
        }, ":")
        if revision > 0 and revision ~= oldRevision and
            newKeystoneSignature ~= oldKeystoneSignature then
            emitRetailEvent("CHALLENGE_MODE_MAPS_UPDATE")
        end
    end

    local function pullCompletion(force)
        if not CM.NativeCompletionInfo then return end
        local values = { CM.NativeCompletionInfo() }
        local revision = values[1] or 0
        if not force and revision == CM.completionRevision then return end

        CM.completionRevision = revision
        if (values[2] or 0) == 0 then
            CM.completionInfo = nil
            if CM._AddonCompletionChanged then
                CM._AddonCompletionChanged(nil)
            end
            return
        end
        local memberCount = math.max(0, math.min(5, values[24] or 0))
        local members = {}
        for index = 1, memberCount do
            local offset = 25 + ((index - 1) * 8)
            members[index] = {
                memberGUID = values[offset] or 0,
                classID = values[offset + 1] or 0,
                name = values[offset + 2],
                ratingBefore = values[offset + 3] or 0,
                ratingAfter = values[offset + 4] or 0,
                ratingChange = values[offset + 5] or 0,
                bestTime = values[offset + 6] or 0,
                newPersonalBest = bit.band(values[offset + 7] or 0, 1) ~= 0,
            }
        end
        CM.completionInfo = {
            mapChallengeModeID = values[2] or 0,
            instanceID = values[3] or 0,
            level = values[4] or 0,
            time = values[5] or 0,
            timeLimit = values[6] or 0,
            deathPenalty = values[7] or 0,
            deaths = values[8] or 0,
            onTime = (values[9] or 0) == 1,
            keystoneUpgradeLevels = values[10] or 0,
            oldKeystoneMapID = values[11] or 0,
            oldKeystoneLevel = values[12] or 0,
            newKeystoneMapID = values[13] or 0,
            newKeystoneLevel = values[14] or 0,
            seasonID = values[15] or 0,
            previousRealmBestTime = values[16] or 0,
            realmBestTime = values[17] or 0,
            previousPersonalBestTime = values[18] or 0,
            personalBestTime = values[19] or 0,
            ratingBefore = values[20] or 0,
            ratingAfter = values[21] or 0,
            ratingChange = values[22] or 0,
            newPersonalBest = bit.band(values[23] or 0, 1) ~= 0,
            newRealmRecord = bit.band(values[23] or 0, 2) ~= 0,
            members = members,
            practiceRun = false,
        }
        if CM._AddonCompletionChanged then
            CM._AddonCompletionChanged(CM.completionInfo)
        end
        local completionSignature = table.concat({
            tostring(CM.completionInfo.mapChallengeModeID or 0),
            tostring(CM.completionInfo.instanceID or 0),
            tostring(CM.completionInfo.level or 0),
            tostring(CM.completionInfo.time or 0),
            tostring(CM.completionInfo.ratingAfter or 0),
        }, ":")
        if completionSignature ~= CM.lastRetailCompletionSignature then
            CM.lastRetailCompletionSignature = completionSignature
            emitRetailEvent("CHALLENGE_MODE_COMPLETED")
            if CM.RequestHistory then
                CM.RequestHistory(0, 0, CM.completionInfo.seasonID or 1)
            end
        end
    end

    local function pullHistory(force)
        if not CM.NativeHistoryInfo then return end
        local values = { CM.NativeHistoryInfo() }
        local revision = values[1] or 0
        if not force and revision == CM.historyRevision then return end

        local oldRevision = CM.historyRevision or 0
        CM.historyRevision = revision
        local runCount = math.max(0, math.min(20, values[6] or 0))
        local cursor = 7
        local runs = {}
        for runIndex = 1, runCount do
            local run = {
                runID = values[cursor],
                mapChallengeModeID = values[cursor + 1] or 0,
                mapName = values[cursor + 2] or "",
                level = values[cursor + 3] or 0,
                time = values[cursor + 4] or 0,
                timeLimit = values[cursor + 5] or 0,
                deaths = values[cursor + 6] or 0,
                onTime = (values[cursor + 7] or 0) == 1,
                completedAt = values[cursor + 8] or 0,
                affixes = {},
                members = {},
                splits = {},
            }
            local affixCount = math.max(0,
                math.min(4, values[cursor + 9] or 0))
            for affixIndex = 1, affixCount do
                local affixID = values[cursor + 9 + affixIndex]
                if affixID and affixID ~= 0 then
                    table.insert(run.affixes, affixID)
                end
            end
            local memberCount = math.max(0,
                math.min(5, values[cursor + 14] or 0))
            cursor = cursor + 15
            for memberIndex = 1, memberCount do
                table.insert(run.members, {
                    guid = values[cursor] or 0,
                    classID = values[cursor + 1] or 0,
                    name = values[cursor + 2] or "Unknown",
                })
                cursor = cursor + 3
            end
            local splitCount = math.max(0,
                math.min(32, values[cursor] or 0))
            cursor = cursor + 1
            for splitIndex = 1, splitCount do
                table.insert(run.splits, {
                    name = values[cursor] or "Boss",
                    time = values[cursor + 1] or 0,
                })
                cursor = cursor + 2
            end
            if run.mapChallengeModeID ~= 0 and run.mapName ~= "" then
                CM.challengeMapInfo = CM.challengeMapInfo or {}
                CM.challengeMapInfo[run.mapChallengeModeID] = {
                    run.mapName, run.timeLimit
                }
            end
            table.insert(runs, run)
        end
        CM.historyInfo = {
            revision = revision,
            scope = values[2] or 0,
            seasonID = values[3] or 0,
            requestedMapID = values[4] or 0,
            overallRating = values[5] or 0,
            runs = runs,
        }
        if CM._AddonHistoryChanged then
            CM._AddonHistoryChanged(CM.historyInfo)
        end
        if revision > 0 and revision ~= oldRevision then
            if CM.historyInfo.scope == 1 then
                emitRetailEvent("CHALLENGE_MODE_LEADERS_UPDATE")
)lua"
R"lua(
            else
                emitRetailEvent("CHALLENGE_MODE_MAPS_UPDATE")
            end
        end
    end

    local function pullLootSpecialization(force)
        if not CM.NativeLootSpecializationInfo then return end
        local revision, selection = CM.NativeLootSpecializationInfo()
        if not force and revision == CM.lootSpecializationRevision then return end
        CM.lootSpecializationRevision = revision or 0
        CM.lootSpecialization = selection or 0
        if CM._AddonLootSpecializationChanged then
            CM._AddonLootSpecializationChanged(CM.lootSpecialization)
        end
    end

    CM._NativeChanged = function()
        pull(true)
    end
    CM._NativeKeystoneChanged = function()
        pullKeystone(true)
    end
    CM._NativeCompletionChanged = function()
        pullCompletion(true)
    end
    CM._NativeHistoryChanged = function()
        pullHistory(true)
    end
    CM._NativeLootSpecializationChanged = function()
        pullLootSpecialization(true)
    end
    CM._NativeRewardToastChanged = function()
        if not CM.NativePopRewardToast then return end
        while true do
            local toastType, method, flags, quality, itemId, quantity, name =
                CM.NativePopRewardToast()
            if toastType == nil then break end
            if CM._AddonRewardToast then
                CM._AddonRewardToast({
                    type = toastType, method = method, flags = flags,
                    quality = quality, itemId = itemId, quantity = quantity,
                    name = name,
                })
            end
        end
    end

    C_ChallengeMode = C_ChallengeMode or {}
    C_ChallengeMode.IsChallengeModeActive = function()
        return CM.state == 1 or CM.state == 2
    end
    C_ChallengeMode.GetActiveChallengeMapID = function()
        if CM.state == 0 then return nil end
        return CM.mapId
    end
    C_ChallengeMode.GetActiveKeystoneInfo = function()
        return CM.level or 0, CM.affixIds or {}, true
    end
    C_ChallengeMode.GetSlottedKeystoneInfo = function()
        if bit.band(CM.keystoneFlags or 0, 1) == 0 then return nil end
        return CM.keystoneMapId, CM.keystoneAffixIds,
            CM.keystoneLevel, CM.keystoneSeasonId
    end
    C_ChallengeMode.GetChallengeCompletionInfo = function()
        return CM.completionInfo
    end
    C_ChallengeMode.RequestHistory = function(scope, mapId, seasonId)
        if not CM.RequestHistory then return false end
        return CM.RequestHistory(scope or 0, mapId or 0,
            seasonId or CM.keystoneSeasonId or CM.seasonId or 1)
    end
    C_ChallengeMode.GetHistory = function()
        return CM.historyInfo
    end
    C_ChallengeMode.RerollKeystone = function()
        if not CM.RerollKeystone then return false end
        return CM.RerollKeystone()
    end
    C_ChallengeMode.AbandonChallengeMode = function()
        if not CM.AbandonChallengeMode then return false end
        return CM.AbandonChallengeMode()
    end
    C_ChallengeMode.AcceptKeystoneUpgrade = function()
        if not CM.AcceptKeystoneUpgrade then return false end
        return CM.AcceptKeystoneUpgrade()
    end
    C_ChallengeMode.GetLootSpecialization = function()
        return CM.lootSpecialization or 0
    end
    C_ChallengeMode.SetLootSpecialization = function(selection)
        if not CM.SetLootSpecialization then return false end
        return CM.SetLootSpecialization(selection or 0)
    end
    local affixInfo = {
        [3] = { "Volcanic", "While in combat, enemies periodically cause flames to erupt beneath distant players.", 3000004, "Interface\\Icons\\Spell_Shaman_LavaBurst" },
        [6] = { "Raging", "Non-boss enemies enrage at low health and become resistant to crowd control.", 3000000, "Interface\\Icons\\Ability_Druid_ChallangingRoar" },
        [7] = { "Bolstering", "When a non-boss enemy dies, it empowers nearby enemies.", 3000005, "Interface\\Icons\\Ability_Warrior_BattleShout" },
        [8] = { "Sanguine", "Non-boss enemies leave pools that damage players and heal enemies.", 3000006, "Interface\\Icons\\Spell_Shadow_BloodBoil" },
        [9] = { "Tyrannical", "Bosses have 25% more health and deal 15% more damage.", 2999999, "Interface\\Icons\\Achievement_Boss_KingYmiron_01" },
        [10] = { "Fortified", "Non-boss enemies have 20% more health and deal 20% more damage.", 2999998, "Interface\\Icons\\Ability_Warrior_DefensiveStance" },
        [13] = { "Explosive", "Enemies periodically summon Explosive Orbs while in combat.", 3000007, "Interface\\Icons\\Spell_Fire_Fireball02" },
        [14] = { "Quaking", "Players periodically emit a damaging, interrupting shockwave.", 3000008, "Interface\\Icons\\Spell_Nature_Earthquake" },
        [124] = { "Storming", "Enemies periodically summon damaging whirlwinds while in combat.", 3000002, "Interface\\Icons\\Spell_Nature_Cyclone" },
    }
    local challengeScaling = {
        [2]=7, [3]=14, [4]=23, [5]=31, [6]=40, [7]=50, [8]=61,
        [9]=72, [10]=84, [11]=102, [12]=122, [13]=145, [14]=169,
        [15]=196, [16]=225, [17]=255, [18]=286, [19]=318,
        [20]=351, [21]=385, [22]=420, [23]=456, [24]=493,
        [25]=531, [26]=570, [27]=610, [28]=651, [29]=693, [30]=736,
    }
    local affixRotation = {
        {10, 6, 3}, {9, 7, 124}, {10, 8, 14}, {9, 6, 13},
        {10, 7, 3}, {9, 8, 124}, {10, 6, 14}, {9, 7, 13},
    }
    local function previewAffixes(level)
        level = math.max(2, math.min(30, math.floor(tonumber(level) or 2)))
        local now = GetServerTime and GetServerTime() or (time and time()) or 1785308400
        local elapsed = math.max(0, now - 1785308400)
        local week = (math.floor(elapsed / 604800) % #affixRotation) + 1
        local rotation = affixRotation[week]
        local result = {}
        if level >= 2 then table.insert(result, rotation[1]) end
        if level >= 4 then table.insert(result, rotation[2]) end
        if level >= 7 then table.insert(result, rotation[3]) end
        return result
    end
    C_ChallengeMode.GetAffixInfo = function(affixId)
        local info = affixInfo[affixId]
        if not info then
            return "Unknown Affix", "Affix " .. tostring(affixId), "Interface\\Icons\\INV_Misc_QuestionMark"
        end
        local texture = GetSpellTexture and GetSpellTexture(info[3]) or nil
        if texture == nil or texture == "" or type(texture) == "number" then texture = info[4] end
        return info[1], info[2], texture
    end
    C_ChallengeMode.GetPreviewAffixes = previewAffixes
    C_ChallengeMode.GetDeathCount = function()
        return CM.deaths or 0, (CM.deathPenaltyMs or 0) / 1000
    end
    C_ChallengeMode.GetPowerLevelDamageHealthMod = function(powerLevel)
        if powerLevel then
            local modifier = challengeScaling[math.floor(tonumber(powerLevel) or 0)] or 0
            return modifier, modifier
        end
        return CM.damagePercent or 0, CM.healthPercent or 0
    end
    local challengeMaps = {
        [389] = { "Ragefire Chasm", 1200 },
        [574] = { "Utgarde Keep", 1800 }, [575] = { "Utgarde Pinnacle", 1800 },
        [576] = { "The Nexus", 1800 }, [578] = { "The Oculus", 2100 },
        [595] = { "The Culling of Stratholme", 1500 }, [599] = { "Halls of Stone", 2100 },
        [600] = { "Drak'Tharon Keep", 1800 }, [601] = { "Azjol-Nerub", 1200 },
        [602] = { "Halls of Lightning", 1800 }, [604] = { "Gundrak", 1800 },
        [608] = { "The Violet Hold", 2100 }, [619] = { "Ahn'kahet: The Old Kingdom", 1800 },
        [632] = { "The Forge of Souls", 1200 }, [650] = { "Trial of the Champion", 1200 },
        [658] = { "Pit of Saron", 1800 }, [668] = { "Halls of Reflection", 1800 },
    }
    C_ChallengeMode.GetMapInfo = function(mapId)
        mapId = mapId or CM.mapId or 0
        local data = challengeMaps[mapId]
        local name = data and data[1] or ("Map " .. tostring(mapId))
        local timeLimit = data and data[2] or 0
        local cached = CM.challengeMapInfo and CM.challengeMapInfo[mapId]
        if cached then
            name = cached[1] or name
            if (cached[2] or 0) > 0 then
                timeLimit = cached[2] / 1000
            end
        end
        if mapId == CM.keystoneMapId then
            if CM.keystoneMapName and CM.keystoneMapName ~= "" then
                name = CM.keystoneMapName
            end
            if (CM.keystoneTimeLimitMs or 0) > 0 then
                timeLimit = CM.keystoneTimeLimitMs / 1000
            end
        end
        if mapId == CM.mapId and (CM.timeLimitMs or 0) > 0 then
            if GetRealZoneText then
                local currentName = GetRealZoneText()
                if currentName and currentName ~= "" then name = currentName end
            end
            timeLimit = CM.timeLimitMs / 1000
        end
        return name, mapId, timeLimit, nil, nil
    end

    local function installSlashCommand()
        if CM.slashInstalled or type(SlashCmdList) ~= "table" then return end
        SLASH_WXLMYTHIC1 = "/wxlmythic"
        SlashCmdList.WXLMYTHIC = function(message)
            local command, value = string.match(message or "", "^(%S*)%s*(%S*)")
            command = string.lower(command or "")
            if command == "key" then
                local level = tonumber(value) or 2
                if not CM.DevelopmentSetKeystone or
                    not CM.DevelopmentSetKeystone(level) then
                    chat("Usage: /wxlmythic key <2-30>", 1, 0.25, 0.2)
                end
            else
                if CM.Request then CM.Request() end
                chat("Mythic+: state=" .. tostring(CM.state) ..
                    " sequence=" .. tostring(CM.sequence) ..
                    " instance=" .. tostring(CM.instanceId) ..
                    " forces=" .. tostring(CM.forcesEarned) .. "/" ..
                        tostring(CM.forcesTotal) ..
                    " scaling=" .. tostring(CM.healthPercent) .. "%" ..
                    " season=" .. tostring(CM.seasonId) ..
                    " week=" .. tostring(CM.weekIndex) ..
                    " affixes=" .. table.concat(CM.affixIds or {}, ",") ..
                    " key=" .. tostring(CM.keystoneMapId) .. ":" ..
                        tostring(CM.keystoneLevel) ..
                    " completion=" .. tostring(CM.completionRevision) ..
                    " rating=" .. tostring(CM.completionInfo and
                        CM.completionInfo.ratingAfter or 0))
            end
        end
        CM.slashInstalled = true
    end

    if not CM.eventFrame then CM.eventFrame = CreateFrame("Frame") end
    CM.eventFrame:RegisterEvent("PLAYER_LOGIN")
    CM.eventFrame:RegisterEvent("PLAYER_ENTERING_WORLD")
    CM.eventFrame:RegisterEvent("ZONE_CHANGED_NEW_AREA")
    CM.eventFrame:SetScript("OnEvent", function()
        installSlashCommand()
        if CM.Request then CM.Request() end
    end)
    CM.eventFrame:SetScript("OnUpdate", function(_, elapsed)
        CM.updateElapsed = (CM.updateElapsed or 0) + elapsed
        if CM.updateElapsed >= 0.10 then
            CM.updateElapsed = 0
            WXL_NativeChallengePump()
            pull(false)
        end
    end)

    installSlashCommand()
    pull(true)
    pullKeystone(true)
    pullCompletion(true)
    pullHistory(true)
    pullLootSpecialization(true)
    if CM.Request then CM.Request() end
end
)lua";


}
