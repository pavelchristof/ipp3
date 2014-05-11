# Find all sources, headers and ui files.
file(GLOB_RECURSE IPP3_SOURCES  ./*.cpp)
file(GLOB_RECURSE IPP3_HEADERS  ./*.hpp)
file(GLOB_RECURSE IPP3_UI_FILES ./*.ui)

# Generate UI headers.
qt5_wrap_ui(IPP3_UI_HEADERS ${IPP3_UI_FILES})

# Compile the executable.
add_executable(ipp3 ${IPP3_SOURCES} ${IPP3_UI_HEADERS})
target_link_libraries(ipp3 Qt5::Core Qt5::Widgets)

# Install the compiled binary.
install(TARGETS ipp3 RUNTIME DESTINATION bin)
