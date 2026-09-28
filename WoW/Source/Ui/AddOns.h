#ifndef WOW_SOURCE_UI_ADDONS_H
#define WOW_SOURCE_UI_ADDONS_H

#include <cstdint>

void ClearAddOnEnableState(const char* character);
const char* GetAddOnByIndex(uint32_t index);
uint32_t GetNumAddOns();
void InitializeAddOns();
void InitializeAddOns(const char* accountName);
bool IsAddOnLoading(const char* name);
void LoadAddOnEnableState(const char* character);
void ShutdownAddOns();

#endif
