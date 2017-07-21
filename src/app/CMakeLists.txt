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


IF(MSVC)
  message(STATUS "MSVC platform detected in app project")
  target_link_libraries(app debug core.dll)
  target_link_libraries(app optimized core.dll)

  target_link_libraries(app debug sdl2main.lib)
  target_link_libraries(app optimized sdl2main.lib)

  target_link_libraries(app debug sdl2.lib)
  target_link_libraries(app optimized sdl2.lib)

  find_package(OpenCL REQUIRED)
  include_directories(${OpenCL_INCLUDE_DIRS})
  link_directories(${OpenCL_LIBRARY})

  target_link_libraries(app debug ${OpenCL_LIBRARY})
  target_link_libraries(app optimized ${OpenCL_LIBRARY})


ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  message(STATUS "Mac OS X platform detected in app project")
  target_link_libraries(app debug core.dylib)
  target_link_libraries(app optimized core.dylib)

  target_link_libraries(app debug sdl2main.dylib)
  target_link_libraries(app optimized sdl2main.dylib)

  target_link_libraries(app debug sdl2.dylib)
  target_link_libraries(app optimized sdl2.dylib)

  target_link_libraries(app debug "-framework OpenCL")
  target_link_libraries(app optimized "-framework OpenCL")
ELSE()
ENDIF()
