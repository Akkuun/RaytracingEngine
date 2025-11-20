#pragma once

#include <map>
#include <vector>

// Key states
#define KEY_PRESSED    1
#define KEY_HELD       2
#define KEY_RELEASED   3
#define KEY_UNPRESSED  4
#define KEY_UNDEFINED  5

/**
 * @brief KeyInput manages the state of keyboard keys for a single context
 * 
 * This class tracks key states (pressed, held, released, unpressed) for a set of monitored keys.
 * It supports key combinations (keybinds) up to 3 keys.
 * 
 * Usage:
 * 1. Create a KeyInput instance with a list of keys to monitor
 * 2. Call setKeyStatus() when Qt key events occur (from keyPressEvent/keyReleaseEvent)
 * 3. Call update() once per frame to transition key states
 * 4. Use isKeyPressed/Held/Released or isKeybindPressed/Held/Released to query states
 */
class KeyInput
{
public:
    /**
     * @brief Construct a new KeyInput object
     * @param keysToMonitor List of Qt::Key codes to track
     */
    KeyInput(std::vector<int> keysToMonitor);
    ~KeyInput();

    // Single key queries
    /**
     * @brief Returns true if the key is currently held down
     * @param key Qt::Key code
     */
    bool isKeyHeld(int key);
    
    /**
     * @brief Returns true if the key was just released this frame
     * @param key Qt::Key code
     */
    bool isKeyReleased(int key);
    
    /**
     * @brief Returns true if the key was just pressed this frame
     * @param key Qt::Key code
     */
    bool isKeyPressed(int key);

    // Keybind (combination) queries
    /**
     * @brief Returns true if all keys in the keybind are currently held
     * @param keybind Vector of Qt::Key codes (up to 3 keys)
     */
    bool isKeybindHeld(const std::vector<int>& keybind);
    
    /**
     * @brief Returns true if all keys in the keybind are held and at least one was just pressed
     * @param keybind Vector of Qt::Key codes (up to 3 keys)
     */
    bool isKeybindPressed(const std::vector<int>& keybind);
    
    /**
     * @brief Returns true if all keys in the keybind were released and at least one was just released
     * @param keybind Vector of Qt::Key codes (up to 3 keys)
     */
    bool isKeybindReleased(const std::vector<int>& keybind);
    
    /**
     * @brief Returns the current state of a key
     * @return KEY_PRESSED, KEY_HELD, KEY_RELEASED, KEY_UNPRESSED, or KEY_UNDEFINED
     */
    int getKeyStatus(int key);

    /**
     * @brief Get if this KeyInput is enabled
     */
    bool getIsEnabled() const { return _isEnabled; }
    
    /**
     * @brief Enable or disable this KeyInput
     * When disabled, all queries return false
     */
    void setIsEnabled(bool value) { _isEnabled = value; }
    
    /**
     * @brief Updates the state of all keys (transitions PRESSED->HELD, RELEASED->UNPRESSED)
     * Call this once per frame
     */
    void update();

    /**
     * @brief Set the status of a key (called from Qt key events)
     * @param key Qt::Key code
     * @param pressed true for press, false for release
     */
    void setKeyStatus(int key, bool pressed);

private:
    // Map from monitored keys to their pressed states
    std::map<int, char> _keys;
    // If disabled, KeyInput.getKeyXXX() will always return false
    bool _isEnabled;
};
