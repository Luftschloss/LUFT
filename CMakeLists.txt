cmake_minimum_required(VERSION 3.24)

project(LUFT)
set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Luft-Client)

set(CMAKE_CXX_STANDARD 17)
set(luft_version_major 1 CACHE INTERNAL "version major")
set(luft_version_minor 0 CACHE INTERNAL "version minor")
set(luft_version_patch 0 CACHE INTERNAL "version patch")
set(package_name "Luft-Engine")


add_subdirectory(vendor/freetype)
add_subdirectory(vendor/sdl)
add_subdirectory(vendor/imgui)
add_subdirectory(src/Core)
add_subdirectory(src/Client)