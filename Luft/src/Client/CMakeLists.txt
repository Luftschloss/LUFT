file(GLOB_RECURSE HEADER_FILES "*.h")
file(GLOB_RECURSE SOURCE_FILES "*.cpp")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADER_FILES} ${SOURCE_FILES})

add_executable(Luft-Client ${HEADER_FILES} ${SOURCE_FILES})
target_include_directories(Luft-Client PRIVATE ../Luft)
target_link_libraries(Luft-Client Luft)
add_dependencies(Luft-Client Luft)

set_target_properties(
  Luft-Client PROPERTIES
  VS_DEBUGGER_WORKING_DIRECTORY ${target_directory}
)