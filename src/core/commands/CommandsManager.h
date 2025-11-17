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
    
    // Legacy method for backward compatibility
    void setSceneChangedCallback(std::function<void()> callback) {
        sceneChangedCallbacks.clear();
        sceneChangedCallbacks.push_back(callback);
    }
    
    // Notify all registered callbacks
    void notifySceneChanged() {
        for (auto& callback : sceneChangedCallbacks) {
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
        
        // Notify that scene changed
        notifySceneChanged();
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

    const int getUndoCommandID() const {
        if (!canUndo()) return -1;
        return undoStack.top()->getID();
    }

    const int getRedoCommandID() const {
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
