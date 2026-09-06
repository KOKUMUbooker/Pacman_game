set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
cmake_minimum_required(VERSION 3.22)
project(pacman VERSION 1.0.0 LANGUAGES CXX)

# VRSFML requires a modern C++ compiler supporting C++20 features
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define the main application target
add_executable(${PROJECT_NAME})

# Common Source Files and Include Directories
file(GLOB_RECURSE SRC_FILES "src/*.cpp")
target_sources(${PROJECT_NAME} PRIVATE ${SRC_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE include)

# include header files for vrsfml
target_include_directories(${PROJECT_NAME} PRIVATE
    include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/vrsfml/include
)

# ==============================================================================
# Compile & Link VRSFML for both Native Desktop and WebAssembly (WASM)
# ==============================================================================
message(STATUS "Configuring project to build VRSFML from source...")

# Disable building extras in VRSFML to maximize compilation speed
set(SFML_BUILD_AUDIO FALSE CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK FALSE CACHE BOOL "" FORCE)
set(SFML_BUILD_EXAMPLES FALSE CACHE BOOL "" FORCE)

if(EMSCRIPTEN)
    add_compile_definitions(SFML_SYSTEM_EMSCRIPTEN)
endif()

# Incorporate the local library repository
add_subdirectory(third_party/vrsfml)

# Link using the modern modular targets defined by the fork
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
    
    # Enforce .html web bundle output
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")

    # Mirroring your original xmake linker optimizations
    target_link_options(${PROJECT_NAME} PRIVATE 
        "--preload-file" "${CMAKE_CURRENT_SOURCE_DIR}/assets@/assets"
        "-sALLOW_MEMORY_GROWTH=1"
        "-sUSE_WEBGL2=1"
        "-sGL_EXPLICIT_UNIFORM_LOCATION=1"
        "-sEXPORTED_RUNTIME_METHODS=[HEAPU8,HEAPU16,HEAPU32,HEAP32,HEAPF32]"
        "-sASYNCIFY=1"
    )
else()
    message(STATUS "Configuring Desktop-specific asset handling rules...")

    # Replicate your xmake after_build asset copy rule for desktop debugging
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
        COMMENT "Copying assets folder to target binary directory..."
    )
endif()
