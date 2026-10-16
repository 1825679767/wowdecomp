file(GLOB_RECURSE PRIVATE_SOURCES CONFIGURE_DEPENDS "*.cpp")
if(NOT WXL_ENABLE_DX11)
    list(FILTER PRIVATE_SOURCES EXCLUDE REGEX "/CGxDeviceD3d11/|/LegacyShaderTranslator\\.cpp$")
endif()

if (NOT WIN32)
    list(FILTER PRIVATE_SOURCES EXCLUDE REGEX "/CGxDevice(D3d|D3d9Ex|OpenGL)/")
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
        zlib
)

if (WIN32)
    target_link_libraries(Gx
        PRIVATE
            nvapi
            opengl32.lib
    )
endif ()
if(WXL_ENABLE_DX11)
    target_link_libraries(Gx PRIVATE GxD3D9ShaderConverter d3d11 dxgi d3dcompiler dxguid)
endif()
