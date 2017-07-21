include_directories(${CMAKE_SOURCE_DIR}/../include)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/OpenCL1.2/)

link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

file (GLOB SRC_FILES "*.cpp")
file (GLOB HDR_FILES "*.h")

message(STATUS ${CMAKE_CXX_STANDARD})

add_library(core SHARED ${SRC_FILES} ${HDR_FILES})

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  message(STATUS "Adding framework OpenCL")

  target_link_libraries(core debug "-framework OpenCL")
  target_link_libraries(core optimized "-framework OpenCL")
ELSEIF(MSVC)

ENDIF()

add_definitions(-DEXPMODULE)
