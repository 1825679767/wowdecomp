// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
#include <cstdint>
#include <vector>
struct CharacterPreferences;
class CM2Model;
class CCharacterComponent;
namespace wxl::native {
struct RetailChoiceState;
enum class RetailAxis : uint32_t { Skin,Face,HairStyle,HairColor,Facial };
uint32_t RetailRaceId(uint32_t) noexcept;
uint32_t RetailChoiceCount(uint32_t,uint32_t,RetailAxis) noexcept;
bool RetailChoicesReady(const CharacterPreferences&,const CM2Model* = nullptr) noexcept;
bool NormalizeRetailPreferences(CharacterPreferences&,const CM2Model* = nullptr) noexcept;
bool SetRetailAxis(CCharacterComponent&,RetailAxis,int32_t) noexcept;
bool CycleRetailAxis(CCharacterComponent&,RetailAxis,int32_t) noexcept;
bool CollectRetailChoices(const CCharacterComponent&,std::vector<uint32_t>&) noexcept;
bool RetailSelectionNeedsProtocol(const CCharacterComponent&) noexcept;
uint32_t RetailChoiceRevision(const CCharacterComponent&) noexcept;
void TransferRetailChoices(const CCharacterComponent&,CCharacterComponent&) noexcept;
void ReleaseRetailChoices(CCharacterComponent&) noexcept;
void InitializeRetailChoiceLua();
}
