// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
namespace wxl::native::creation_equipment {
void InitializeLua();void UpdateUi();void Reset() noexcept;
// Called only by native creation Dress(); false leaves its normal path intact.
bool DressHidden() noexcept;
}
