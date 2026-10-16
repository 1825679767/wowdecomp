// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#include "BlpCompat.hpp"
#include "NativeFeatures.hpp"
#include "engine/assets/shared/textures/blp/BlpTranscode.hpp"
namespace wxl::native {
constexpr uint32_t Limit=64u*1024*1024;
bool ConvertBlp(const void* raw,uint32_t size,std::vector<uint8_t>& output){
 if(!BlpEnabled() || !raw || size>Limit)return false;
 try{return wxl::modern::assets::textures::blp::TranscodeBlp({static_cast<const uint8_t*>(raw),size},output);}catch(...){return false;}
}
}
