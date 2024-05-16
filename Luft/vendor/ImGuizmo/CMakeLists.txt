file(GLOB SRC
  *.cpp
  *.h
)

add_library(imguuizmo ${SRC})
target_include_directories(imguuizmo
PUBLIC .
PRIVATE ${CMAKE_SOURCE_DIR}/Luft/vendor/imgui)

target_link_libraries(imguuizmo imgui)
