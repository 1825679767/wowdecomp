file(GLOB_RECURSE PRIVATE_SOURCES CONFIGURE_DEPENDS "*.cpp")

add_library(Frame STATIC
    ${PRIVATE_SOURCES}
)

target_include_directories(Frame
    PUBLIC
        ${CMAKE_SOURCE_DIR}
)
