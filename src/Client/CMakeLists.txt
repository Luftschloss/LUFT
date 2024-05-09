file(GLOB_RECURSE SRC
  *.c
  *.cpp
  *.h
)

add_executable(Luft-Client ${SRC})
target_link_libraries(Luft-Client Luft-Core)
target_include_directories(Luft-Client PRIVATE ../Core)
#add_dependencies(Luft-Client Luft-Core)