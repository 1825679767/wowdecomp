#ifndef WOW_SOURCE_UI_ADDONS_H
#define WOW_SOURCE_UI_ADDONS_H

#include <cstdint>
#include "WoW/Source/UIUtil/Types.h"

void ClearAddOnEnableState(const char* character);
const char* GetAddOnByIndex(uint32_t index);
uint32_t GetNumAddOns();
void InitializeAddOns();
void InitializeAddOns(const char* accountName);
bool IsAddOnLoading(const char* name);
void LoadAddOnEnableState(const char* character);
void ShutdownAddOns();

uint32_t GetCodeInterfaceVersion();
uint32_t GetDataInterfaceVersion();
int32_t GetAddOnEnableState(const char* name, const char* character, bool compute);
bool IsAddOnLoadable(const char* name, bool demandLoad, UIADDON_LOADABLE_REASON& reason, UIADDON_LOADABLE_REASON& dependencyReason, const char* character);
bool IsAddOnLoaded(const char* name);
char* AddOnUnloadableToken(UIADDON_LOADABLE_REASON reason, UIADDON_LOADABLE_REASON dependencyReason, char* buffer, uint32_t size);
bool LoadAddOn(const char* name, bool demandLoad, CStatus* status);
uint32_t LoadAddOnFileCount();
void LoadAddOns(CStatus* status);
void UnloadAddOns();
void SaveAddOnEnableState();

#endif
