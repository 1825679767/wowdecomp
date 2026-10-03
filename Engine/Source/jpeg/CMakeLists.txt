file(GLOB_RECURSE PRIVATE_SOURCES CONFIGURE_DEPENDS "*.cpp")

add_library(jpegmem STATIC
    ${PRIVATE_SOURCES}
)

target_include_directories(jpegmem
    PUBLIC
        ${CMAKE_SOURCE_DIR}
)

target_link_libraries(jpegmem
    PUBLIC
        jpeg
)
