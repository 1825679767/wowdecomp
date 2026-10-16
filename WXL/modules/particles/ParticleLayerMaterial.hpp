// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#pragma once
#include "Engine/Source/Gx/Types.h"
#include <cstdint>
namespace wxl::native {
// Source particle numbers are not mesh M2BLEND or Gx numeric identities. These
// exact mappings come from native CM2Model::InitializeLoaded and M2Scene.cpp.
// Keep capture and draw admission coupled; never resolve unsupported numbers to
// alpha or substitute the donor experimental BlendAdd state for native Screen.
struct ParticleLayerMaterial {
 uint8_t sourceBlend;
 EGxBlend nativeBlend;
 bool blackFog;
};
inline bool ResolveParticleLayerMaterial(uint8_t source,ParticleLayerMaterial& output) noexcept {
 switch(source){
  case 2:output={source,GxBlend_Alpha,false};return true; // SRC_ALPHA, INV_SRC_ALPHA
  case 3:output={source,GxBlend_NoAlphaAdd,true};return true; // ONE, ONE
  case 4:output={source,GxBlend_Add,true};return true; // SRC_ALPHA, ONE
  case 7:output={source,GxBlend_Screen,true};return true; // INV_DST_COLOR, ONE
  default:return false;
 }
}
}
