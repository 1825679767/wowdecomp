// Copyright (C)2026 WarcraftXL. GPL-3.0-or-later.
#include "NativeStorage.hpp"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
namespace wxl::native {
constexpr uint32_t Limit=64u*1024*1024;
bool ReadAsset(const char* path,std::vector<uint8_t>& output){
 if(!path || !*path)return false;void* data=nullptr;uint32_t size=0;
 if(!SFile::LoadFile(path,&data,&size,0,nullptr) || !data)return false;
 bool ok=false;try{if(size && size<=Limit){std::vector<uint8_t> next(static_cast<const uint8_t*>(data),static_cast<const uint8_t*>(data)+size);output.swap(next);ok=true;}}catch(...){}
 SFile::Unload(data);return ok;
}
}
