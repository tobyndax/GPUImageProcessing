add_custom_target("sdl2")

IF(WIN32) #If windows
		message(STATUS "Windows x64")

		add_custom_command(TARGET sdl2 POST_BUILD												# Add a post-build event
	    COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_SOURCE_DIR}/thirdparty/sdl2-2.0.5/lib/${CMAKE_VS_PLATFORM_NAME}/sdl2.dll"  	# input
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}$<CONFIGURATION>"/sdl2.dll")							# output

		add_custom_command(TARGET sdl2 POST_BUILD												# Add a post-build event
	    COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_SOURCE_DIR}/thirdparty/sdl2-2.0.5/lib/${CMAKE_VS_PLATFORM_NAME}/sdl2.lib"  	# input
        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}$<CONFIGURATION>"/sdl2.lib")							# output

		add_custom_command(TARGET sdl2 POST_BUILD												# Add a post-build event
	    COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_SOURCE_DIR}/thirdparty/sdl2-2.0.5/lib/${CMAKE_VS_PLATFORM_NAME}/sdl2main.lib"	# input
        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}$<CONFIGURATION>"/sdl2main.lib")						# output

ELSE()
	message(STATUS "Non windows platform")
ENDIF()
