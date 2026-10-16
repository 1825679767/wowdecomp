// Donor quest/corpse world quads on typed native world matrices. GPL-3.0-or-later.
#pragma once
#include "Engine/Source/Tempest/C3Vector.h"
#include <span>
namespace wxl::native::quest_marker {
struct WorldPoint {C3Vector position;bool corpse=false;};
// Bit0: at least one quest draw, bit1: corpse draw. Unsupported APIs keep UI fallback.
unsigned DrawWorldNavigation(std::span<const WorldPoint>,float scale,unsigned alpha);
void DestroyWorldNavigation() noexcept;
}
