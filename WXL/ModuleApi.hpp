// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
#include <cstdint>
namespace wxl::native {
// Borrowed immutable metadata; returned strings have process lifetime.
struct ModuleInfo { const char* id; uint32_t apiVersion; const char* dependencies; bool enabled; };
uint32_t ModuleCount() noexcept;
bool QueryModule(uint32_t index,ModuleInfo& output) noexcept;
void InitializeModuleLua();
}
