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

# SDL3 static config
set(SDL_SHARED ON CACHE BOOL "Build SDL as a shared library")
set(SDL_STATIC OFF CACHE BOOL "Build SDL as a static library")
set(SDL_TESTS OFF CACHE BOOL "Disable SDL tests")

log("Adding SDL3...")
add_subdirectory(Thirdparty/sdl3)

log("Adding SimpleIni...")
set(SIMPLEINI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Thirdparty/simpleini)
include_directories(${SIMPLEINI_DIR})

log("Thirdparty Libraries Configured.")

# -------- Engine & Modules --------
log("Adding Engine...")
add_subdirectory(Sources/Engine)

log("Adding Window Module (SDL3)...")
add_subdirectory(Sources/Modules/Window/Window-SDL3)

# -------- Executables / Applications --------
log("Adding Sandbox Application...")
add_subdirectory(Sources/Sandbox)