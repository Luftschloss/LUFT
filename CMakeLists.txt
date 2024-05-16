cmake_minimum_required(VERSION 3.24)

project(LUFT)
set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Luft-Client)

set(target_directory ${CMAKE_CURRENT_BINARY_DIR} CACHE PATH "target directory")

set(CMAKE_CXX_STANDARD 17)
set(luft_version_major 1 CACHE INTERNAL "version major")
set(luft_version_minor 0 CACHE INTERNAL "version minor")
set(luft_version_patch 0 CACHE INTERNAL "version patch")
set(package_name "Luft-Engine")

set(rt_files
  ${CMAKE_SOURCE_DIR}/Luft/vendor/sdl/lib/x64/SDL2.dll
)
file(COPY
  ${rt_files}
  DESTINATION ${target_directory}
)

add_subdirectory(Luft)