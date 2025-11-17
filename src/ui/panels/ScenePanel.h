#pragma once

#include <QWidget>
#include <QPushButton>
#include "../../core/commands/CommandsManager.h"

// Forward declaration
class SceneTreeWidget;

class ScenePanel : public QWidget
{
    Q_OBJECT

private:
    SceneTreeWidget *sceneTreeWidget;
    CommandsManager &commandManager;
    
    // Buttons
    QPushButton *addSphereBtn;
    QPushButton *addSquareBtn;
    QPushButton *addTriangleBtn;

public:
    explicit ScenePanel(QWidget *parent = nullptr);
    void setupUI();

private slots:
    void onAddSphere();
    void onAddSquare();
    void onAddTriangle();
};
