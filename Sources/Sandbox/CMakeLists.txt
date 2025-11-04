cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)

set(THIRDPARTY_DIR ${CMAKE_SOURCE_DIR}/Thirdparty)
set(SOURCES_DIR ${CMAKE_SOURCE_DIR}/Sources)

file(GLOB_RECURSE SANDBOX_SRC CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

file(GLOB_RECURSE SANDBOX_INC CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
)

add_executable(Sandbox
        ${SANDBOX_SRC}
        ${SANDBOX_INC}
)

target_include_directories(Sandbox PUBLIC
        ${SOURCES_DIR}/Engine/include
        ${SOURCES_DIR}/Modules/Window/Window-SDL3/include
        ${SOURCES_DIR}/Modules/Window/Window-GLFW/include
        ${THIRDPARTY_DIR}/sdl3/include
)

target_link_libraries(Sandbox PRIVATE
        Engine
        Window-GLFW
        Window-SDL3
)

target_compile_definitions(Sandbox PRIVATE
        "ZEDENGINE_API=__declspec(dllimport)"
)

add_custom_command(TARGET Sandbox POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3::SDL3-shared>
        $<TARGET_FILE_DIR:Sandbox>
)

add_custom_command(TARGET Sandbox POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
        $<TARGET_FILE_DIR:Sandbox>/Configs
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/Sources/Configs/zedengine.ini
        $<TARGET_FILE_DIR:Sandbox>/Configs/zedengine.ini
)