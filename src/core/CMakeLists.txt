include_directories(${CMAKE_SOURCE_DIR}/../include)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/OpenCL1.2/)

link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

file (GLOB SRC_FILES "*.cpp")
file (GLOB HDR_FILES "*.h")

message(STATUS ${CMAKE_CXX_STANDARD})

add_library(core SHARED ${SRC_FILES} ${HDR_FILES})


IF(WIN32) #If windows
		add_custom_command(TARGET core PRE_BUILD										# Add a post-build event
	    COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_SOURCE_DIR}/core/vectorAdd.cl"									  		# input
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}$<CONFIGURATION>"/vectorAdd.cl")					# output

		add_custom_command(TARGET core PRE_BUILD										# Add a post-build event
	    COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_SOURCE_DIR}/core/lowPassKernel.cl"									  		# input
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}$<CONFIGURATION>"/lowPassKernel.cl")					# output

ELSE(WIN32)
		add_custom_command(TARGET core PRE_BUILD											# Add a post-build event
	    COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_SOURCE_DIR}/core/vectorAdd.cl"									  		# input
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"vectorAdd.cl")									# output
		
		add_custom_command(TARGET core PRE_BUILD										# Add a post-build event
	    COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_SOURCE_DIR}/core/lowPassKernel.cl"									  		# input
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}$<CONFIGURATION>"/lowPassKernel.cl")					# output
ENDIF(WIN32)


IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  message(STATUS "Adding framework OpenCL")

  target_link_libraries(core debug "-framework OpenCL")
  target_link_libraries(core optimized "-framework OpenCL")
ELSEIF(MSVC)

  find_package(OpenCL REQUIRED)
  include_directories(${OpenCL_INCLUDE_DIRS})
  link_directories(${OpenCL_LIBRARY})

  target_link_libraries(core debug ${OpenCL_LIBRARY})
  target_link_libraries(core optimized ${OpenCL_LIBRARY})

ENDIF()

add_definitions(-DEXPMODULE)
