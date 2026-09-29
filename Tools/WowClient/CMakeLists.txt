file(GLOB CLIENT_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/WoW/Source/*.cpp")

if (WOWDECOMP_ARCH_64)
    file(GLOB CLIENT_PLATFORM_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/WoW/Source/Win64/*.cpp")
    set(WOWCLIENT_OUTPUT_NAME WowClient64)
else ()
    file(GLOB CLIENT_PLATFORM_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/WoW/Source/Win32/*.cpp")
    set(WOWCLIENT_OUTPUT_NAME WowClient32)
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
        Triage
        dbghelp
)
