# Native WXL modules are object libraries, collected once into a static facade.
# This retains independent build targets without cross-archive linker cycles.
option(WXL_ENABLE_NATIVE "Enable native WXL features" ON)
option(WXL_ENABLE_ASSETS "Enable modern M2 assets" ON)
option(WXL_ENABLE_DB2 "Enable retail DB2/FDID data" ON)
option(WXL_ENABLE_BLP "Enable modern BLP decoding" ON)
option(WXL_ENABLE_MATERIALS "Enable custom mesh materials" ON)
option(WXL_ENABLE_PARTICLES "Enable modern particle compatibility" ON)
option(WXL_ENABLE_APPEARANCES "Enable retail appearance recipes" ON)
option(WXL_ENABLE_PROTOCOL "Enable native custom opcodes" ON)
option(WXL_ENABLE_APPEARANCE_SAVE "Enable negotiated retail appearance saving" ON)
foreach(feature NATIVE ASSETS DB2 BLP MATERIALS PARTICLES APPEARANCES PROTOCOL APPEARANCE_SAVE)
 if(WXL_ENABLE_${feature})
  add_compile_definitions(WXL_ENABLE_${feature}=1)
 else()
  add_compile_definitions(WXL_ENABLE_${feature}=0)
 endif()
endforeach()
set(V "${CMAKE_CURRENT_SOURCE_DIR}/vendor")
function(wxl_module target alias)
 add_library(${target} OBJECT ${ARGN})
 add_library(WXL::${alias} ALIAS ${target})
 target_compile_features(${target} PRIVATE cxx_std_20)
 target_include_directories(${target} PRIVATE "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" "${V}/src" "${V}/include")
 target_compile_definitions(${target} PRIVATE WXL_WAYSTONE=1)
endfunction()
wxl_module(WxlRuntime Runtime modules/runtime/NativeFeatures.cpp modules/runtime/ModuleApi.cpp
 "${V}/src/common/Log.cpp" "${V}/src/common/Config.cpp")
wxl_module(WxlStorage Storage modules/storage/NativeStorage.cpp)
wxl_module(WxlDB2 DB2 modules/db2/FdidCompat.cpp
 "${V}/extensions/wxl-db2/src/decode/DB2File.cpp" "${V}/extensions/wxl-db2/src/decode/Db2Decode.cpp"
 "${V}/extensions/wxl-db2/src/decode/Wdc5.cpp" "${V}/extensions/wxl-db2/src/decode/Wdc5Table.cpp"
 "${V}/extensions/wxl-db2/src/api/Db2.cpp" "${V}/extensions/wxl-db2/src/schemas/AppearanceStore.cpp")
wxl_module(WxlBlp BLP modules/blp/BlpCompat.cpp "${V}/src/engine/assets/shared/textures/blp/BlpTranscode.cpp")
wxl_module(WxlMaterials Materials modules/materials/MaterialCompat.cpp)
wxl_module(WxlParticles Particles modules/particles/ParticleCompat.cpp
 "${V}/extensions/wxl-modern-m2/src/load/M2NormalizeParticles.cpp")
wxl_module(WxlM2 M2 modules/m2/AssetCompat.cpp
 "${V}/src/platform/waystone/M2Bytes.cpp" "${V}/src/platform/waystone/SkeletonIndex.cpp"
 "${V}/src/platform/waystone/SkeletonEmbed.cpp" "${V}/src/platform/waystone/SkinPartition.cpp"
 "${V}/src/platform/waystone/SkinRebuild.cpp" "${V}/extensions/wxl-modern-m2/src/load/M2Normalize.cpp"
 "${V}/extensions/wxl-modern-m2/src/load/M2NormalizeCameras.cpp")
wxl_module(WxlAppearances Appearances modules/appearances/AppearanceCompat.cpp modules/appearances/RetailChoices.cpp
 "${V}/src/platform/waystone/CharacterAtlas.cpp")
wxl_module(WxlProtocol Protocol modules/protocol/NativeProtocol.cpp modules/protocol/ProtocolConsumers.cpp)
wxl_module(WxlAppearanceSession AppearanceSession modules/appearance-session/AppearanceSession.cpp)
add_dependencies(WxlAppearanceSession WxlAppearances WxlProtocol)
add_dependencies(WxlDB2 WxlRuntime WxlStorage)
add_dependencies(WxlBlp WxlRuntime)
add_dependencies(WxlMaterials WxlRuntime)
add_dependencies(WxlParticles WxlRuntime)
add_dependencies(WxlM2 WxlStorage WxlDB2 WxlBlp WxlMaterials WxlParticles)
add_dependencies(WxlAppearances WxlM2 WxlDB2 WxlBlp)
add_dependencies(WxlProtocol WxlRuntime)
add_library(WxlNative STATIC
 $<TARGET_OBJECTS:WxlRuntime> $<TARGET_OBJECTS:WxlStorage> $<TARGET_OBJECTS:WxlDB2>
 $<TARGET_OBJECTS:WxlBlp> $<TARGET_OBJECTS:WxlMaterials> $<TARGET_OBJECTS:WxlParticles>
 $<TARGET_OBJECTS:WxlM2> $<TARGET_OBJECTS:WxlAppearances> $<TARGET_OBJECTS:WxlProtocol> $<TARGET_OBJECTS:WxlAppearanceSession>)
add_library(WXL::Native ALIAS WxlNative)
target_include_directories(WxlNative PUBLIC "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(WxlNative PUBLIC bcrypt)
