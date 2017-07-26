cmake_minimum_required(VERSION 3.5.0) #TODO Figure out what the minimum required version actually is.
project(GPUImageProcessing)

IF(MSVC) #If windows (better to check MSVC for the MSVC flags)
  message(STATUS "Setting MSVC flags")

  #Normal exception handling
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHc")
ENDIF(MSVC)

#Set c++11 as standard
set(CMAKE_CXX_STANDARD 11)
IF(MSVC AND ${CMAKE_CONFIGURATION_TYPES} STREQUAL "Release")
   message(STATUS ${CMAKE_CONFIGURATION_TYPES} " Enabling optimization")
	
   ADD_DEFINITIONS(/arch:AVX2)
   ADD_DEFINITIONS(/fp:fast)
   ADD_DEFINITIONS(/O2)
   ADD_DEFINITIONS(/FA)
ENDIF(MSVC AND ${CMAKE_CONFIGURATION_TYPES} STREQUAL "Release")
#Useful output directories.
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../lib/")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../lib/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../bin/")

set(BUILD_SHARED_LIBS TRUE)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

#Add core and app subdirs
add_subdirectory(core)
add_subdirectory(app)
add_subdirectory(thirdparty/sdl2-2.0.5)

#App is dependent on core
add_dependencies(app core)
add_dependencies(app sdl2)

#Set startup project for VS
IF(MSVC)
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT app)
ENDIF()
