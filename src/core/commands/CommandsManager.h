#pragma once
#include "ICommand.h"
#include <stack>
#include <memory>
#include <functional>
#include <vector>

class CommandsManager {
private:
    std::stack<std::unique_ptr<ICommand>> undoStack;
    std::stack<std::unique_ptr<ICommand>> redoStack;
    
    // Multiple callbacks for notifying listeners (UI) about scene changes
    std::vector<std::function<void()>> sceneChangedCallbacks;
    std::vector<std::function<void()>> cameraChangedCallbacks;
    std::vector<std::function<void()>> shapesChangedCallbacks;
    std::vector<std::function<void()>> materialChangedCallbacks;
    std::vector<std::function<void()>> bvhChangedCallbacks;
    
    CommandsManager() = default;

public:
    static CommandsManager& getInstance() {
        static CommandsManager instance;
        return instance;
    }

    CommandsManager(const CommandsManager&) = delete;
    CommandsManager& operator=(const CommandsManager&) = delete;

    // Register callback for scene changes (called after execute/undo/redo)
    void addSceneChangedCallback(std::function<void()> callback) {
        sceneChangedCallbacks.push_back(callback);
    }

    void addCameraChangedCallback(std::function<void()> callback) {
        cameraChangedCallbacks.push_back(callback);
    }

    void addShapesChangedCallback(std::function<void()> callback) {
        shapesChangedCallbacks.push_back(callback);
    }

    void addMaterialChangedCallback(std::function<void()> callback) {
        materialChangedCallbacks.push_back(callback);
    }
    
    // Legacy method for backward compatibility
    void setSceneChangedCallback(std::function<void()> callback) {
        sceneChangedCallbacks.clear();
        sceneChangedCallbacks.push_back(callback);
    }

    void setCameraChangedCallback(std::function<void()> callback) {
        cameraChangedCallbacks.clear();
        cameraChangedCallbacks.push_back(callback);
    }

    void setShapesChangedCallback(std::function<void()> callback) {
        shapesChangedCallbacks.clear();
        shapesChangedCallbacks.push_back(callback);
    }

    void setMaterialChangedCallback(std::function<void()> callback) {
        materialChangedCallbacks.clear();
        materialChangedCallbacks.push_back(callback);
    }
    
    // Notify all registered callbacks
    void notifySceneChanged() {
        for (const auto& callback : sceneChangedCallbacks) {
            if (callback) {
                callback();
            }
        }
    }

    void notifyCameraChanged() {
        for (const auto& callback : cameraChangedCallbacks) {
            if (callback) {
                callback();
            }
        }
    }

    void notifyShapesChanged() {
        for (const auto& callback : shapesChangedCallbacks) {
            if (callback) {
                callback();
            }
        }
    }

    void notifyMaterialChanged() {
        for (const auto& callback : materialChangedCallbacks) {
            if (callback) {
                callback();
            }
        }
    }

    void notifyBVHChanged() {
        for (const auto& callback : bvhChangedCallbacks) {
            if (callback) {
                callback();
            }
        }
    }

    void executeCommand(ICommand* command) {
        command->execute();
        undoStack.push(std::unique_ptr<ICommand>(command));
        
        // Clear redo stack when new command is executed
        while (!redoStack.empty()) {
            redoStack.pop();
        }
    }

    bool canUndo() const {
        return !undoStack.empty();
    }

    bool canRedo() const {
        return !redoStack.empty();
    }

    void undo() {
        if (!canUndo()) return;
        
        auto command = std::move(undoStack.top());
        undoStack.pop();
        
        command->undo();
        redoStack.push(std::move(command));
        
        notifySceneChanged();
    }

    void redo() {
        if (!canRedo()) return;
        
        auto command = std::move(redoStack.top());
        redoStack.pop();
        
        command->execute();
        undoStack.push(std::move(command));
        
        notifySceneChanged();
    }

    int getUndoCommandID() const {
        if (!canUndo()) return -1;
        return undoStack.top()->getID();
    }

    int getRedoCommandID() const {
        if (!canRedo()) return -1;
        return redoStack.top()->getID();
    }

    const char* getUndoCommandName() const {
        if (!canUndo()) return nullptr;
        // For now, return a generic name based on ID
        // You can enhance this later
        return "Command";
    }

    const char* getRedoCommandName() const {
        if (!canRedo()) return nullptr;
        return "Command";
    }

    void clear() {
        while (!undoStack.empty()) undoStack.pop();
        while (!redoStack.empty()) redoStack.pop();
    }
};
