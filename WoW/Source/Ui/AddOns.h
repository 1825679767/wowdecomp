#ifndef WOW_SOURCE_UI_ADDONS_H
#define WOW_SOURCE_UI_ADDONS_H

#include <cstdint>

const char* GetAddOnByIndex(uint32_t index);
uint32_t GetNumAddOns();
bool IsAddOnLoading(const char* name);

#endif
