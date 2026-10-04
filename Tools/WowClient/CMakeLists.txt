file(GLOB CLIENT_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/WoW/Source/*.cpp")

if (WOWDECOMP_ARCH_64)
    file(GLOB CLIENT_PLATFORM_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/WoW/Source/Win64/*.cpp")
    set(WOWCLIENT_OUTPUT_NAME WowClient64)
else ()
    file(GLOB CLIENT_PLATFORM_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/WoW/Source/Win32/*.cpp")
    set(WOWCLIENT_OUTPUT_NAME WowClient32)
endif ()

set(WOWCLIENT_OUTPUT_OVERRIDE "" CACHE STRING "WowClient executable name instead of WowClient64/WowClient32 (re/tools/build sets Waystone / WaystoneDev)")
if (WOWCLIENT_OUTPUT_OVERRIDE)
    set(WOWCLIENT_OUTPUT_NAME ${WOWCLIENT_OUTPUT_OVERRIDE})
endif ()

set(STANDINS_GENERATED "${CMAKE_CURRENT_BINARY_DIR}/standins_generated.cpp")
if (NOT EXISTS "${STANDINS_GENERATED}")
    file(WRITE "${STANDINS_GENERATED}" "extern \"C\" const char* const g_standinNames[] = { \"\" };\nextern \"C\" const int g_standinCount = 0;\n")
endif ()

add_executable(WowClient WIN32 ${CLIENT_SOURCES} ${CLIENT_PLATFORM_SOURCES} standin_runtime.cpp "${STANDINS_GENERATED}")

set_target_properties(WowClient PROPERTIES
    OUTPUT_NAME ${WOWCLIENT_OUTPUT_NAME}
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/Win
)

foreach (config ${CMAKE_CONFIGURATION_TYPES} ${CMAKE_BUILD_TYPE})
    string(TOUPPER ${config} config)
    set_target_properties(WowClient PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${config} ${CMAKE_SOURCE_DIR}/bin/Win)
endforeach ()

target_include_directories(WowClient
    PRIVATE
        ${CMAKE_SOURCE_DIR}
)

target_link_options(WowClient PRIVATE "/INCREMENTAL:NO" "/ENTRY:start")

set(WOWCLIENT_STANDIN_LINKER "" CACHE FILEPATH "Link launcher that turns unresolved symbols into stand-ins (re/tools/standin_link.py); Ninja and Makefile generators only")
if (WOWCLIENT_STANDIN_LINKER)
    find_package(Python3 COMPONENTS Interpreter REQUIRED)
    if (WOWDECOMP_ARCH_32)
        set(WOWCLIENT_STANDIN_ARCH win32)
    else ()
        set(WOWCLIENT_STANDIN_ARCH x64)
    endif ()
    set_property(TARGET WowClient PROPERTY RULE_LAUNCH_LINK "\"${Python3_EXECUTABLE}\" \"${WOWCLIENT_STANDIN_LINKER}\" \"${STANDINS_GENERATED}\" ${WOWCLIENT_STANDIN_ARCH} --")
endif ()

option(WOWCLIENT_STANDIN_CONTINUE "Stand-ins log their first hit and return 0 instead of exiting the client" OFF)
if (WOWCLIENT_STANDIN_CONTINUE)
    set_source_files_properties(standin_runtime.cpp PROPERTIES COMPILE_DEFINITIONS WOWCLIENT_STANDIN_CONTINUE_DEFAULT)
endif ()

target_link_libraries(WowClient
    PRIVATE
        BLPFile
        Base
        BlizzardCore
        ComSat
        ConsoleBase
        Event
        FFX
        Frame
        FrameScript
        lua
        FrameXML
        Gx
        Gxu
        Images
        Model2
        Os
        Scrn
        Services
        ShaderEffect
        Tempest
        SoundEngine
        SteelseriesMouse
        Storm
        WoWCommon
        Battlenet
        Console
        ConsoleDevice
        DB
        Game
        Glue
        Magic
        Minigame
        Net
        Object
        ObjectAlloc
        ObjectMgrClient
        SoundInterface
        UIUtil
        Ui
        WorldClient
        LiquidSystem
        WowServices
        WowSvcs
        dbghelp
)
