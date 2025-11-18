#pragma once

#include <QWidget>
#include <QTreeWidget>
#include <QToolButton>
#include <QKeyEvent>
#include "../../core/commands/CommandsManager.h"

class SceneTreeWidget : public QWidget
{
    Q_OBJECT

private:
    QTreeWidget *sceneTree;
    CommandsManager &commandManager;
    QToolButton *undoBtn;
    QToolButton *redoBtn;
    QToolButton *deleteBtn;

public:
    explicit SceneTreeWidget(QWidget *parent = nullptr);
    void updateSceneTree();
    void updateUndoRedoButtons();

signals:
    void shapeSelectionChanged(int shapeID);    
private slots:
    void onUndo();
    void onRedo();
    void onDelete();
    void onItemSelectionChanged();

    int getSelectedShapeID() const;

protected:
    void keyPressEvent(QKeyEvent *event) override;
};
