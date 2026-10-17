// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
#include <cstdint>
#include <string>
#include <vector>
class CCharacterComponent;
namespace wxl::appearance {struct Selection;}
namespace wxl::native {
struct PrivateAppearanceState;
namespace private_appearances {
struct Option {uint32_t id,count,defaultChoice;const char* name;};
struct Description {uint32_t id=0;std::string path;std::vector<Option> options;};
void Register();
bool Enabled() noexcept;
const char* ActorPath(uint32_t race,uint32_t sex,const char* nativePath) noexcept;
// Only explicitly ported native private profiles have an identity.
uint32_t Identity(uint32_t race,uint32_t sex) noexcept;
bool Describe(uint32_t race,uint32_t sex,const char* mountedNativePath,Description&) noexcept;
bool Validate(const wxl::appearance::Selection&) noexcept;
const char* OptionName(uint32_t option) noexcept;
const char* ChoiceName(uint32_t race,uint32_t option,uint32_t choice) noexcept;
uint32_t ChoiceSwatch(uint32_t race,uint32_t option,uint32_t choice) noexcept;
// Native render owner thread, after the classic compositor/geoset pass.
bool Apply(CCharacterComponent&) noexcept;
bool Ready(const CCharacterComponent&) noexcept;
void Release(CCharacterComponent&) noexcept;
}
}
