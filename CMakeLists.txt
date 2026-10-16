cmake_minimum_required(VERSION 3.24)
project(Doldrum VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Prefer an installed SDL3 package; otherwise fetch the known SDL3 release.
find_package(SDL3 CONFIG QUIET)
if(NOT TARGET SDL3::SDL3)
    include(FetchContent)
    FetchContent_Declare(SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.2.0
        GIT_SHALLOW TRUE
    )
    set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
    set(SDL_TESTS OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(SDL3)
endif()

add_executable(Doldrum src/main.cpp src/world/Room.cpp)
target_include_directories(Doldrum PRIVATE src)
add_custom_command(TARGET Doldrum POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/data
        $<TARGET_FILE_DIR:Doldrum>/data
    VERBATIM)
target_link_libraries(Doldrum PRIVATE SDL3::SDL3)

if(MSVC)
    target_compile_options(Doldrum PRIVATE /W4 /permissive-)
else()
    target_compile_options(Doldrum PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Keep the SDL shared library next to the executable where supported.
if(WIN32 AND TARGET SDL3::SDL3-shared)
    add_custom_command(TARGET Doldrum POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3-shared>
            $<TARGET_FILE_DIR:Doldrum>
        VERBATIM)
endif()
