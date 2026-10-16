// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
#include <cstdint>
class CCharacterComponent;
class CSimpleModel;
class CM2Scene;
class C3Vector;
namespace wxl::native::creation_beams {
void Register();
bool Enabled() noexcept;
// Only the authored Priest channel phase may request this presentation.
// Spell/visual/kit/chain IDs and textures come from mounted native DB rows.
bool StartMindFlay(CCharacterComponent&,uint32_t phaseDurationMs) noexcept;
void Stop() noexcept;
void ReleaseScene(CM2Scene*) noexcept;
// Called with the matching Glue model's camera/viewport still installed.
void Render(CSimpleModel*,const C3Vector& cameraPosition) noexcept;
}
