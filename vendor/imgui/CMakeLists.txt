file(GLOB SRC
  *.cpp
  backends/imgui_impl_sdl.cpp
  backends/imgui_impl_opengl3.cpp
  misc/freetype/*.cpp
)

add_library(imgui ${SRC})
target_include_directories(imgui PUBLIC . backends misc/freetype)
target_link_libraries(imgui sdl2 freetype)