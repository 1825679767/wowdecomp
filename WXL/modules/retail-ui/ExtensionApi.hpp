// Native source adapter; no plugin loading, x86 addresses or offset-based calls.
#pragma once
#include "Engine/Source/FrameScript/FrameScript.h"
#include "common/Log.hpp"
#include <cstdint>
namespace wxl_retail_ui {
using PacketCallback=void(*)(const uint8_t*,uint32_t,void*);
struct NativeFrameScript {int(*RegisterFunction)(const char*,lua_CFunction);int(*RegisterScript)(const char*,const char*);int(*Execute)(const char*,const char*);};
struct NativeNetwork {int(*RegisterClientOpcode)(uint16_t,const char*);int(*RegisterServerOpcode)(uint16_t,const char*,PacketCallback,void*);int(*Send)(uint16_t,const void*,uint32_t);};
struct NativeFdid {const char*(*ResolveTexture)(uint32_t);};
struct NativeInterfaces {const void*(*GetInterface)(const char*,uint32_t);};
const NativeFrameScript* FrameScript();const NativeNetwork* Network();const NativeFdid* Fdid();
extern const NativeInterfaces* g_api;
bool InstallItemVariants();void ResetItemVariants();bool InstallRetailTransmogPreview();bool InstallRetailUiCompatibility();bool InstallWeeklyRewards();bool InstallRetailTransmog();bool InstallRetailTransmogEquipment();bool InstallRetailTransmogApply();bool InstallRetailTransmogOutfits();bool InstallRetailTransmogCustomSets();bool InstallRetailTransmogSituations();
void ResetWeeklyRewards();void ResetCollections();void ResetEquipment();void ResetApply();void ResetOutfits();void ResetCustomSets();void ResetSituations();
}
using wxl_retail_ui::NativeFrameScript;using wxl_retail_ui::NativeNetwork;using wxl_retail_ui::NativeFdid;
