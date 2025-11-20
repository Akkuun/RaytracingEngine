# Camera Commands System

## Overview

The raytracing engine now uses the **Command Pattern** for discrete camera actions, enabling undo/redo functionality while maintaining direct control for continuous movement.

## Architecture Decision

### Continuous Actions (Direct Manipulation)
- **WASD Movement** (KB_MOVE_FORWARD, KB_MOVE_BACKWARD, KB_MOVE_LEFT, KB_MOVE_RIGHT)
- **Space/Shift** (KB_MOVE_UP, KB_MOVE_DOWN)
- **Arrow Key Rotation** (KB_CAMERA_LEFT, KB_CAMERA_RIGHT, KB_CAMERA_UP, KB_CAMERA_DOWN)

These actions happen **every frame** in `Camera::update()` and don't create commands because:
- Creating a command per frame would flood the undo stack
- User expects immediate, fluid response
- No meaningful "undo" for gradual movement

### Discrete Actions (Command Pattern)
- **Camera Reset** (KB_RESET_CAMERA - R key) → `CameraResetCommand`
- **Position Changes** (from UI panels) → `CameraMoveCommand`
- **Rotation Changes** (from UI panels) → `CameraRotationCommand`
- **FOV Changes** (from UI panels) → `CameraFOVCommand`

These actions are **one-time state changes** and:
- Create undoable history
- Go through `CommandsManager`
- Enable Ctrl+Z / Ctrl+Shift+Z to undo/redo

## Implementation

### Memory Management Note

The `CommandsManager::executeCommand()` takes a **raw pointer** `ICommand*` and immediately wraps it in a `std::unique_ptr` for ownership. This means:

```cpp
// ✅ Correct usage - raw pointer
commandManager.executeCommand(new CameraResetCommand());

// ❌ Wrong - don't use std::unique_ptr or std::move
commandManager.executeCommand(std::make_unique<CameraResetCommand>());
```

The CommandsManager takes ownership and handles memory cleanup automatically through its internal `std::stack<std::unique_ptr<ICommand>>`.

### Camera Reset Command

**File**: `/src/core/commands/actionsCommands/CameraResetCommand.h`

```cpp
class CameraResetCommand : public ICommand {
public:
    // For keybind shortcuts (uses singleton)
    CameraResetCommand();
    
    // For UI panels (takes reference)
    CameraResetCommand(Camera& cam);
    
    // Custom target state
    CameraResetCommand(glm::vec3 pos, glm::vec3 rot, float fov);
    CameraResetCommand(Camera& cam, glm::vec3 pos, glm::vec3 rot, float fov);
    
    void execute() override;  // Apply target state
    void undo() override;     // Restore previous state
    int getID() const override;
};
```

### MainWindow Integration

**File**: `/src/ui/MainWindow.cpp`

```cpp
// In constructor - setup shortcut
resetCameraShortcut = new QShortcut(keybinds.getKeybind(KB_RESET_CAMERA), this);
connect(resetCameraShortcut, &QShortcut::activated, this, &MainWindow::onResetCamera);

// Slot implementation
void MainWindow::onResetCamera()
{
    // Create and execute camera reset command (allows undo)
    // CommandsManager takes ownership of the raw pointer
    commandManager.executeCommand(new CameraResetCommand());
}
```

### Keybind Assignment

**File**: `/src/core/input/Keybinds.hpp`

```cpp
keybindsMap[KB_RESET_CAMERA] = QKeySequence(Qt::Key_R);
```

Press `R` to reset camera to default position/rotation/FOV. Press `Ctrl+Z` to undo the reset.

## Usage Examples

### From Keybind (Shortcut)
```cpp
// User presses R key
// → MainWindow::onResetCamera() called
// → Creates CameraResetCommand()
// → CommandsManager::executeCommand()
// → Camera reset with undo capability
```

### From UI Panel

**File**: `/src/ui/panels/CameraPanel.cpp`

```cpp
// Reset button with keybind tooltip
QPushButton *resetButton = new QPushButton("Reset Camera");
Keybinds& keybinds = Keybinds::getInstance();
resetButton->setToolTip(QString("Reset camera to default position (%1)")
    .arg(keybinds.getKeybind(KB_RESET_CAMERA).toString()));
    
connect(resetButton, &QPushButton::clicked, [&camera, &commandManager](){
    commandManager.executeCommand(new CameraResetCommand(camera));
});
```

The CameraPanel now includes a "Reset Camera" button that:
- Shows the keybind shortcut in its tooltip ("Reset camera to default position (R)")
- Uses the same command as the shortcut (ensures consistency)
- Provides a GUI alternative for users who prefer clicking

### Custom Target State
```cpp
// Save current camera state as bookmark
glm::vec3 bookmarkPos = camera.getPosition();
glm::vec3 bookmarkRot = camera.getRotationEuler();
float bookmarkFOV = camera.getFOV();

// Later: restore bookmark (undoable)
commandManager.executeCommand(
    new CameraResetCommand(bookmarkPos, bookmarkRot, bookmarkFOV)
);
```

## Benefits

1. **Undo/Redo Support**: Discrete camera actions can be undone (Ctrl+Z)
2. **Clean Separation**: Continuous movement stays direct, discrete actions use commands
3. **Consistency**: Follows same pattern as other commands (CameraMoveCommand, etc.)
4. **Flexibility**: Both singleton and reference-based constructors available
5. **User Experience**: No command spam from movement, meaningful undo for intentional actions

## Future Extensions

Other camera actions that could use commands:
- **Toggle Camera Mode** (FPS/Orbital)
- **Toggle Attached Mode**
- **Camera Bookmarks** (save/restore named positions)
- **Camera Animations** (record/replay camera paths)

Each would follow the same pattern: QShortcut → Slot → Command → CommandsManager.

## Testing

1. **Camera Reset**:
   - Press `R` → Camera resets to default
   - Press `Ctrl+Z` → Camera returns to previous state
   - Press `Ctrl+Shift+Z` → Camera resets again

2. **Continuous Movement**:
   - Press `W` → Camera moves forward smoothly
   - Release → Movement stops
   - Press `Ctrl+Z` → Does NOT undo movement (correct behavior)

3. **Combined Actions**:
   - Move with WASD
   - Press `R` to reset
   - Press `Ctrl+Z` → Only the reset is undone, not the movement
