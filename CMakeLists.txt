cmake_minimum_required(VERSION 3.14)

project(customengine
	LANGUAGES C CXX
	)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include(Policies)
include(ProjectFuncs)

# required system dependencies
find_package(Threads REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)

# default linker
if(NOT CUSTENGINE_LINKER AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CUSTENGINE_LINKER "lld")
elseif(NOT CUSTENGINE_LINKER)
    set(CUSTENGINE_LINKER "bfd")
endif()

custengine_set_alternate_linker()

add_subdirectory(third_party/glad)

add_subdirectory(src/core)
add_subdirectory(src/main)

