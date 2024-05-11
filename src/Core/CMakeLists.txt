configure_file(
  Version.h.in
  ${CMAKE_CURRENT_SOURCE_DIR}/Version.h
  @ONLY)

file(GLOB_RECURSE HEADER_FILES "*.h" "*.in")
file(GLOB_RECURSE SOURCE_FILES "*.cpp")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADER_FILES} ${SOURCE_FILES})

add_library(Luft-Core ${HEADER_FILES} ${SOURCE_FILES})

set(LC_DEFINITIONS PUBLIC LUFT_PLATFORM_WINDOWS LUFT_BUILD_DLL)
target_compile_definitions(Luft-Core ${LC_DEFINITIONS})

target_include_directories(Luft-Core
 PRIVATE ${CMAKE_SOURCE_DIR}/vendor/spdlog/include)
target_link_libraries(Luft-Core spdlog imgui)
