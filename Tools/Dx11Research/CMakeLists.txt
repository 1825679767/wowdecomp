cmake_minimum_required(VERSION 3.20)
project(Dx11Research LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
get_filename_component(ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
add_library(Dx11Zlib STATIC
    ${ROOT}/ThirdParty/zlib/adler32.c ${ROOT}/ThirdParty/zlib/crc32.c
    ${ROOT}/ThirdParty/zlib/inffast.c ${ROOT}/ThirdParty/zlib/inflate.c
    ${ROOT}/ThirdParty/zlib/inftrees.c ${ROOT}/ThirdParty/zlib/zutil.c
    ${ROOT}/ThirdParty/zlib/compress.c ${ROOT}/ThirdParty/zlib/deflate.c
    ${ROOT}/ThirdParty/zlib/trees.c)
add_library(GxShaderContainer STATIC ${ROOT}/Engine/Source/Gx/ShaderContainer/ShaderContainer.cpp)
target_include_directories(GxShaderContainer PUBLIC ${ROOT})
target_link_libraries(GxShaderContainer PUBLIC Dx11Zlib)
add_executable(ShaderContainerTests ShaderContainerTests.cpp)
target_link_libraries(ShaderContainerTests PRIVATE GxShaderContainer)
add_executable(ShaderInspect ShaderInspect.cpp)
target_link_libraries(ShaderInspect PRIVATE GxShaderContainer)
if(WIN32)
    add_executable(Dx11Offscreen Dx11Offscreen.cpp)
    target_link_libraries(Dx11Offscreen PRIVATE GxShaderContainer d3d11 d3dcompiler dxguid)
    target_link_libraries(ShaderInspect PRIVATE d3dcompiler dxguid)
endif()
enable_testing()
add_test(NAME ShaderContainerBounds COMMAND ShaderContainerTests)
if(WIN32)
    add_test(NAME Dx11WarpOffscreen COMMAND Dx11Offscreen)
endif()
