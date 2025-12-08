#pragma once

#include <QWidget>
#include <QPushButton>
#include "../../core/commands/CommandsManager.h"

// Forward declaration
class SceneTreeWidget;

class ScenePanel : public QWidget
{
    Q_OBJECT

signals:
    void shapeSelectionChanged(int shapeID);
    void addedShape();

private:
    SceneTreeWidget *sceneTreeWidget;
    CommandsManager &commandManager;
    
    // Buttons
    QPushButton *addSphereBtn;
    QPushButton *addSquareBtn;
    QPushButton *addTriangleBtn;
    QPushButton *addMeshBtn;

public:
    explicit ScenePanel(QWidget *parent = nullptr);
    void setupUI();
    SceneTreeWidget* getSceneTreeWidget() const { return sceneTreeWidget; }

private slots:
    void onAddSphere();
    void onAddSquare();
    void onAddTriangle();
    void onAddMesh();
};
