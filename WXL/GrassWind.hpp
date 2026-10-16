// Source-native donor grass sway/repulsion. GPL-3.0-or-later.
#pragma once
#include <cstdint>
class CGxShader;class CGxDevice;
namespace wxl::native::grass_wind {void Register();bool Enabled() noexcept;bool CreateVertex(CGxDevice&,CGxShader&) noexcept;void Forget(CGxShader&) noexcept;void Upload(CGxShader&) noexcept;}
namespace wxl::native::grass_wind {
// Exact donor bytecode for the native detail-doodad identity; remains CPU-owned.
const void* VertexCode(CGxShader&,uint32_t& length) noexcept;
}
