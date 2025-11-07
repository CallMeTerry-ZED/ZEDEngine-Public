cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)

set(THIRDPARTY_DIR ${CMAKE_SOURCE_DIR}/Thirdparty)
set(SOURCES_DIR ${CMAKE_SOURCE_DIR}/Sources)

file(GLOB_RECURSE INPUT_SDL3_SRC CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

file(GLOB_RECURSE INPUT_SDL3_INC CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
)

add_library(Input-SDL3 SHARED
        ${INPUT_SDL3_SRC}
        ${INPUT_SDL3_INC}
)

target_include_directories(Input-SDL3 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${SOURCES_DIR}/Engine/include
        ${THIRDPARTY_DIR}/sdl3/include
)

target_link_libraries(Input-SDL3 PRIVATE
        Engine
        SDL3::SDL3-shared
)

target_compile_definitions(Input-SDL3 PRIVATE
        "ZEDENGINE_API=__declspec(dllexport)"
        ZEDENGINE_EXPORTS
)