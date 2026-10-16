# Native WXL modules are object libraries, collected once into a static facade.
# This retains independent build targets without cross-archive linker cycles.
option(WXL_ENABLE_NATIVE "Enable native WXL features" ON)
option(WXL_ENABLE_ASSETS "Enable modern M2 assets" ON)
option(WXL_ENABLE_DB2 "Enable retail DB2/FDID data" ON)
option(WXL_ENABLE_BLP "Enable modern BLP decoding" ON)
option(WXL_ENABLE_MATERIALS "Enable custom mesh materials" ON)
option(WXL_ENABLE_BLOOD_REFRACTION "Enable exact-source donor Blood Boil refraction compatibility" ON)
option(WXL_ENABLE_DND_MATERIALS "Enable exact-source donor DnD material compatibility" ON)
option(WXL_ENABLE_PARTICLES "Enable modern particle compatibility" ON)
option(WXL_ENABLE_APPEARANCES "Enable retail appearance recipes" ON)
option(WXL_ENABLE_PROTOCOL "Enable native custom opcodes" ON)
option(WXL_ENABLE_APPEARANCE_SAVE "Enable negotiated retail appearance saving" ON)
option(WXL_ENABLE_WMO "Enable modern WMO byte providers" ON)
option(WXL_ENABLE_SPLIT_ADT "Enable split ADT byte providers" ON)
option(WXL_ENABLE_CLIENT_EXTENSIONS "Enable WXL addon helper and packet APIs" ON)
option(WXL_ENABLE_CHALLENGE_MODE "Enable native Mythic+ challenge API" ON)
option(WXL_ENABLE_MYTHIC_DIFFICULTY "Enable native third dungeon difficulty and portal models" ON)
option(WXL_ENABLE_QUEST_MARKER "Enable native quest marker and objective highlights" ON)
option(WXL_ENABLE_RADIAL_PING "Enable native radial pings" ON)
option(WXL_ENABLE_CREATION_EFFECTS "Enable native character creation choreography" ON)
option(WXL_ENABLE_SKYRIDING "Enable native server-authorized Skyriding" ON)
option(WXL_ENABLE_BARBER_PREVIEW "Enable native barber retail-detail previews" ON)
option(WXL_ENABLE_PRIVATE_APPEARANCES "Enable authored private creature appearance profiles" ON)
foreach(feature BARBER_PREVIEW SKYRIDING CREATION_EFFECTS NATIVE ASSETS DB2 BLP MATERIALS PARTICLES APPEARANCES PROTOCOL APPEARANCE_SAVE WMO SPLIT_ADT CLIENT_EXTENSIONS RADIAL_PING QUEST_MARKER CHALLENGE_MODE MYTHIC_DIFFICULTY)
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
wxl_module(WxlMaterials Materials modules/materials/MaterialCompat.cpp modules/materials/MeshMaterials.cpp modules/materials/DndMaterials.cpp)
target_compile_definitions(WxlMaterials PRIVATE WXL_ENABLE_DND_MATERIALS=$<BOOL:${WXL_ENABLE_DND_MATERIALS}>)
wxl_module(WxlParticles Particles modules/particles/ParticleCompat.cpp modules/particles/ParticleCutoff.cpp modules/particles/ParticleLayerCapture.cpp modules/particles/ParticleLayers.cpp modules/particles/ParticleRefraction.cpp
 "${V}/extensions/wxl-modern-m2/src/load/M2NormalizeParticles.cpp")
wxl_module(WxlM2 M2 modules/m2/AssetCompat.cpp
 "${V}/src/platform/waystone/M2Bytes.cpp" "${V}/src/platform/waystone/SkeletonIndex.cpp"
 "${V}/src/platform/waystone/SkeletonEmbed.cpp" "${V}/src/platform/waystone/SkinPartition.cpp"
 "${V}/src/platform/waystone/SkinRebuild.cpp" "${V}/extensions/wxl-modern-m2/src/load/M2Normalize.cpp"
 "${V}/extensions/wxl-modern-m2/src/load/M2NormalizeCameras.cpp")
wxl_module(WxlAppearances Appearances modules/private-appearances/PrivateAppearances.cpp modules/barber-preview/BarberAppearance.cpp modules/appearances/AppearanceCompat.cpp modules/appearances/RetailChoices.cpp modules/appearances/AlternateAppearances.cpp
 "${V}/src/platform/waystone/CharacterAtlas.cpp")
