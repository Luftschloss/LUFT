add_library(freetype SHARED IMPORTED GLOBAL)
target_include_directories(freetype INTERFACE include)
set_target_properties(freetype PROPERTIES
IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/x64/freetype.lib"
IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/bin/x64/freetype.dll"
)