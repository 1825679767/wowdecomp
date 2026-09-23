file(GLOB_RECURSE PRIVATE_SOURCES CONFIGURE_DEPENDS "*.cpp")

add_library(DayNight STATIC
    ${PRIVATE_SOURCES}
)

target_include_directories(DayNight
    PUBLIC
        ${CMAKE_SOURCE_DIR}
)
