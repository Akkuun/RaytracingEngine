#pragma once

#include <QKeySequence>
#include <map>
#include <string>

#define KB_MOVE_FORWARD       "moveForward"
#define KB_MOVE_BACKWARD      "moveBackward"
#define KB_MOVE_LEFT          "moveLeft"
#define KB_MOVE_RIGHT         "moveRight"
#define KB_MOVE_UP            "moveUp"
#define KB_MOVE_DOWN          "moveDown"
#define KB_CAMERA_LEFT        "cameraLeft"
#define KB_CAMERA_RIGHT       "cameraRight"
#define KB_CAMERA_UP          "cameraUp"
#define KB_CAMERA_DOWN        "cameraDown"
#define KB_RESET_CAMERA       "resetCamera"

#define KB_TOGGLE_PANELS       "togglePanels"
#define KB_UNDO              "undo"
#define KB_REDO              "redo"

#define KB_SAVE             "save"

#define KB_APPLY_ON_ALL_AXIS "applyOnAllAxis"

#define KB_REBIND_SUCCESS          0
#define KB_ERROR_ACTION_NOT_FOUND   -1
#define KB_ERROR_KEYBIND_IN_USE     -2

/**
 * @brief Keybinds singleton - centralized management of all application keybinds
 * 
 * Uses QKeySequence for Qt-native key combination handling.
 * Supports single keys and combinations (Ctrl+Z, Ctrl+Shift+Z, etc.)
 * 
 * This class only manages keybind definitions. Actual key event handling
 * is done through Qt's standard event system (keyPressEvent, QShortcut, etc.)
 */
class Keybinds {
private:
    Keybinds() {
        // Camera movement keybinds (single keys)
        keybindsMap[KB_MOVE_FORWARD] = QKeySequence(Qt::Key_Z);
        keybindsMap[KB_MOVE_BACKWARD] = QKeySequence(Qt::Key_S);
        keybindsMap[KB_MOVE_LEFT] = QKeySequence(Qt::Key_D);
        keybindsMap[KB_MOVE_RIGHT] = QKeySequence(Qt::Key_Q);
        keybindsMap[KB_MOVE_UP] = QKeySequence(Qt::Key_Space);
        keybindsMap[KB_MOVE_DOWN] = QKeySequence(Qt::Key_C);
        
        // Camera rotation keybinds (keyboard mode)
        keybindsMap[KB_CAMERA_LEFT] = QKeySequence(Qt::Key_Left);
        keybindsMap[KB_CAMERA_RIGHT] = QKeySequence(Qt::Key_Right);
        keybindsMap[KB_CAMERA_UP] = QKeySequence(Qt::Key_Up);
        keybindsMap[KB_CAMERA_DOWN] = QKeySequence(Qt::Key_Down);
        keybindsMap[KB_RESET_CAMERA] = QKeySequence(Qt::Key_R);

        // UI keybinds
        keybindsMap[KB_TOGGLE_PANELS] = QKeySequence(Qt::Key_F);
        keybindsMap[KB_APPLY_ON_ALL_AXIS] = QKeySequence(Qt::CTRL);

        // Undo/Redo with standard shortcuts
        keybindsMap[KB_UNDO] = QKeySequence(Qt::CTRL | Qt::Key_Z);
        keybindsMap[KB_REDO] = QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Z);
        
        // Save
        keybindsMap[KB_SAVE] = QKeySequence(Qt::CTRL | Qt::Key_S);
    
    }
    Keybinds(const Keybinds&) = delete;
    Keybinds& operator=(const Keybinds&) = delete;

public:
    // Singleton instance accessor
    static Keybinds& getInstance() {
        static Keybinds instance;
        return instance;
    }

    std::map<std::string, QKeySequence> keybindsMap;
    
    /**
     * @brief Get all keybinds
     * @return Map of action names to QKeySequence
     */
    const std::map<std::string, QKeySequence>& getAllKeybinds() const {
        return keybindsMap;
    }

    /**
     * @brief Get keybind for a specific action
     * @param action Action name (use KB_* defines)
     * @return QKeySequence for the action, or empty sequence if not found
     */
    QKeySequence getKeybind(const std::string& action) const {
        auto it = keybindsMap.find(action);
        if (it != keybindsMap.end()) {
            return it->second;
        }
        return QKeySequence();
    }

    /**
     * @brief Change a keybind
     * @param action Action name (use KB_* defines)
     * @param newKeybind New QKeySequence
     * @return KB_REBIND_SUCCESS, KB_ERROR_ACTION_NOT_FOUND, or KB_ERROR_KEYBIND_IN_USE
     */
    int changeKeybind(const std::string& action, const QKeySequence& newKeybind) {
        auto it = keybindsMap.find(action);
        if (it == keybindsMap.end()) {
            return KB_ERROR_ACTION_NOT_FOUND;
        }
        
        // Check for conflicts
        for (const auto& pair : keybindsMap) {
            if (pair.first != action && pair.second == newKeybind) {
                return KB_ERROR_KEYBIND_IN_USE;
            }
        }
        
        keybindsMap[action] = newKeybind;
        return KB_REBIND_SUCCESS;
    }
    
    /**
     * @brief Check if a key event matches a keybind
     * @param action Action name (use KB_* defines)
     * @param key Qt::Key code from key event
     * @param modifiers Qt::KeyboardModifiers from key event
     * @return true if the key+modifiers match the keybind
     */
    bool matches(const std::string& action, int key, Qt::KeyboardModifiers modifiers) const {
        QKeySequence sequence = getKeybind(action);
        if (sequence.isEmpty()) return false;
        
        // Create a QKeySequence from the current key+modifiers
        int combined = key | modifiers;
        QKeySequence currentSeq(combined);
        
        return sequence.matches(currentSeq) == QKeySequence::ExactMatch;
    }
};