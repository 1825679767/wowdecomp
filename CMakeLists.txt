cmake_minimum_required(VERSION 3.10)
if(POLICY CMP0079)
    cmake_policy(SET CMP0079 NEW) # Source integration links native subdirectory targets.
endif()

if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR
        "In-source builds not allowed.
        Please make a new directory (called a build directory) and run CMake from there.
        You may need to remove CMakeCache.txt."
    )
endif ()

project(wowdecomp CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT CMAKE_SYSTEM_NAME MATCHES "Windows")
    message(FATAL_ERROR "wowdecomp targets Windows only.")
endif ()

# x64 is the target. Win32 stays buildable: it is the configuration whose struct layouts can be
# checked against Wow.exe 3.3.5a (12340).
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(WOWDECOMP_ARCH_64 1)
    add_compile_definitions(WOWDECOMP_ARCH_64=1)
elseif (CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(WOWDECOMP_ARCH_32 1)
    add_compile_definitions(WOWDECOMP_ARCH_32=1)
    # x87 floating point like the VS2005 original; keeps Win32 code comparable with the binary (re/tools/verify.py).
    if (MSVC)
        add_compile_options(/arch:IA32 /fp:fast)
    endif ()
    # 32-bit time_t like the original build: time/localtime/mktime resolve to _time32/_localtime32/_mktime32.
    add_compile_definitions(_USE_32BIT_TIME_T)
else ()
    message(FATAL_ERROR "Unsupported pointer size.")
endif ()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_compile_definitions(
    WIN32
    _WINDOWS
    NOMINMAX
    WIN32_LEAN_AND_MEAN
    _CRT_SECURE_NO_WARNINGS
)

# Off by default: the client checks MPQ file MD5s (signaturefile) and GlueXML/FrameXML/AddOn .toc.sig files like the
# original. ON accepts unsigned or modified game data and interface files.
option(WOWDECOMP_DISABLE_SIGNATURE_CHECKS "Skip MPQ file signature and .toc signature checks" OFF)
if (WOWDECOMP_DISABLE_SIGNATURE_CHECKS)
    add_compile_definitions(DISABLE_SIGNATURE_CHECKS=1)
endif ()

# Third-party code is linked, not decompiled, for now.
option(WXL_ENABLE_DX11 "Build the separate native x64 D3D11 backend" OFF)
set(WXL_DX11_WDK_ROOT "" CACHE PATH "Pinned WDK Include/version directory for the shader converter")
set(WXL_DX11_SIGNER_DLL "" CACHE FILEPATH "Pinned AMD64 Microsoft dxbcSigner.dll")
if(WXL_ENABLE_DX11)
    if(NOT WOWDECOMP_ARCH_64)
        message(FATAL_ERROR "The experimental native DX11 backend belongs to the separate x64 runtime family")
    endif()
    if(NOT EXISTS "${WXL_DX11_SIGNER_DLL}")
        message(FATAL_ERROR "Set WXL_DX11_SIGNER_DLL; see Tools/Dx11Research/DEPENDENCIES.md")
    endif()
    add_compile_definitions(WXL_ENABLE_DX11=1)
endif()
if (EXISTS ${CMAKE_SOURCE_DIR}/ThirdParty/CMakeLists.txt)
    add_subdirectory(ThirdParty)
endif ()

add_subdirectory(WXL)

add_subdirectory(Engine/Source/BLPFile)
add_subdirectory(Engine/Source/Base)
add_subdirectory(Engine/Source/BlizzardCore)
add_subdirectory(Engine/Source/ComSat)
add_subdirectory(Engine/Source/ConsoleBase)
add_subdirectory(Engine/Source/Event)
add_subdirectory(Engine/Source/FFX)
add_subdirectory(Engine/Source/Frame)
add_subdirectory(Engine/Source/FrameScript)
add_subdirectory(Engine/Source/FrameXML)
add_subdirectory(Engine/Source/Gx)
add_subdirectory(Engine/Source/Gxu)
add_subdirectory(Engine/Source/Images)
add_subdirectory(Engine/Source/jpeg)
add_subdirectory(Engine/Source/lua-5.1)
add_subdirectory(Engine/Source/Model2)
add_subdirectory(Engine/Source/Os)
add_subdirectory(Engine/Source/pcre)
add_subdirectory(Engine/Source/Scrn)
add_subdirectory(Engine/Source/Services)
add_subdirectory(Engine/Source/ShaderEffect)
add_subdirectory(Engine/Source/SoundEngine)
add_subdirectory(Engine/Source/SteelseriesMouse)
add_subdirectory(Engine/Source/Storm)
add_subdirectory(Engine/Source/Tempest)
add_subdirectory(WoW/Common)
add_subdirectory(WoW/Source/Battlenet)
add_subdirectory(WoW/Source/Console)
add_subdirectory(WoW/Source/ConsoleDevice)
add_subdirectory(WoW/Source/DB)
add_subdirectory(WoW/Source/Game)
add_subdirectory(WoW/Source/Glue)
add_subdirectory(WoW/Source/Magic)
add_subdirectory(WoW/Source/LiquidSystem)
add_subdirectory(WoW/Source/Minigame)
add_subdirectory(WoW/Source/Net)
add_subdirectory(WoW/Source/Object)
add_subdirectory(WoW/Source/ObjectAlloc)
add_subdirectory(WoW/Source/ObjectMgrClient)
add_subdirectory(WoW/Source/SoundInterface)
add_subdirectory(WoW/Source/UIUtil)
add_subdirectory(WoW/Source/Ui)
add_subdirectory(WoW/Source/WorldClient)
add_subdirectory(WoW/Source/WowServices)
add_subdirectory(WoW/Source/WowSvcs)
add_subdirectory(WoW/Source)
if (WIN32)
    add_subdirectory(Tools/WowClient)
endif ()
if (WOWDECOMP_ARCH_64)
    add_subdirectory(Tools/XvidDecoder)
endif ()

foreach (target Wow WowClient MapViewer M2Viewer GxTest MpqTest BlpTest DbcTest AssetCheck)
    if (TARGET ${target})
        target_sources(${target} PRIVATE ${CMAKE_SOURCE_DIR}/waystone.rc)
    endif ()
endforeach ()

# Native source modules share compiled types/lifetimes; no injected WXL runtime.
target_link_libraries(Model2 PUBLIC WxlNative)
target_link_libraries(BLPFile PUBLIC WxlNative)
target_link_libraries(WowClient PRIVATE WxlNative)
if(TARGET Wow)
 target_link_libraries(Wow PRIVATE WxlNative)
endif()
if(WXL_ENABLE_DX11)
    add_executable(NativeDx11BackendTests Tools/Dx11Research/NativeBackendTests.cpp)
    target_include_directories(NativeDx11BackendTests PRIVATE ${CMAKE_SOURCE_DIR})
    target_link_libraries(NativeDx11BackendTests PRIVATE Gx Tempest Storm Os Base BlizzardCore zlib)
    # Current Gx teardown owns WXL custom programs and refraction resources.
    # Link the real client module graph so tests exercise those owners too.
    get_target_property(_wxl_native_test_libraries WowClient LINK_LIBRARIES)
    target_link_libraries(NativeDx11BackendTests PRIVATE ${_wxl_native_test_libraries})
    get_target_property(_wxl_native_test_owners WowClient SOURCES)
    list(FILTER _wxl_native_test_owners EXCLUDE REGEX "WinMain[.]cpp$|crash_log[.]cpp$|[.]rc$")
    target_sources(NativeDx11BackendTests PRIVATE ${_wxl_native_test_owners})
    add_dependencies(NativeDx11BackendTests XvidDecoder)
    target_link_options(NativeDx11BackendTests PRIVATE "/MAP:${CMAKE_BINARY_DIR}/NativeDx11BackendTests.map")
    add_custom_command(TARGET NativeDx11BackendTests POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${WXL_DX11_SIGNER_DLL}" "$<TARGET_FILE_DIR:NativeDx11BackendTests>/dxbcSigner.dll")
    add_custom_command(TARGET NativeDx11BackendTests POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:XvidDecoder>" "$<TARGET_FILE_DIR:NativeDx11BackendTests>/XvidDecoder.dll")
endif()
