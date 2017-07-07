include_directories(${CMAKE_SOURCE_DIR}/../include)

link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

file (GLOB SRC_FILES "*.cpp")  
file (GLOB HDR_FILES "*.h")

add_library(core STATIC ${SRC_FILES} ${HDR_FILES})

add_definitions(-DEXPMODULE)