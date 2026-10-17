cmake_minimum_required(VERSION 3.24)
project(Doldrum VERSION 0.3.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
find_package(SDL3 CONFIG QUIET)
if(NOT TARGET SDL3::SDL3)
  include(FetchContent)
  FetchContent_Declare(SDL3 GIT_REPOSITORY https://github.com/libsdl-org/SDL.git GIT_TAG release-3.2.0 GIT_SHALLOW TRUE)
  set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
  set(SDL_TESTS OFF CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(SDL3)
endif()
find_package(SDL3_image CONFIG QUIET)
if(NOT TARGET SDL3_image::SDL3_image)
    include(FetchContent)
    FetchContent_Declare(SDL3_image
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
        GIT_TAG release-3.2.0
        GIT_SHALLOW TRUE
    )
    set(SDLIMAGE_VENDORED ON CACHE BOOL "" FORCE)
    set(SDLIMAGE_AVIF OFF CACHE BOOL "" FORCE)
    set(SDLIMAGE_WEBP OFF CACHE BOOL "" FORCE)
    set(SDLIMAGE_JXL OFF CACHE BOOL "" FORCE)
    set(SDLIMAGE_TIF OFF CACHE BOOL "" FORCE)
    set(SDLIMAGE_PNG ON CACHE BOOL "" FORCE)
    set(SDLIMAGE_SAMPLES OFF CACHE BOOL "" FORCE)
    set(SDLIMAGE_TESTS OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(SDL3_image)
endif()

add_executable(Doldrum
    src/main.cpp
    src/core/AppSystem.cpp
    src/world/Room.cpp
    src/world/WorldSystem.cpp
    src/world/Tiles.cpp
    src/entities/Player.cpp
    src/entities/Squib.cpp
    src/combat/Projectile.cpp
    src/render/DrawUtils.cpp
    src/render/Renderer.cpp
    src/save/SaveSystem.cpp
    src/ui/MenuSystem.cpp
    src/ui/PixelFont.cpp)
target_include_directories(Doldrum PRIVATE src)
target_link_libraries(Doldrum PRIVATE SDL3::SDL3 SDL3_image::SDL3_image)
add_custom_command(TARGET Doldrum POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/data $<TARGET_FILE_DIR:Doldrum>/data
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets $<TARGET_FILE_DIR:Doldrum>/assets
  VERBATIM)
if(MSVC)
  target_compile_options(Doldrum PRIVATE /W4 /permissive-)
else()
  target_compile_options(Doldrum PRIVATE -Wall -Wextra -Wpedantic)
endif()
if(WIN32 AND TARGET SDL3::SDL3-shared)
  add_custom_command(TARGET Doldrum POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:SDL3::SDL3-shared> $<TARGET_FILE_DIR:Doldrum> VERBATIM)
endif()

if(WIN32 AND TARGET SDL3_image::SDL3_image-shared)
    add_custom_command(TARGET Doldrum POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3_image::SDL3_image-shared> $<TARGET_FILE_DIR:Doldrum> VERBATIM)
endif()
