# Shader conversion only; no D3D9On12/D3D12 device or mapping layer is linked.
if(NOT EXISTS "${WXL_DX11_WDK_ROOT}/um/d3d10umddi.h")
    message(FATAL_ERROR "Set WXL_DX11_WDK_ROOT to the pinned WDK Include/<version> directory; see Tools/Dx11Research/DEPENDENCIES.md")
endif()
file(GLOB CONVERTER_SOURCES CONFIGURE_DEPENDS
    ShaderConverter/ShaderConv/*.cpp ShaderConverter/ShaderBinary/*.cpp)
add_library(GxD3D9ShaderConverter STATIC ${CONVERTER_SOURCES})
target_include_directories(GxD3D9ShaderConverter PUBLIC
    ShaderConverter/Inc external "${WXL_DX11_WDK_ROOT}/um" "${WXL_DX11_WDK_ROOT}/shared"
    PRIVATE ShaderConverter/ShaderConv)
target_compile_definitions(GxD3D9ShaderConverter PRIVATE NOMINMAX _CRT_SECURE_NO_WARNINGS)
target_compile_options(GxD3D9ShaderConverter PRIVATE /W3 /wd4238 /wd4324 /wd4201)
