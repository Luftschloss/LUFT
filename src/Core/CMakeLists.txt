configure_file(
  Version.h.in
  ${CMAKE_CURRENT_SOURCE_DIR}/Version.h
  @ONLY)

file(GLOB_RECURSE SRC
  *.c
  *.cpp
  *.h
  Version.h.in
)

add_library(Luft-Core ${SRC})