file(GLOB PRIVATE_SOURCES CONFIGURE_DEPENDS "*.cpp")

add_executable(Wow WIN32 ${PRIVATE_SOURCES})

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
