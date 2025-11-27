#include "ScenePanel.h"
#include "SceneTreeWidget.h"
#include "../../core/commands/actionsCommands/shapes/AddShapeCommand.h"
#include "../../core/systems/SceneManager/SceneManager.h"
#include "../../core/shapes/Mesh.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QFileDialog>

ScenePanel::ScenePanel(QWidget *parent) 
    : QWidget(parent)
    , commandManager(CommandsManager::getInstance())
{
    setupUI();
}

void ScenePanel::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(8);
    mainLayout->setContentsMargins(8, 8, 8, 8);

    // Scene Tree Widget
    sceneTreeWidget = new SceneTreeWidget(this);
    mainLayout->addWidget(sceneTreeWidget);

    // Add Shapes buttons
    QFrame *addShapesFrame = new QFrame(this);
    addShapesFrame->setStyleSheet(
        "QFrame {"
        "    background-color: rgba(40, 40, 40, 180);"
        "    border-radius: 6px;"
        "    padding: 4px;"
        "}"
    );
    
    QVBoxLayout *addShapesLayout = new QVBoxLayout(addShapesFrame);
    addShapesLayout->setSpacing(4);
    addShapesLayout->setContentsMargins(4, 4, 4, 4);

    // Title
    QLabel *titleLabel = new QLabel("Add Shape", this);
    titleLabel->setStyleSheet(
        "QLabel {"
        "    color: rgba(255, 255, 255, 180);"
        "    font-size: 10px;"
        "    font-weight: bold;"
        "    padding-left: 2px;"
        "}"
    );
    addShapesLayout->addWidget(titleLabel);

    // Button style - taille réduite
    QString buttonStyle = 
        "QPushButton {"
        "    background-color: rgba(60, 60, 60, 200);"
        "    color: white;"
        "    border: 1px solid rgba(80, 80, 80, 200);"
        "    border-radius: 4px;"
        "    padding: 4px 8px;"
        "    font-size: 11px;"
        "    font-weight: 500;"
        "    text-align: left;"
        "}"
        "QPushButton:hover {"
        "    background-color: rgba(0, 120, 215, 180);"
        "    border: 1px solid rgba(0, 140, 235, 255);"
        "}"
        "QPushButton:pressed {"
        "    background-color: rgba(0, 100, 195, 200);"
        "}";

    // Sphere button
    addSphereBtn = new QPushButton("● Sphere", this);
    addSphereBtn->setStyleSheet(buttonStyle);
    addSphereBtn->setCursor(Qt::PointingHandCursor);
    addShapesLayout->addWidget(addSphereBtn);

    // Square button
    addSquareBtn = new QPushButton("■ Square", this);
    addSquareBtn->setStyleSheet(buttonStyle);
    addSquareBtn->setCursor(Qt::PointingHandCursor);
    addShapesLayout->addWidget(addSquareBtn);

    // Triangle button
    addTriangleBtn = new QPushButton("▲ Triangle", this);
    addTriangleBtn->setStyleSheet(buttonStyle);
    addTriangleBtn->setCursor(Qt::PointingHandCursor);
    addShapesLayout->addWidget(addTriangleBtn);

    // Mesh button
    addMeshBtn = new QPushButton("≡ Mesh", this);
    addMeshBtn->setStyleSheet(buttonStyle);
    addMeshBtn->setCursor(Qt::PointingHandCursor);
    addShapesLayout->addWidget(addMeshBtn);

    mainLayout->addWidget(addShapesFrame);

    // Connect signals
    connect(addSphereBtn, &QPushButton::clicked, this, &ScenePanel::onAddSphere);
    connect(addSquareBtn, &QPushButton::clicked, this, &ScenePanel::onAddSquare);
    connect(addTriangleBtn, &QPushButton::clicked, this, &ScenePanel::onAddTriangle);
    connect(addMeshBtn, &QPushButton::clicked, this, &ScenePanel::onAddMesh);
    
    // Forward the shape selection signal from SceneTreeWidget
    connect(sceneTreeWidget, &SceneTreeWidget::shapeSelectionChanged, 
            this, &ScenePanel::shapeSelectionChanged);
}

void ScenePanel::onAddSphere()
{
    Shape *sphere = new Sphere(); // Use default constructor
    commandManager.executeCommand(new AddShapeCommand(sphere));
}

void ScenePanel::onAddSquare()
{
    Shape *square = new Square(); // Use default constructor
    commandManager.executeCommand(new AddShapeCommand(square));
}

void ScenePanel::onAddTriangle()
{
    Shape *triangle = new Triangle(); // Use default constructor
    commandManager.executeCommand(new AddShapeCommand(triangle));
}

void ScenePanel::onAddMesh()
{
    // open file dialog to select mesh file (.off only)
    QString filePath = QFileDialog::getOpenFileName(this, "Select Mesh File", "", "Mesh Files (*.off)");
    if (!filePath.isEmpty()) {
        commandManager.executeCommand(new AddShapeCommand(new Mesh(filePath.toStdString())));
    }
}