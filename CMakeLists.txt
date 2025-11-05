cmake_minimum_required(VERSION 3.16)
project(RaytracingEngine VERSION 1.0.0 LANGUAGES CXX)

# Prevent in-source builds
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are not allowed. Please create a separate build directory.")
endif()

# Options de build
option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(ENABLE_WARNINGS "Enable extra compiler warnings" ON)
option(BUILD_TESTS "Build unit tests" OFF)
set(DEFAULT_BUILD_TYPE "Debug" CACHE STRING "Default build type")

# Set build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE ${DEFAULT_BUILD_TYPE} CACHE STRING "Build type" FORCE)
endif()

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

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

# Find OpenMP
find_package(OpenMP REQUIRED)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Define OpenCL version to avoid warnings
add_definitions(-DCL_HPP_TARGET_OPENCL_VERSION=300)
add_definitions(-DCL_HPP_MINIMUM_OPENCL_VERSION=120)

find_path(OpenCL_HPP_INCLUDE_DIR CL/opencl.hpp
    HINTS
    ${OpenCL_INCLUDE_DIRS}
    /usr/include
    /usr/local/include
    /opt/local/include)

# Find only Qt headers that need MOC processing (contain Q_OBJECT or Q_GADGET)
file(GLOB UI_HEADERS "src/ui/*.h" "src/ui/panels/*.h")

# Qt6 MOC - process only Qt headers
qt6_wrap_cpp(MOC_SOURCES ${UI_HEADERS})

# Automatically find all source files (excluding external directory)
file(GLOB SOURCES "*.cpp")
file(GLOB_RECURSE UI_SOURCES "ui/*.cpp")
file(GLOB_RECURSE PANEL_SOURCES "panels/*.cpp")
file(GLOB_RECURSE SRC_SOURCES "src/*.cpp")
# Combine all sources
list(APPEND SOURCES ${UI_SOURCES} ${PANEL_SOURCES} ${SRC_SOURCES} ${MOC_SOURCES})

# Executable
add_executable(raytrace ${SOURCES})

# Includes
target_include_directories(raytrace PRIVATE
    ${OpenCL_INCLUDE_DIRS}
    ${OpenCL_HPP_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/external/OpenCL-CLHPP/include
)

# Link
target_link_libraries(raytrace PRIVATE ${OpenCL_LIBRARIES} Qt6::Widgets OpenMP::OpenMP_CXX)

# Copy kernels to output directory
file(GLOB KERNEL_FILES "${CMAKE_SOURCE_DIR}/kernels/*.cl")
foreach(KERNEL_FILE ${KERNEL_FILES})
    get_filename_component(KERNEL_NAME ${KERNEL_FILE} NAME)
    configure_file(${KERNEL_FILE} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KERNEL_NAME} COPYONLY)
    # Also copy to build root for backwards compatibility
    configure_file(${KERNEL_FILE} ${CMAKE_BINARY_DIR}/${KERNEL_NAME} COPYONLY)
endforeach()
# Print summary
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "OpenCL include: ${OpenCL_INCLUDE_DIRS}")
message(STATUS "OpenCL HPP include: ${OpenCL_HPP_INCLUDE_DIR}")
message(STATUS "OpenCL libs: ${OpenCL_LIBRARIES}")