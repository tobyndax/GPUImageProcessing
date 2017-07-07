#Include the include directory
include_directories(${CMAKE_SOURCE_DIR}/../include)
#Also include core itself
include_directories(${CMAKE_SOURCE_DIR}/core)

#Set the link directory to the output_dir for libs
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

#create app.exe based on entrypoint in main.cpp
add_executable(app main.cpp)

#app links core.lib
target_link_libraries(app debug core.lib)
target_link_libraries(app optimized core.lib)