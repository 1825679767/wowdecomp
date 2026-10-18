// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
#include <cstdint>
#include <vector>
struct CharacterPreferences;
class CM2Model;
class CCharacterComponent;
namespace wxl::native {
struct RetailChoiceState;
struct RetailChoicePair {uint32_t option,choice;};
uint32_t RetailRecipeRace(const CCharacterComponent&) noexcept;
// Native equipment suffix authority for the actual model, not the wire race.
uint32_t RetailEquipmentRace(const CCharacterComponent&) noexcept;
bool IsRetailExtraOption(const CCharacterComponent&,uint32_t option) noexcept;
bool RetailExtrasMatch(const CCharacterComponent&,const RetailChoicePair*,uint32_t count) noexcept;
bool RestoreRetailExtras(CCharacterComponent&,const RetailChoicePair*,uint32_t count) noexcept;
bool CaptureRetailChoicePairs(const CCharacterComponent&,std::vector<RetailChoicePair>&) noexcept;
bool ApplyRetailChoicePair(CCharacterComponent&,const RetailChoicePair*,uint32_t,CCharacterComponent&,const RetailChoicePair*,uint32_t) noexcept;
bool ApplyRetailChoices(CCharacterComponent&,const RetailChoicePair*,uint32_t count) noexcept;
enum class RetailAxis : uint32_t { Skin,Face,HairStyle,HairColor,Facial };
uint32_t RetailRaceId(uint32_t) noexcept;
// Owned recipe path, stable for the lifetime of the immutable authority.
// Private/classic actors retain the caller's mounted DBC path.
const char* RetailActorModelPath(uint32_t race,uint32_t sex,const char* nativePath) noexcept;
const char* RetailNpcActorModelPath(uint32_t race,uint32_t sex,const char* nativePath) noexcept;
uint32_t RetailChoiceCount(uint32_t,uint32_t,RetailAxis) noexcept;
bool RetailAxisChoicesReady(const CharacterPreferences&,const CM2Model* = nullptr) noexcept;
void SyncPrivateHairColor(CCharacterComponent&,uint32_t color) noexcept;
bool RetailChoicesReady(const CharacterPreferences&,const CM2Model* = nullptr) noexcept;
bool NormalizeRetailPreferences(CharacterPreferences&,const CM2Model* = nullptr) noexcept;
bool SetRetailAxis(CCharacterComponent&,RetailAxis,int32_t) noexcept;
bool CycleRetailAxis(CCharacterComponent&,RetailAxis,int32_t) noexcept;
bool CollectRetailChoices(const CCharacterComponent&,std::vector<uint32_t>&) noexcept;
bool RetailSelectionNeedsProtocol(const CCharacterComponent&) noexcept;
uint32_t RetailChoiceRevision(const CCharacterComponent&) noexcept;
void TransferRetailChoices(const CCharacterComponent&,CCharacterComponent&) noexcept;
void ReleaseRetailChoices(CCharacterComponent&) noexcept;
bool RandomizeRetailExtras(CCharacterComponent&) noexcept;
void InitializeRetailChoiceLua();
void UpdateRetailChoiceUi();
void HideRetailChoiceUi();
}
