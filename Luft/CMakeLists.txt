configure_file(
  src/Version.h.in
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Version.h
  @ONLY)

add_subdirectory(vendor/spdlog)
add_subdirectory(vendor/freetype)
add_subdirectory(vendor/sdl)
add_subdirectory(vendor/imgui)
add_subdirectory(vendor/ImGuizmo)
add_subdirectory(src/Client)

file(GLOB_RECURSE HEADER_FILES "src/*.h" "src/*.in")
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADER_FILES} ${SOURCE_FILES})

add_library(Luft ${HEADER_FILES} ${SOURCE_FILES})

set(CORE_DEFINITIONS PUBLIC LUFT_PLATFORM_WINDOWS LUFT_BUILD_DLL)
target_compile_definitions(Luft ${CORE_DEFINITIONS})

if(NOT DEFINED ENV{VK_SDK_PATH})
	message(FATAL_ERROR "not defined vulkan sdk env :VK_SDK_PATH £¬ check")  
endif()
set(VK_SDK_PATH $ENV{VK_SDK_PATH})
set(VK_SDK_INCLUDE ${VK_SDK_PATH}/Include)
set(VK_SDK_LIB ${VK_SDK_PATH}/Lib/vulkan-1.lib)

target_include_directories(Luft
 PRIVATE vendor/spdlog/include
 PRIVATE ${VK_SDK_INCLUDE}
 PUBLIC src)
target_link_libraries(Luft spdlog imgui imguuizmo ${VK_SDK_LIB})
