// Source-native donor grass sway/repulsion. GPL-3.0-or-later.
#pragma once
class CGxShader;class CGxDevice;
namespace wxl::native::grass_wind {void Register();bool Enabled() noexcept;bool CreateVertex(CGxDevice&,CGxShader&) noexcept;void Forget(CGxShader&) noexcept;void Upload(CGxShader&) noexcept;}
