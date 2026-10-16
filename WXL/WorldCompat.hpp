// Copyright (C) 2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
class CMapArea;
class CMapLowDetail;
#include <cstdint>
class CMapObj;
class CMapObjGroup;
namespace wxl::native::world {
enum class SplitLoad { Classic, Loaded, Rejected };
SplitLoad LoadSplitArea(CMapArea&, const char* nativeFilename) noexcept;
bool PrepareWmo(CMapObj&) noexcept;
bool PrepareWdl(CMapLowDetail&,uint32_t& size) noexcept;
enum class GroupLoad { Classic, Loaded, Rejected };
GroupLoad LoadPreparedWmoGroup(CMapObj&, unsigned index, CMapObjGroup&) noexcept;
void ReleaseWmo(CMapObj&) noexcept;
bool IsPreparedWmo(const CMapObj*) noexcept;
bool ValidWorldRay(const float* start,const float* end) noexcept;
}
