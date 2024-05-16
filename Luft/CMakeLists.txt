configure_file(
  src/Version.h.in
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Version.h
  @ONLY)

add_subdirectory(vendor/spdlog)
add_subdirectory(vendor/freetype)
add_subdirectory(vendor/sdl)
add_subdirectory(vendor/imgui)
add_subdirectory(src/Client)

file(GLOB_RECURSE HEADER_FILES "src/*.h" "src/*.in")
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADER_FILES} ${SOURCE_FILES})

add_library(Luft ${HEADER_FILES} ${SOURCE_FILES})

set(LC_DEFINITIONS PUBLIC LUFT_PLATFORM_WINDOWS LUFT_BUILD_DLL)
target_compile_definitions(Luft ${LC_DEFINITIONS})

target_include_directories(Luft
 PRIVATE vendor/spdlog/include
 PUBLIC src)
target_link_libraries(Luft spdlog imgui)
