# Generates compile_commands.json, which editor tooling (clangd, VS Code's
# C/C++ extension, etc.) uses to resolve include paths and defines correctly.
# Without this, editors fall back to guessing and produce false-positive
# red squiggles even though the actual build succeeds.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

cmake_minimum_required(VERSION 3.22)
project(pacman VERSION 1.0.0 LANGUAGES CXX)

# VRSFML's aggregate/designated-initializer API style and other modern
# features require a recent C++ standard.
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Sources are added via target_sources() below rather than passed here,
# so this is intentionally empty.
add_executable(${PROJECT_NAME})

# Common Source Files and Include Directories
file(GLOB_RECURSE SRC_FILES "src/*.cpp")
target_sources(${PROJECT_NAME} PRIVATE ${SRC_FILES})

# Explicitly point at VRSFML's own include directory rather than relying
# solely on its exported CMake targets to propagate it. This guards against
# accidentally picking up an unrelated SFML installation elsewhere on the
# system (e.g. under /usr/local/include) if the include path ever fails to
# propagate as expected.
target_include_directories(${PROJECT_NAME} PRIVATE
    include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/vrsfml/include
)

# ==============================================================================
# Compile & Link VRSFML for both Native Desktop and WebAssembly (WASM)
# ==============================================================================
message(STATUS "Configuring project to build VRSFML from source...")

# Disable building extras in VRSFML to maximize compilation speed. We only
# need Graphics/Window/System.
set(SFML_BUILD_AUDIO FALSE CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK FALSE CACHE BOOL "" FORCE)
set(SFML_BUILD_EXAMPLES FALSE CACHE BOOL "" FORCE)

# VRSFML's shader preamble logic (getShaderPreamble() in Shader.cpp) picks
# the correct GLSL version string based on this define. Without it, builds
# under Emscripten silently fall through to a GLSL ES 310 preamble instead
# of the WebGL2-compatible GLSL ES 300, and every shader fails to compile
# at runtime with cryptic "invalid layout qualifier" errors. This should
# arguably be set automatically by VRSFML's own CMake when EMSCRIPTEN is
# detected, but isn't in the currently pinned commit — hence setting it
# explicitly here.
if(EMSCRIPTEN)
    add_compile_definitions(SFML_SYSTEM_EMSCRIPTEN)
endif()

# Incorporate the local library repository (see the submodule setup in the
# README before this will actually have any content to build).
add_subdirectory(third_party/vrsfml)

# Link using the modern modular targets defined by the fork.
target_link_libraries(${PROJECT_NAME} PRIVATE 
    SFML::Graphics 
    SFML::Window 
    SFML::System
)

# ==============================================================================
# Platform-Specific Adjustments (Web vs Desktop)
# ==============================================================================
if(EMSCRIPTEN)
    message(STATUS "Configuring Emscripten-specific flags and target outputs...")
    
    # Enforce .html web bundle output (plus matching .js/.wasm/.data files)
    # instead of a bare native-looking executable.
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")

    target_link_options(${PROJECT_NAME} PRIVATE 
        # Bundles the assets/ directory into the wasm build's virtual
        # filesystem at /assets, matching the relative paths our own code
        # already uses (e.g. "./assets/fonts/...").
        "--preload-file" "${CMAKE_CURRENT_SOURCE_DIR}/assets@/assets"

        # SFML/VRSFML's dynamic containers can grow at runtime; without this,
        # Emscripten's default fixed heap size can be exhausted.
        "-sALLOW_MEMORY_GROWTH=1"

        "-sUSE_WEBGL2=1"

        # VRSFML's built-in shader uses `layout(location = N) uniform ...`,
        # which is NOT valid GLSL ES 3.00 (WebGL2) syntax on its own --
        # explicit uniform locations normally require GLSL ES 3.10+ or
        # desktop GL 4.30+. This flag enables Emscripten's
        # GL_EMSCRIPTEN_explicit_uniform_location shim, which rewrites the
        # shader source and emulates explicit uniform locations at the
        # JS/GL-shim layer so VRSFML's hardcoded glUniform*(0/1/3, ...)
        # calls in RenderTarget.cpp continue to hit the correct uniform.
        # Without this, every shader compile fails with
        # "invalid layout qualifier: only valid on program inputs and outputs".
        "-sGL_EXPLICIT_UNIFORM_LOCATION=1"

        # VRSFML's Emscripten-side glue code accesses these typed-array heap
        # views directly; they aren't exported by default and the app aborts
        # at runtime ("'HEAPU32' was not exported...") without this.
        "-sEXPORTED_RUNTIME_METHODS=[HEAPU8,HEAPU16,HEAPU32,HEAP32,HEAPF32]"

        # Our main loop is a blocking `while (running) { ... }`, which is
        # the normal pattern for a native game but is incompatible with a
        # browser's single-threaded event loop -- without yielding back to
        # the browser, the page can never repaint and any blocking/waiting
        # call (e.g. emscripten_sleep, used internally for frame limiting)
        # aborts immediately. Asyncify transforms the compiled code so it
        # can suspend and resume around such calls, unblocking the browser
        # between iterations.
        #
        # This is a known-costly shortcut: Asyncify meaningfully increases
        # binary size and can hurt performance, since Asyncify has to
        # save/restore stack state around every potential suspend point.
        # The more correct long-term fix is restructuring the game loop
        # to be callback-driven via emscripten_set_main_loop (called once
        # per browser animation frame, no blocking/sleeping required) --
        # worth revisiting if binary size or performance becomes a problem.
        "-sASYNCIFY=1"
    )
else()
    message(STATUS "Configuring Desktop-specific asset handling rules...")

    # Copies the assets/ folder next to the built binary so relative asset
    # paths (e.g. "./assets/fonts/...") resolve correctly when running
    # directly from the build directory.
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
        COMMENT "Copying assets folder to target binary directory..."
    )
endif()