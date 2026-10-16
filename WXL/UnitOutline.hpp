// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
#include <cstdint>
class CGxDevice;class CM2Model;struct IDirect3DDevice9;
namespace wxl::native::unit_outline {
void Register();bool Enabled() noexcept;
void BeginWorld() noexcept;void EndWorld() noexcept;
void Release(CGxDevice&) noexcept;
// Backend-neutral selection policy for the currently scoped M2 batch.
bool CurrentTarget(CGxDevice&,float* rgba,bool& depthTest) noexcept;
class ModelScope {CM2Model* previous;public:explicit ModelScope(CM2Model*) noexcept;~ModelScope();ModelScope(const ModelScope&)=delete;};
void Stamp(IDirect3DDevice9*,uint32_t primitive,int32_t base,uint32_t min,uint32_t vertices,uint32_t start,uint32_t count) noexcept;
}
