#pragma once

#include <QWidget>
#include <QTreeWidget>
#include <QPushButton>
#include <QToolButton>
#include <QLabel>
#include "../../core/commands/CommandsManager.h"

class ScenePanel : public QWidget
{
    Q_OBJECT

private:
    QTreeWidget *sceneTree;
    CommandsManager &commandManager;
    
    // Buttons
    QPushButton *addSphereBtn;
    QPushButton *addSquareBtn;
    QPushButton *addTriangleBtn;
    QToolButton *undoBtn;
    QToolButton *redoBtn;
    QToolButton *deleteBtn;

public:
    explicit ScenePanel(QWidget *parent = nullptr);
    void setupUI();
    void updateSceneTree();
    void updateUndoRedoButtons();

private slots:
    void onAddSphere();
    void onAddSquare();
    void onAddTriangle();
    void onUndo();
    void onRedo();
    void onDelete();
    void onItemSelectionChanged();

protected:
    void keyPressEvent(QKeyEvent *event) override;
};
