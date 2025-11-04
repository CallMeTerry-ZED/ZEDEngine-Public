cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)

set(THIRDPARTY_DIR ${CMAKE_SOURCE_DIR}/Thirdparty)
set(SOURCES_DIR ${CMAKE_SOURCE_DIR}/Sources)

file(GLOB_RECURSE WINDOW_GLFW_SRC CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

file(GLOB_RECURSE WINDOW_GLFW_INC CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
)

add_library(Window-GLFW SHARED
        ${WINDOW_GLFW_SRC}
        ${WINDOW_GLFW_INC}
)

target_include_directories(Window-GLFW PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${SOURCES_DIR}/Engine/include
        ${THIRDPARTY_DIR}/glfw/include
)

target_link_libraries(Window-GLFW PRIVATE
        Engine
        glfw
        opengl32
)

target_compile_definitions(Window-GLFW PRIVATE
        "ZEDENGINE_API=__declspec(dllexport)"
        ZEDENGINE_EXPORTS
        GLFW_INCLUDE_NONE
)