file(GLOB PRIVATE_SOURCES CONFIGURE_DEPENDS "*.cpp")

add_executable(Wow WIN32 ${PRIVATE_SOURCES})

if (WOWDECOMP_ARCH_64)
    set(WOW_OUTPUT_NAME Wow64)
else ()
    set(WOW_OUTPUT_NAME Wow32)
endif ()

set_target_properties(Wow PROPERTIES
    OUTPUT_NAME ${WOW_OUTPUT_NAME}
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/Win
)

foreach (config ${CMAKE_CONFIGURATION_TYPES} ${CMAKE_BUILD_TYPE})
    string(TOUPPER ${config} config)
    set_target_properties(Wow PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${config} ${CMAKE_SOURCE_DIR}/bin/Win)
endforeach ()

target_include_directories(Wow
    PRIVATE
        ${CMAKE_SOURCE_DIR}
)

target_link_libraries(Wow
    PRIVATE
        BLPFile
        Base
        BlizzardCore
        ComSat
        ConsoleBase
        DayNight
        Event
        FFX
        Frame
        FrameScript
        FrameXML
        Gx
        Gxu
        Images
        Model2
        Os
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
        UIUtil
        Ui
        WorldClient
        WowServices
        WowSvcs
        Triage
)
