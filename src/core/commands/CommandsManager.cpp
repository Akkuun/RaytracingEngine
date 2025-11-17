#ifndef COMMAND_MANAGER_H
#define COMMAND_MANAGER_H

#include "ICommand.h"
#include <stack>
#include <memory>
#include <functional>

class CommandsManager {
private:
    std::stack<std::unique_ptr<ICommand>> undoStack;
    std::stack<std::unique_ptr<ICommand>> redoStack;
    
    // Callback for notifying listeners (UI) about scene changes
    std::function<void()> sceneChangedCallback;
    
    CommandsManager() = default;

public:
    static CommandsManager& getInstance() {
        static CommandsManager instance;
        return instance;
    }

    CommandsManager(const CommandsManager&) = delete;
    CommandsManager& operator=(const CommandsManager&) = delete;

    // Register callback for scene changes (called after execute/undo/redo)
    void setSceneChangedCallback(std::function<void()> callback) {
        sceneChangedCallback = callback;
    }

    void executeCommand(ICommand* command) {
        command->execute();
        undoStack.push(std::unique_ptr<ICommand>(command));
        
        // Clear redo stack when new command is executed
        while (!redoStack.empty()) {
            redoStack.pop();
        }
        
        // Notify that scene changed
        if (sceneChangedCallback) {
            sceneChangedCallback();
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
        
        if (sceneChangedCallback) {
            sceneChangedCallback();
        }
    }

    void redo() {
        if (!canRedo()) return;
        
        auto command = std::move(redoStack.top());
        redoStack.pop();
        
        command->execute();
        undoStack.push(std::move(command));
        
        if (sceneChangedCallback) {
            sceneChangedCallback();
        }
    }

    int getUndoCommandID() const {
        if (!canUndo()) return -1;
        return undoStack.top()->getID();
    }

    int getRedoCommandID() const {
        if (!canRedo()) return -1;
        return redoStack.top()->getID();
    }

    void clear() {
        while (!undoStack.empty()) undoStack.pop();
        while (!redoStack.empty()) redoStack.pop();
    }
};

#endif // COMMAND_MANAGER_H