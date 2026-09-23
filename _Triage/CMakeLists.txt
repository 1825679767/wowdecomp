file(GLOB_RECURSE PRIVATE_SOURCES CONFIGURE_DEPENDS "*.cpp")

add_library(Triage STATIC
    ${PRIVATE_SOURCES}
)

target_include_directories(Triage
    PUBLIC
        ${CMAKE_SOURCE_DIR}
)
