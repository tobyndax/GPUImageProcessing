#Include the include 
include_directories(${CMAKE_SOURCE_DIR}/../include)
#Include SDL2
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/sdl2-2.0.5/include)
#Also include core itself
include_directories(${CMAKE_SOURCE_DIR}/core)
#Include the own folder
include_directories(${CMAKE_SOURCE_DIR}/app)

#Set the link directory to the output_dir for libs
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
link_directories(${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
link_directories(${CMAKE_RUNTIME_OUTPUT_DIRECTORY})



#create app.exe based on entrypoint in main.cpp
file (GLOB SRC_FILES "*.cpp")  
file (GLOB HDR_FILES "*.h")

add_executable(app ${SRC_FILES} ${HDR_FILES})
#add_executable(app main.cpp PNGLoader.cpp)
#app links core.lib



target_link_libraries(app debug core.dll)
target_link_libraries(app optimized core.dll)

target_link_libraries(app debug sdl2main.lib)
target_link_libraries(app optimized sdl2main.lib)

target_link_libraries(app debug sdl2.lib)
target_link_libraries(app optimized sdl2.lib)