target_compile_definitions(WxlAppearances PRIVATE WXL_ENABLE_PRIVATE_APPEARANCES=$<BOOL:${WXL_ENABLE_PRIVATE_APPEARANCES}>)
wxl_module(WxlProtocol Protocol modules/protocol/NativeProtocol.cpp modules/protocol/ProtocolConsumers.cpp)
wxl_module(WxlAppearanceSession AppearanceSession modules/appearance-session/AppearanceSession.cpp)
wxl_module(WxlWorld World modules/world/WmoNative.cpp modules/world/SplitAdtNative.cpp
 "${V}/src/platform/waystone/WmoAssetFamily.cpp" "${V}/src/platform/waystone/WmoRootBytes.cpp"
 "${V}/src/platform/waystone/WmoMaterialPlan.cpp" "${V}/src/platform/waystone/WmoChunkIndex.cpp")
wxl_module(WxlClientExtensions ClientExtensions modules/client-extensions/ClientHelpers.cpp
 modules/client-extensions/LegacyPackets.cpp modules/client-extensions/ParagonCompat.cpp)
wxl_module(WxlChallengeMode ChallengeMode modules/challenge-mode/ChallengeMode.cpp modules/challenge-mode/MythicDifficulty.cpp)
add_dependencies(WxlChallengeMode WxlRuntime WxlProtocol)
wxl_module(WxlQuestMarker QuestMarker modules/quest-marker/QuestMarker.cpp)
add_dependencies(WxlQuestMarker WxlRuntime WxlProtocol)
wxl_module(WxlCreationEffects CreationEffects modules/creation/CreationEffects.cpp)
add_dependencies(WxlCreationEffects WxlRuntime WxlAppearances WxlStorage)
wxl_module(WxlSkyriding Skyriding modules/skyriding/Skyriding.cpp)
add_dependencies(WxlSkyriding WxlRuntime WxlProtocol WxlM2)
wxl_module(WxlRadialPing RadialPing modules/radial-ping/RadialPing.cpp)
add_dependencies(WxlRadialPing WxlRuntime WxlProtocol)
add_dependencies(WxlClientExtensions WxlRuntime WxlProtocol WxlRadialPing WxlQuestMarker WxlChallengeMode)
add_dependencies(WxlWorld WxlStorage WxlDB2 WxlRuntime)
add_dependencies(WxlAppearanceSession WxlAppearances WxlProtocol)
add_dependencies(WxlDB2 WxlRuntime WxlStorage)
add_dependencies(WxlBlp WxlRuntime)
add_dependencies(WxlMaterials WxlRuntime)
target_compile_definitions(WxlParticles PRIVATE WXL_ENABLE_BLOOD_REFRACTION=$<BOOL:${WXL_ENABLE_BLOOD_REFRACTION}>)
add_dependencies(WxlParticles WxlRuntime)
add_dependencies(WxlM2 WxlStorage WxlDB2 WxlBlp WxlMaterials WxlParticles)
add_dependencies(WxlAppearances WxlM2 WxlDB2 WxlBlp)
add_dependencies(WxlProtocol WxlRuntime)
add_library(WxlNative STATIC
 $<TARGET_OBJECTS:WxlRuntime> $<TARGET_OBJECTS:WxlStorage> $<TARGET_OBJECTS:WxlDB2>
 $<TARGET_OBJECTS:WxlBlp> $<TARGET_OBJECTS:WxlMaterials> $<TARGET_OBJECTS:WxlParticles>
 $<TARGET_OBJECTS:WxlSkyriding> $<TARGET_OBJECTS:WxlM2> $<TARGET_OBJECTS:WxlAppearances> $<TARGET_OBJECTS:WxlProtocol> $<TARGET_OBJECTS:WxlAppearanceSession> $<TARGET_OBJECTS:WxlWorld> $<TARGET_OBJECTS:WxlClientExtensions> $<TARGET_OBJECTS:WxlRadialPing> $<TARGET_OBJECTS:WxlQuestMarker> $<TARGET_OBJECTS:WxlCreationEffects> $<TARGET_OBJECTS:WxlChallengeMode>)
add_library(WXL::Native ALIAS WxlNative)
target_include_directories(WxlNative PUBLIC "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(WxlNative PUBLIC bcrypt shell32)
