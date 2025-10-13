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

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Widgets)

find_path(OpenCL_HPP_INCLUDE_DIR CL/opencl.hpp
    HINTS
    ${OpenCL_INCLUDE_DIRS}
    /usr/include
    /usr/local/include
    /opt/local/include)

# Qt6 MOC
qt6_wrap_cpp(MOC_SOURCES 
    ui/MainWindow.h
    ui/CollapsiblePanel.h
    ui/RenderWidget.h
    panels/CameraPanel.h
    panels/ObjectPanel.h
    panels/ParametersPanel.h
    panels/ScenePanel.h
)

# Sources
set(SOURCES
    main.cpp
    ui/MainWindow.cpp
    ui/CollapsiblePanel.cpp
    ui/RenderWidget.cpp
    panels/CameraPanel.cpp
    panels/ObjectPanel.cpp
    panels/ParametersPanel.cpp
    panels/ScenePanel.cpp
    ${MOC_SOURCES}
)

# Executable
add_executable(raytrace ${SOURCES})

# Includes
target_include_directories(raytrace PRIVATE
    ${OpenCL_INCLUDE_DIRS}
    ${OpenCL_HPP_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/external/OpenCL-CLHPP/include
)

# Link
target_link_libraries(raytrace PRIVATE ${OpenCL_LIBRARIES} Qt6::Widgets)

# Copy kernels to output directory
file(GLOB KERNEL_FILES "${CMAKE_SOURCE_DIR}/kernel/*.cl")
foreach(KERNEL_FILE ${KERNEL_FILES})
    get_filename_component(KERNEL_NAME ${KERNEL_FILE} NAME)
    configure_file(${KERNEL_FILE} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${KERNEL_NAME} COPYONLY)
endforeach()

# Also copy to build root for backwards compatibility
configure_file(${CMAKE_SOURCE_DIR}/kernel/hello.cl ${CMAKE_BINARY_DIR}/hello.cl COPYONLY)

# Print summary
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "OpenCL include: ${OpenCL_INCLUDE_DIRS}")
message(STATUS "OpenCL HPP include: ${OpenCL_HPP_INCLUDE_DIR}")
message(STATUS "OpenCL libs: ${OpenCL_LIBRARIES}")