cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)

set(THIRDPARTY_DIR ${CMAKE_SOURCE_DIR}/Thirdparty)
set(SOURCES_DIR ${CMAKE_SOURCE_DIR}/Sources)

file(GLOB_RECURSE ENGINE_SRC CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

file(GLOB_RECURSE ENGINE_INC CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
)

add_library(Engine SHARED
        ${ENGINE_SRC}
        ${ENGINE_INC}
)

target_include_directories(Engine PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${THIRDPARTY_DIR}/sdl3/include
        ${THIRDPARTY_DIR}/glfw/include
        ${SIMPLEINI_DIR}
        ${THIRDPARTY_DIR}/luau
        ${THIRDPARTY_DIR}/entt/src
        ${THIRDPARTY_DIR}/glm
)

target_link_libraries(Engine PRIVATE
        Jolt::Jolt
        SDL3::SDL3-shared
        glfw
        EnTT::EnTT
        Luau.VM
        Luau.Common
        Luau.Compiler
        Luau.CodeGen
        Luau.Config
        Luau.Require
        Bullet3Common
        Bullet3Dynamics
        Bullet3Geometry
        BulletDynamics
        BulletCollision
        LinearMath
        BulletSoftBody
        BulletInverseDynamics
        BulletFileLoader
        BulletWorldImporter
        d3d11
        dxgi
        d3dcompiler
        glm::glm
)

target_compile_definitions(Engine PRIVATE
        "ZEDENGINE_API=__declspec(dllexport)"
        ZEDENGINE_EXPORTS
        GLFW_INCLUDE_NONE

        # DirectX math conventions for GLM
        GLM_FORCE_LEFT_HANDED
        GLM_FORCE_DEPTH_ZERO_TO_ONE
        GLM_FORCE_RADIANS
)
