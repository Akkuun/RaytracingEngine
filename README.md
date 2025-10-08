# RaytracingEngine OpenCL Example

## Prerequisites
- Linux system (tested on Arch Linux)
- CMake >= 3.10
- OpenCL runtime and development headers (e.g. `opencl-headers`, `opencl-clhpp`, `ocl-icd`)
- C++17 compatible compiler (e.g. g++, clang++)
- Git

## Setup Instructions

### 1. Clone the repository
```bash
git clone <your-repo-url>
cd RaytracingEngine
```

### 2. Install OpenCL C++ Bindings (CLHPP)
If your system does not provide `CL/opencl.hpp`, you need to install the OpenCL C++ bindings:

#### Option A: System package (recommended if available)
- On Arch Linux:
  ```bash
  sudo pacman -S opencl-headers opencl-clhpp
  ```
- On Ubuntu:
  ```bash
  sudo apt install opencl-headers
  # For CLHPP, download from KhronosGroup if not available
  ```

#### Option B: Submodule/manual install
```bash
git submodule update --init --recursive
cd external/OpenCL-CLHPP
mkdir build && cd build
cmake -DBUILD_DOCS=OFF -DBUILD_EXAMPLES=OFF -DBUILD_TESTING=OFF ..
sudo make install
```

### 3. Build and Run the Project
From the project root:
```bash
bash build.sh
```
This will:
- Create the build directory
- Run CMake and make
- Copy the OpenCL kernel file
- Launch the executable

## Project Structure
- `main.cpp`: Host C++ code
- `hello.cl`: OpenCL kernel (simple sum)
- `CMakeLists.txt`: Build configuration
- `build.sh`: Build and run script
- `external/OpenCL-CLHPP`: OpenCL C++ bindings (if using submodule)

## Troubleshooting
- If you get errors about missing `CL/opencl.hpp`, ensure CLHPP is installed and included in your CMake configuration.
- If you update the submodule, you may need to re-run the CLHPP build/install step.
- For OpenCL runtime issues, check your GPU drivers and OpenCL ICD loader.

## License
See LICENSE.txt for details.
