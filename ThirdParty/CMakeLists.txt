add_library(zlib STATIC
    zlib/adler32.c
    zlib/compress.c
    zlib/crc32.c
    zlib/deflate.c
    zlib/gzio.c
    zlib/infback.c
    zlib/inffast.c
    zlib/inflate.c
    zlib/inftrees.c
    zlib/trees.c
    zlib/uncompr.c
    zlib/zutil.c
)

target_include_directories(zlib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/zlib
)

add_library(blast STATIC
    zlib/contrib/blast/blast.c
)

target_include_directories(blast
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/zlib/contrib/blast
)

add_library(bzip2 STATIC
    bzip2/blocksort.c
    bzip2/bzlib.c
    bzip2/compress.c
    bzip2/crctable.c
    bzip2/decompress.c
    bzip2/huffman.c
    bzip2/randtable.c
)

target_include_directories(bzip2
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/bzip2
)

add_library(lzma STATIC
    lzma/LzmaDecode.c
)

target_include_directories(lzma
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/lzma
)

if (WIN32)
    add_library(nvapi INTERFACE)

    target_include_directories(nvapi
        INTERFACE
            ${CMAKE_CURRENT_SOURCE_DIR}/nvapi
    )

    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        target_link_libraries(nvapi INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/nvapi/amd64/nvapi64.lib)
    else ()
        target_link_libraries(nvapi INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/nvapi/x86/nvapi.lib)
    endif ()
endif ()

add_library(freetype STATIC
    freetype/src/base/ftsystem.c
    freetype/src/base/ftinit.c
    freetype/src/base/ftdebug.c
    freetype/src/base/ftbase.c
    freetype/src/autohint/autohint.c
    freetype/src/cff/cff.c
    freetype/src/cid/type1cid.c
    freetype/src/psaux/psaux.c
    freetype/src/psnames/psnames.c
    freetype/src/raster/raster.c
    freetype/src/sfnt/sfnt.c
    freetype/src/smooth/smooth.c
    freetype/src/truetype/truetype.c
    freetype/src/type1/type1.c
    freetype/src/winfonts/winfnt.c
)

target_include_directories(freetype
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/freetype/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/freetype/src
)
