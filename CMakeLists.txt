cmake_minimum_required(VERSION 3.20)
project(ZEDEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# -------- Architecture and Config --------
string(TOLOWER "${CMAKE_BUILD_TYPE}" BUILD_CONFIG)
set(ARCH_DIR ${CMAKE_SYSTEM_PROCESSOR})

# -------- Output Directories --------
set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/Bin/${ARCH_DIR}/${CMAKE_BUILD_TYPE})
set(INT_OUTPUT_DIR ${CMAKE_BINARY_DIR}/Bin-Int/${ARCH_DIR}/${CMAKE_BUILD_TYPE})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${INT_OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})

# -------- Logging Macro --------
macro(log msg)
    message(STATUS "====> ${msg}")
endmacro()

# -------- Thirdparty Config --------
log("Configuring Thirdparty Libraries...")

# SDL3 config
set(SDL_SHARED ON CACHE BOOL "Build SDL as a shared library")
set(SDL_STATIC OFF CACHE BOOL "Build SDL as a static library")
set(SDL_TESTS OFF CACHE BOOL "Disable SDL tests")
log("Adding SDL3...")
add_subdirectory(Thirdparty/sdl3)

log("Adding SimpleIni...")
set(SIMPLEINI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Thirdparty/simpleini)
include_directories(${SIMPLEINI_DIR})

# Jolt Physics Config
set(JPH_BUILD_SHARED_LIBS OFF CACHE BOOL "Compile Jolt as a shared library")
set(JPH_SHARED_LIBRARY OFF CACHE BOOL "Sets Jolt as a shared library")
set(JPH_DEBUG_RENDERER ON CACHE BOOL "Adds support to draw lines and triangles")
set(JPH_CROSS_PLATFORM_DETERMINISTIC ON CACHE BOOL "Turns on behavior to attempt cross platform determinism")
set(JPH_DET_LOG ON CACHE BOOL "Turns on a lot of extra logging to help debug determinism issues when JPH_CROSS_PLATFORM_DETERMINISTIC is turned on")
set(CPP_RTTI_ENABLED ON CACHE BOOL "Enables RTTI inside Jolt to avoid typeinfo link errors")
log("Adding Jolt Physics...")
add_subdirectory(Thirdparty/jolt/Build)

# Bullet Physics Config
set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_STATIC_LIBS ON CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "")
set(BUILD_UNIT_TESTS OFF CACHE BOOL "")
set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "")
set(BUILD_BULLET3 ON CACHE BOOL "")
set(BUILD_CPU_DEMOS OFF CACHE BOOL "")
set(BUILD_BULLET_ROBOTICS OFF CACHE BOOL "")
set(BUILD_EXTRAS ON CACHE BOOL "")
set(BUILD_ENET OFF CACHE BOOL "")
set(BUILD_CLSOCKET OFF CACHE BOOL "")
set(BUILD_OPENGL3_DEMOS OFF CACHE BOOL "")
set(INSTALL_EXTRA_LIBS OFF CACHE BOOL "")
set(INSTALL_LIBS OFF CACHE BOOL "")
log("Adding Bullet Physics...")
add_subdirectory(Thirdparty/bullet3)

log("Adding GLFW...")
add_subdirectory(Thirdparty/glfw)

log("Adding Luau...")
add_subdirectory(Thirdparty/luau)

log("Adding Entt...")
add_subdirectory(Thirdparty/entt)

log("Thirdparty Libraries Configured.")

# -------- Engine & Modules --------
log("Adding Engine...")
add_subdirectory(Sources/Engine)

log("Setting up Window Modules...")

log("Adding Window Module: SDL3...")
add_subdirectory(Sources/Modules/Window/Window-SDL3)

log("Adding Window Module: GLFW...")
add_subdirectory(Sources/Modules/Window/Window-GLFW)

log("Window Modules Setup Complete!")

log("Setting up Input Modules...")

log("Adding Input Module: SDL3...")
add_subdirectory(Sources/Modules/Input/Input-SDL3)

log("Input Modules Setup Complete!")

log("Setting up Scripting Modules...")

log("Adding Scripting Module: Luau...")
add_subdirectory(Sources/Modules/Script/Script-Luau)

log("Scripting Modules Setup Complete!")

# -------- Executables / Applications --------
log("Adding Sandbox Application...")
add_subdirectory(Sources/Sandbox)