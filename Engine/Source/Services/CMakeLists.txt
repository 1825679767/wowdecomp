file(GLOB_RECURSE PRIVATE_SOURCES CONFIGURE_DEPENDS "*.cpp")

add_library(Services STATIC
    ${PRIVATE_SOURCES}
)

target_include_directories(Services
    PUBLIC
        ${CMAKE_SOURCE_DIR}
)
