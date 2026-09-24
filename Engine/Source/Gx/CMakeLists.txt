file(GLOB_RECURSE PRIVATE_SOURCES CONFIGURE_DEPENDS "*.cpp")

if (NOT WIN32)
    list(FILTER PRIVATE_SOURCES EXCLUDE REGEX "/CGxDeviceD3d/")
endif ()

add_library(Gx STATIC
    ${PRIVATE_SOURCES}
)

target_include_directories(Gx
    PUBLIC
        ${CMAKE_SOURCE_DIR}
)

target_link_libraries(Gx
    PRIVATE
        d3d9.lib
)
