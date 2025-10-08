cmake_minimum_required(VERSION 3.10)
project(RaytracingEngine)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable debug symbols
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g")

# Find OpenCL package
find_package(OpenCL REQUIRED)

find_path(OpenCL_HPP_INCLUDE_DIR CL/opencl.hpp
      HINTS
      ${OpenCL_INCLUDE_DIRS}
      /usr/include
      /usr/local/include
      /opt/local/include)

   include_directories(${OpenCL_INCLUDE_DIRS} ${OpenCL_HPP_INCLUDE_DIR})

# Add OpenCL C++ bindings path - adjust this path if needed
include_directories(${OpenCL_INCLUDE_DIRS})
# If you downloaded OpenCL-CLHPP, add its include path:
include_directories(external/OpenCL-CLHPP/include)

# Add executable
add_executable(raytrace main.cpp)

# Link OpenCL
target_link_libraries(raytrace ${OpenCL_LIBRARIES})

# Copy OpenCL kernel to build directory
configure_file(${CMAKE_SOURCE_DIR}/hello.cl ${CMAKE_BINARY_DIR}/hello.cl COPYONLY)