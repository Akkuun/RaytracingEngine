cmake_minimum_required(VERSION 3.10)
project(RaytracingEngine)

# Options de build
option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(ENABLE_WARNINGS "Enable extra compiler warnings" ON)
set(DEFAULT_BUILD_TYPE "Debug" CACHE STRING "Default build type")
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE ${DEFAULT_BUILD_TYPE} CACHE STRING "Build type" FORCE)
endif()

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compilation flags
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
endif()

if(ENABLE_WARNINGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
endif()

if(ENABLE_ASAN AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
endif()

# Find OpenCL
find_package(OpenCL REQUIRED)

find_path(OpenCL_HPP_INCLUDE_DIR CL/opencl.hpp
    HINTS
    ${OpenCL_INCLUDE_DIRS}
    /usr/include
    /usr/local/include
    /opt/local/include)

# Sources
set(SOURCES main.cpp)

# Executable
add_executable(raytrace ${SOURCES})

# Includes
target_include_directories(raytrace PRIVATE
    ${OpenCL_INCLUDE_DIRS}
    ${OpenCL_HPP_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/external/OpenCL-CLHPP/include
)

# Link
target_link_libraries(raytrace PRIVATE ${OpenCL_LIBRARIES})

# Copy kernel
configure_file(${CMAKE_SOURCE_DIR}/hello.cl ${CMAKE_BINARY_DIR}/hello.cl COPYONLY)

# Print summary
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "OpenCL include: ${OpenCL_INCLUDE_DIRS}")
message(STATUS "OpenCL HPP include: ${OpenCL_HPP_INCLUDE_DIR}")
message(STATUS "OpenCL libs: ${OpenCL_LIBRARIES}")