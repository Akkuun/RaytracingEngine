# RaytracingEngine OpenCL Example

## Prerequisites
- Linux system (tested on Arch Linux)
- CMake >= 3.10
- OpenCL runtime and development headers (e.g. `opencl-headers`, `opencl-clhpp`, `ocl-icd`)
- Qt6 (Widgets) development libraries (e.g. `qt6-base`, `qt6-tools`, `qt6-qtbase-devel`)
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

### 2b. Install Qt6 (required for GUI)
- On Arch Linux:
  ```bash
  sudo pacman -S qt6-base qt6-tools
  ```
- On Ubuntu:
  ```bash
  sudo apt install qt6-base-dev qt6-tools-dev-tools
  ```

### 3. Install dependencies depending on your graphics hardware (NVIDIA or AMD)

```bash
# For NVIDIA GPUs (Ubuntu/Debian)
sudo apt install nvidia-opencl-icd
# Or on Arch Linux
sudo pacman -S opencl-nvidia

# For AMD GPUs (Ubuntu/Debian)
sudo apt install ocl-icd-libopencl1 mesa-opencl-icd
# Or on Arch Linux
sudo pacman -S opencl-mesa
```

If you use Intel integrated graphics, you may need:
```bash
# Intel (Ubuntu/Debian)
sudo apt install intel-opencl-icd
# Or on Arch Linux
sudo pacman -S intel-compute-runtime
```

### 4. Build and Run the Project
From the project root:
```bash
bash build.sh
```
This will:
- Create the build directory
- Run CMake and make
- Copy the OpenCL kernel file
- Launch the executable
- Afficher une interface graphique Qt6 avec un bouton pour lancer le calcul OpenCL et afficher le résultat

## Project Structure
- `main.cpp`: Host C++ code, lance l'interface Qt6
- `mainwindow.cpp`/`mainwindow.h`: Interface graphique Qt6, bouton et affichage du résultat
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


## Shapes Size

CPU (C++)              vs    GPU (OpenCL)        ✅
---------------------------------------------------
Vec3: 16 bytes         vs    float3: 16 bytes    ✅
GPUSphere: 64 bytes    vs    Sphere: 64 bytes    ✅
GPUSquare: 80 bytes    vs    Square: 80 bytes    ✅
GPUShape: 96 bytes     vs    Shape: 96 bytes     ✅