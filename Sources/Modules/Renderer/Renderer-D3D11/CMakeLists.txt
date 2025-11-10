cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)

set(THIRDPARTY_DIR ${CMAKE_SOURCE_DIR}/Thirdparty)
set(SOURCES_DIR ${CMAKE_SOURCE_DIR}/Sources)

file(GLOB_RECURSE RENDERER_D3D11_SRC CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

file(GLOB_RECURSE RENDERER_D3D11_INC CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
)

add_library(Renderer-D3D11 SHARED
        ${RENDERER_D3D11_SRC}
        ${RENDERER_D3D11_INC}
)

target_include_directories(Renderer-D3D11 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${SOURCES_DIR}/Engine/include
)

target_link_libraries(Renderer-D3D11 PRIVATE
        Engine
        d3d11
        dxgi
        d3dcompiler
)

target_compile_definitions(Renderer-D3D11 PRIVATE
        "ZEDENGINE_API=__declspec(dllexport)"
        ZEDENGINE_EXPORTS
)