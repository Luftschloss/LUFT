file(GLOB SRC
  *.cpp
  *.h
  backends/imgui_impl_sdl2.cpp
  backends/imgui_impl_sdl2.h
  backends/imgui_impl_vulkan.cpp
  backends/imgui_impl_vulkan.h
  misc/freetype/*.cpp
  misc/freetype/*.h
)

add_library(imgui ${SRC})
target_include_directories(imgui
PRIVATE ${VK_SDK_INCLUDE}
PUBLIC . backends misc/freetype
)
target_link_libraries(imgui sdl2 freetype)
