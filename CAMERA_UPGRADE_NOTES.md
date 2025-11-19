# Camera Upgrade - Migration from newcamera/ to camera/

## Overview
The camera system in `src/core/camera/` has been upgraded to match the sophisticated features from `src/core/newcamera/`, adapted to work with Qt instead of GLFW for input management.

## Key Changes

### 1. Updated Camera.h
- **Replaced custom vec3 with glm::vec3**: Now uses GLM library for vector mathematics
- **Added quaternion support**: Camera rotation is now handled with `glm::quat` for smoother interpolation
- **Euler angles**: Added `m_eulerAngle` for pitch/yaw/roll rotation representation
- **Advanced features**:
  - Reset animation with smooth interpolation
  - Attached/detached camera modes (third-person view support)
  - FOV animation during sprint
  - Sneak animation with vertical offset
  - Multiple control modes (mouse/keyboard)
  
### 2. Created Camera_Helper
New files: `Camera_Helper.h` and `Camera_Helper.cpp`
- **Interpolation functions**: Support for LINEAR, COS, EXP, LOG, SQUARE, SQRT, CUBIC, SQRT3
- **Angle utilities**: `clipAnglePI()` for angle normalization, `clamp()` for value constraints
- **Matrix computation**: `computeFinalView()` generates view and projection matrices
- **Quaternion conversion**: `quatToEuler()` for converting between representations

### 3. Updated Camera.cpp
- **Qt input handling**: Replaced GLFW input with Qt event system
- **Key state tracking**: Maintains array of pressed keys for continuous input
- **Mouse control**: Supports both mouse look and keyboard rotation
- **Animation systems**:
  - Sprint FOV increase for speed feeling
  - Sneak camera lowering for crouch effect
  - Smooth reset animation back to defaults
  
### 4. Updated Command Classes
- `CameraMoveCommand.h`: Now uses `glm::vec3` instead of custom `vec3`
- `CameraRotationCommand.h`: Now uses `glm::vec3` and `getRotationEuler()` method

### 5. Updated RenderWidget
Added input event handlers to forward user input to the camera:
- `keyPressEvent()` / `keyReleaseEvent()`: Forward keyboard input
- `mouseMoveEvent()`: Handle mouse look for camera rotation
- `mousePressEvent()`: Enable mouse control with right-click
- `wheelEvent()`: Handle scroll wheel for distance adjustment
- **Camera update loop**: Calls `Camera::getInstance().update(deltaTime)` each frame

## Control Scheme

### Keyboard Controls
- **F**: Toggle between mouse control (mode 0) and keyboard control (mode 1)
- **F5**: Toggle between first-person and third-person (attached) camera
- **R**: Reset camera to default position/rotation
- **Arrow Keys** (in keyboard mode): Rotate camera
  - Up/Down: Pitch
  - Left/Right: Yaw

### Mouse Controls
- **Right Mouse Button + Drag**: Rotate camera (in any mode)
- **Mouse Movement** (in FPS mode): Free look
- **Mouse Wheel**: Adjust camera distance (in attached/third-person mode)

## GPU Camera Structure
The `toGPU()` method has been updated to:
- Convert from `glm::vec3` to `GPUVec3` with proper padding
- Calculate target point from position and rotation quaternion
- Maintain compatibility with OpenCL kernel expectations

## Integration Notes

### Using the Camera
```cpp
Camera& camera = Camera::getInstance();

// Update each frame
camera.update(deltaTime);

// Get matrices for rendering
glm::mat4 view = camera.getViewMatrix();
glm::mat4 proj = camera.getProjectionMatrix();

// Get camera state
glm::vec3 pos = camera.getPosition();
glm::vec3 front = camera.getFront();
float fov = camera.getFOV();

// For GPU rendering
GPUCamera gpuCam = camera.toGPU();
```

### Input Forwarding (already implemented in RenderWidget)
```cpp
void YourWidget::keyPressEvent(QKeyEvent *event) {
    Camera::getInstance().handleKeyPress(event->key(), true);
}

void YourWidget::mouseMoveEvent(QMouseEvent *event) {
    float deltaX = /* calculate delta */;
    float deltaY = /* calculate delta */;
    Camera::getInstance().handleMouseMove(deltaX, deltaY);
}
```

## Constants (defined in Camera.h)
- `DEFAULT_FOV`: 70.0°
- `DEFAULT_POSITION`: (0, 0, 40)
- `DEFAULT_ROTATION_SPEED`: 0.1
- `DEFAULT_TRANSLATION_SPEED`: 0.05
- `DELTA_Y_SNEAK`: 0.3 (camera lowering during sneak)

## Benefits
1. **Smoother rotation**: Quaternion-based rotation eliminates gimbal lock
2. **Better UX**: Interpolated animations for FOV and position changes
3. **Qt integration**: Native Qt event handling instead of GLFW
4. **Flexible control**: Multiple control modes (mouse/keyboard)
5. **Advanced features**: Sprint FOV, sneak animation, smooth reset
6. **Maintained compatibility**: Command pattern still works, GPU structure unchanged

## Build Status
✅ Successfully compiled with no errors
✅ All command classes updated
✅ Input handling integrated into RenderWidget
