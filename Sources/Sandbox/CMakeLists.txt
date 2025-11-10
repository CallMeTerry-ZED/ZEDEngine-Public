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
        ${SOURCES_DIR}/Modules/Input/Input-SDL3/include
        ${THIRDPARTY_DIR}/sdl3/include
        ${THIRDPARTY_DIR}/luau/
)

target_link_libraries(Sandbox PRIVATE
        Engine
        # Window Modules
        Window-GLFW
        Window-SDL3
        # Input Modules
        Input-SDL3
        # Script Modules
        Script-Luau
        # Renderer Modules
        Renderer-D3D11
)

target_compile_definitions(Sandbox PRIVATE
        "ZEDENGINE_API=__declspec(dllimport)"
)

# ---------- Luau compile setup ----------

# Path to luau compiler exe
set(LUAU_COMPILE_EXE
        "${CMAKE_SOURCE_DIR}/Vendor/Windows/Luau/luau-compile.exe"
        CACHE FILEPATH "Path to luau-compile.exe"
)

if(NOT EXISTS "${LUAU_COMPILE_EXE}")
    message(WARNING "luau-compile.exe not found at: ${LUAU_COMPILE_EXE}")
endif()

# Helper CMake script that runs the compiler and writes stdout to a file (no shell redirection)
set(LUAU_COMPILE_SCRIPT "${CMAKE_BINARY_DIR}/cmake/CompileLuau.cmake")
file(WRITE "${LUAU_COMPILE_SCRIPT}" "
if(NOT DEFINED LUAU_EXE OR NOT DEFINED SRC OR NOT DEFINED OUT)
  message(FATAL_ERROR \"CompileLuau.cmake requires LUAU_EXE, SRC, OUT\")
endif()
# Ensure output dir exists
get_filename_component(OUT_DIR \"\${OUT}\" DIRECTORY)
file(MAKE_DIRECTORY \"\${OUT_DIR}\")
# Run luau-compile: use --binary (flag form)
execute_process(
  COMMAND \"\${LUAU_EXE}\" --binary -O2 \"\${SRC}\"
  OUTPUT_FILE \"\${OUT}\"
  RESULT_VARIABLE rv
  ERROR_VARIABLE err
  WORKING_DIRECTORY \"${CMAKE_SOURCE_DIR}\"
)
if(NOT rv EQUAL 0)
  message(FATAL_ERROR \"luau-compile failed (code=\${rv}) for \${SRC}: \${err}\")
endif()
")

# Find all .luau sources under Assets/Scripts
file(GLOB_RECURSE LUAU_SRC_FILES
        "${CMAKE_SOURCE_DIR}/Assets/Scripts/*.luau"
)

set(GENERATED_ASSETS_DIR "${CMAKE_BINARY_DIR}/GeneratedAssets")
set(LUAU_BC_OUTPUTS "")

if(LUAU_SRC_FILES)
    foreach(SRC ${LUAU_SRC_FILES})
        # Mirror the folder structure under GeneratedAssets and output .luau.bc
        file(RELATIVE_PATH REL_FROM_ASSETS "${CMAKE_SOURCE_DIR}/Assets" "${SRC}")  # e.g., Scripts/foo/bar.luau
        get_filename_component(REL_DIR "${REL_FROM_ASSETS}" DIRECTORY)             # e.g., Scripts/foo
        get_filename_component(BASE    "${SRC}" NAME_WE)                            # e.g., bar

        set(OUT_DIR "${GENERATED_ASSETS_DIR}/${REL_DIR}")
        set(OUT_BC  "${OUT_DIR}/${BASE}.luau.bc")

        add_custom_command(
                OUTPUT "${OUT_BC}"
                COMMAND ${CMAKE_COMMAND} -E make_directory "${OUT_DIR}"
                COMMAND ${CMAKE_COMMAND}
                -DLUAU_EXE=${LUAU_COMPILE_EXE}
                -DSRC=${SRC}
                -DOUT=${OUT_BC}
                -P "${LUAU_COMPILE_SCRIPT}"
                DEPENDS "${SRC}"
                COMMENT "Compiling Luau bytecode: ${REL_FROM_ASSETS} -> ${OUT_BC}"
                VERBATIM
        )

        list(APPEND LUAU_BC_OUTPUTS "${OUT_BC}")
    endforeach()

    # Target that holds all bytecode outputs
    add_custom_target(CompileLuauBytecode
            DEPENDS ${LUAU_BC_OUTPUTS}
    )

    # Building Sandbox will trigger bytecode compilation first
    add_dependencies(Sandbox CompileLuauBytecode)
endif()

# ---------- Stage everything for Sandbox (runs when building Sandbox) ----------

add_custom_target(StageSandbox
        # Ensure the output dir exists
        COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_FILE_DIR:Sandbox>"

        # 1) Copy Assets/
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/Assets"
        "$<TARGET_FILE_DIR:Sandbox>/Assets"

        # 2) Overlay compiled .bc files
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${GENERATED_ASSETS_DIR}"
        "$<TARGET_FILE_DIR:Sandbox>/Assets"

        # 3) Copy Luau tool
        COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_FILE_DIR:Sandbox>/Tools/Luau"
        COMMAND ${CMAKE_COMMAND} -E copy
        "${LUAU_COMPILE_EXE}"
        "$<TARGET_FILE_DIR:Sandbox>/Tools/Luau/luau-compile.exe"

        # 4) Copy SDL3 runtime next to the exe
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3::SDL3-shared>
        $<TARGET_FILE_DIR:Sandbox>

        # 5) Copy INI into Configs/
        COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_FILE_DIR:Sandbox>/Configs"
        COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_SOURCE_DIR}/Sources/Configs/zedengine.ini"
        "$<TARGET_FILE_DIR:Sandbox>/Configs/zedengine.ini"

        COMMENT "StageSandbox: Assets, .bc, Tools/Luau, SDL DLL, INI"
)

# If we generated bytecode, staging should wait on it
if(TARGET CompileLuauBytecode)
    add_dependencies(StageSandbox CompileLuauBytecode)
endif()

# Building Sandbox will run staging first
add_dependencies(Sandbox StageSandbox)

# ---------- Post Build ----------

add_custom_command(TARGET Sandbox POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3::SDL3-shared>
        $<TARGET_FILE_DIR:Sandbox>)