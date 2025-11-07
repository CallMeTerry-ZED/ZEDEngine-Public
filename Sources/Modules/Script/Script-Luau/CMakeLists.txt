cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)

set(THIRDPARTY_DIR ${CMAKE_SOURCE_DIR}/Thirdparty)
set(SOURCES_DIR ${CMAKE_SOURCE_DIR}/Sources)

file(GLOB_RECURSE SCRIPT_LUAU_SRC CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

file(GLOB_RECURSE SCRIPT_LUAU_INC CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
)

add_library(Script-Luau SHARED
        ${SCRIPT_LUAU_SRC}
        ${SCRIPT_LUAU_INC}
)

target_include_directories(Script-Luau PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${SOURCES_DIR}/Engine/include
        ${THIRDPARTY_DIR}/luau
)

target_link_libraries(Script-Luau PRIVATE
        Engine
        Luau.VM
        Luau.Common
        Luau.Compiler
        Luau.CodeGen
        Luau.Config
        Luau.Require
)

target_compile_definitions(Script-Luau PRIVATE
        "ZEDENGINE_API=__declspec(dllexport)"
        ZEDENGINE_EXPORTS
)