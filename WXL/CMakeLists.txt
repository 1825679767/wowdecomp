option(WXL_ENABLE_NATIVE "Compile WXL integration into the native executable" ON)
option(WXL_ENABLE_ASSETS "Enable modern M2 and FDID data" ON)
option(WXL_ENABLE_BLP "Enable modern BLP transcode" ON)
option(WXL_ENABLE_MATERIALS "Enable custom mesh BlendAdd materials" ON)
option(WXL_ENABLE_PARTICLES "Enable native Screen particle rendering" ON)
option(WXL_ENABLE_APPEARANCES "Enable retail character appearance recipes" ON)
option(WXL_ENABLE_PROTOCOL "Enable native custom opcodes" ON)
foreach(feature NATIVE ASSETS BLP MATERIALS PARTICLES APPEARANCES PROTOCOL)
 if(WXL_ENABLE_${feature})
  add_compile_definitions(WXL_ENABLE_${feature}=1)
 else()
  add_compile_definitions(WXL_ENABLE_${feature}=0)
 endif()
endforeach()
set(V "${CMAKE_CURRENT_SOURCE_DIR}/vendor")
add_library(WxlNative STATIC NativeFeatures.cpp AssetCompat.cpp FdidCompat.cpp NativeProtocol.cpp AppearanceCompat.cpp
 "${V}/src/platform/waystone/CharacterAtlas.cpp"
 "${V}/extensions/wxl-db2/src/decode/Wdc5.cpp"
 "${V}/extensions/wxl-db2/src/decode/Wdc5Table.cpp"
 "${V}/extensions/wxl-db2/src/api/Db2.cpp"
 "${V}/extensions/wxl-db2/src/schemas/AppearanceStore.cpp"
 "${V}/src/platform/waystone/M2Bytes.cpp"
 "${V}/src/platform/waystone/SkeletonIndex.cpp" "${V}/src/platform/waystone/SkeletonEmbed.cpp"
 "${V}/src/platform/waystone/SkinPartition.cpp" "${V}/src/platform/waystone/SkinRebuild.cpp"
 "${V}/extensions/wxl-modern-m2/src/load/M2Normalize.cpp"
 "${V}/extensions/wxl-modern-m2/src/load/M2NormalizeParticles.cpp"
 "${V}/extensions/wxl-modern-m2/src/load/M2NormalizeCameras.cpp"
 "${V}/src/engine/assets/shared/textures/blp/BlpTranscode.cpp"
 "${V}/extensions/wxl-db2/src/decode/DB2File.cpp" "${V}/extensions/wxl-db2/src/decode/Db2Decode.cpp"
 "${V}/src/common/Log.cpp" "${V}/src/common/Config.cpp")
target_compile_features(WxlNative PRIVATE cxx_std_20)
target_include_directories(WxlNative PUBLIC "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" PRIVATE "${V}/src" "${V}/include")
target_compile_definitions(WxlNative PRIVATE WXL_WAYSTONE=1)
target_link_libraries(WxlNative PUBLIC bcrypt)
