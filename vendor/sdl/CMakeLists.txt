add_library(sdl2 INTERFACE)
target_include_directories(sdl2 INTERFACE include)
target_link_directories(sdl2 INTERFACE lib/x64)
target_link_libraries(sdl2 INTERFACE SDL2 SDL2main)