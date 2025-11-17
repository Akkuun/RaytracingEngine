#include "ScenePanel.h"
#include "../../core/commands/actionsCommands/AddShapeCommand.h"
#include "../../core/commands/actionsCommands/DeleteShapeCommand.h"
#include "../../core/systems/SceneManager/SceneManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QToolButton>
#include <QPushButton>
#include <QFrame>
#include <QLabel>
#include <QKeyEvent>

ScenePanel::ScenePanel(QWidget *parent) 
    : QWidget(parent)
    , commandManager(CommandsManager::getInstance())
{
    setupUI();
    
    // Register callback for scene changes
    commandManager.addSceneChangedCallback([this]() {
        updateSceneTree();
        updateUndoRedoButtons();
    });
    
    // Initialize tree with existing shapes
    updateSceneTree();
    updateUndoRedoButtons();
}

void ScenePanel::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(8);
    mainLayout->setContentsMargins(8, 8, 8, 8);

    // === TOP TOOLBAR: Undo/Redo/Delete ===
    QFrame *topToolbar = new QFrame(this);
    topToolbar->setStyleSheet(
        "QFrame {"
        "    background-color: rgba(40, 40, 40, 180);"
        "    border-radius: 6px;"
        "    padding: 4px;"
        "}"
    );
    
    QHBoxLayout *topToolbarLayout = new QHBoxLayout(topToolbar);
    topToolbarLayout->setSpacing(4);
    topToolbarLayout->setContentsMargins(4, 4, 4, 4);

    // Undo button
    undoBtn = new QToolButton(this);
    undoBtn->setText("↶");
    undoBtn->setToolTip("Undo (Ctrl+Z)");
    undoBtn->setFixedSize(32, 32);
    
    // Redo button
    redoBtn = new QToolButton(this);
    redoBtn->setText("↷");
    redoBtn->setToolTip("Redo (Ctrl+Shift+Z)");
    redoBtn->setFixedSize(32, 32);
    
    // Delete button
    deleteBtn = new QToolButton(this);
    deleteBtn->setText("🗑");
    deleteBtn->setToolTip("Delete (Del)");
    deleteBtn->setFixedSize(32, 32);
    
    // Style for tool buttons
    QString toolButtonStyle = 
        "QToolButton {"
        "    background-color: rgba(60, 60, 60, 150);"
        "    color: white;"
        "    border: 1px solid rgba(80, 80, 80, 200);"
        "    border-radius: 4px;"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "}"
        "QToolButton:hover {"
        "    background-color: rgba(80, 80, 80, 200);"
        "    border: 1px solid rgba(100, 100, 100, 255);"
        "}"
        "QToolButton:pressed {"
        "    background-color: rgba(50, 50, 50, 255);"
        "}"
        "QToolButton:disabled {"
        "    background-color: rgba(40, 40, 40, 100);"
        "    color: rgba(255, 255, 255, 100);"
        "    border: 1px solid rgba(60, 60, 60, 150);"
        "}";
    
    undoBtn->setStyleSheet(toolButtonStyle);
    redoBtn->setStyleSheet(toolButtonStyle);
    deleteBtn->setStyleSheet(toolButtonStyle);
    
    topToolbarLayout->addWidget(undoBtn);
    topToolbarLayout->addWidget(redoBtn);
    topToolbarLayout->addStretch();
    topToolbarLayout->addWidget(deleteBtn);
    
    mainLayout->addWidget(topToolbar);

    // Scene Tree
    sceneTree = new QTreeWidget(this);
    sceneTree->setHeaderHidden(true);
    sceneTree->setIndentation(20);
    sceneTree->setStyleSheet(
        "QTreeWidget {"
        "    background-color: rgba(30, 30, 30, 180);"
        "    color: white;"
        "    border: 1px solid rgba(60, 60, 60, 150);"
        "    border-radius: 6px;"
        "    padding: 4px;"
        "}"
        "QTreeWidget::item {"
        "    padding: 4px;"
        "    border-radius: 3px;"
        "}"
        "QTreeWidget::item:hover {"
        "    background-color: rgba(64, 64, 64, 150);"
        "}"
        "QTreeWidget::item:selected {"
        "    background-color: rgba(0, 120, 215, 180);"
        "}"
    );

    mainLayout->addWidget(sceneTree);

    // Add Shapes buttons
    QFrame *addShapesFrame = new QFrame(this);
    addShapesFrame->setStyleSheet(
        "QFrame {"
        "    background-color: rgba(40, 40, 40, 180);"
        "    border-radius: 6px;"
        "    padding: 6px;"
        "}"
    );
    
    QVBoxLayout *addShapesLayout = new QVBoxLayout(addShapesFrame);
    addShapesLayout->setSpacing(6);
    addShapesLayout->setContentsMargins(6, 6, 6, 6);

    // Title
    QLabel *titleLabel = new QLabel("Add Shape", this);
    titleLabel->setStyleSheet(
        "QLabel {"
        "    color: rgba(255, 255, 255, 180);"
        "    font-size: 11px;"
        "    font-weight: bold;"
        "    padding-left: 4px;"
        "}"
    );
    addShapesLayout->addWidget(titleLabel);

    // Button style
    QString buttonStyle = 
        "QPushButton {"
        "    background-color: rgba(60, 60, 60, 200);"
        "    color: white;"
        "    border: 1px solid rgba(80, 80, 80, 200);"
        "    border-radius: 5px;"
        "    padding: 8px 16px;"
        "    font-size: 13px;"
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
    addSphereBtn = new QPushButton("⚪ Sphere", this);
    addSphereBtn->setStyleSheet(buttonStyle);
    addSphereBtn->setCursor(Qt::PointingHandCursor);
    addShapesLayout->addWidget(addSphereBtn);

    // Square button
    addSquareBtn = new QPushButton("⬜ Square", this);
    addSquareBtn->setStyleSheet(buttonStyle);
    addSquareBtn->setCursor(Qt::PointingHandCursor);
    addShapesLayout->addWidget(addSquareBtn);

    // Triangle button
    addTriangleBtn = new QPushButton("🔺 Triangle", this);
    addTriangleBtn->setStyleSheet(buttonStyle);
    addTriangleBtn->setCursor(Qt::PointingHandCursor);
    addShapesLayout->addWidget(addTriangleBtn);

    mainLayout->addWidget(addShapesFrame);

    // Connect signals
    connect(addSphereBtn, &QPushButton::clicked, this, &ScenePanel::onAddSphere);
    connect(addSquareBtn, &QPushButton::clicked, this, &ScenePanel::onAddSquare);
    connect(addTriangleBtn, &QPushButton::clicked, this, &ScenePanel::onAddTriangle);
    connect(undoBtn, &QToolButton::clicked, this, &ScenePanel::onUndo);
    connect(redoBtn, &QToolButton::clicked, this, &ScenePanel::onRedo);
    connect(deleteBtn, &QToolButton::clicked, this, &ScenePanel::onDelete);
    connect(sceneTree, &QTreeWidget::itemSelectionChanged, this, &ScenePanel::onItemSelectionChanged);
    
    setFocusPolicy(Qt::StrongFocus);
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

void ScenePanel::onUndo()
{
    commandManager.undo();
}

void ScenePanel::onRedo()
{
    commandManager.redo();
}

void ScenePanel::onDelete()
{
    QTreeWidgetItem *selected = sceneTree->currentItem();
    if (!selected) return;
    
    // Check if this item has shape data (user added shapes, not environment items)
    QVariant shapeData = selected->data(0, Qt::UserRole);
    if (!shapeData.isValid()) return;
    
    // Get shape pointer from user data
    Shape* shape = reinterpret_cast<Shape*>(shapeData.value<quintptr>());
    if (shape) {
        commandManager.executeCommand(new DeleteShapeCommand(shape));
    }
}

void ScenePanel::onItemSelectionChanged()
{
    deleteBtn->setEnabled(sceneTree->currentItem() != nullptr);
}

void ScenePanel::updateSceneTree()
{
    // Clear all items from the tree
    sceneTree->clear();
    
    // Get shapes from SceneManager
    const std::vector<Shape*>& shapes = SceneManager::getInstance().getShapes();
    
    // Add each shape directly to the tree (no hierarchy)
    for (size_t i = 0; i < shapes.size(); ++i) {
        Shape* shape = shapes[i];
        
        // Create item with appropriate icon based on shape type
        QTreeWidgetItem *shapeItem = new QTreeWidgetItem(sceneTree);
        
        // Determine shape type and create appropriate label
        QString shapeLabel;
        ShapeType type = shape->getType();
        
        switch (type) {
            case ShapeType::SPHERE:
                shapeLabel = QString("⚪ Sphere #%1").arg(shape->getID());
                break;
            case ShapeType::SQUARE:
                shapeLabel = QString("⬜ Square #%1").arg(shape->getID());
                break;
            case ShapeType::TRIANGLE:
                shapeLabel = QString("🔺 Triangle #%1").arg(shape->getID());
                break;
            default:
                shapeLabel = QString("❓ Shape #%1").arg(shape->getID());
                break;
        }
        
        shapeItem->setText(0, shapeLabel);
        shapeItem->setFlags(shapeItem->flags() | Qt::ItemIsUserCheckable);
        shapeItem->setCheckState(0, Qt::Checked);
        
        // Store shape pointer as user data for later retrieval
        shapeItem->setData(0, Qt::UserRole, QVariant::fromValue(reinterpret_cast<quintptr>(shape)));
    }
}

void ScenePanel::updateUndoRedoButtons()
{
    undoBtn->setEnabled(commandManager.canUndo());
    redoBtn->setEnabled(commandManager.canRedo());
    
    // Update tooltips with command names
    if (commandManager.canUndo()) {
        undoBtn->setToolTip(QString("Undo: %1 (Ctrl+Z)").arg(commandManager.getUndoCommandName()));
    } else {
        undoBtn->setToolTip("Undo (Ctrl+Z)");
    }
    
    if (commandManager.canRedo()) {
        redoBtn->setToolTip(QString("Redo: %1 (Ctrl+Shift+Z)").arg(commandManager.getRedoCommandName()));
    } else {
        redoBtn->setToolTip("Redo (Ctrl+Shift+Z)");
    }
}

void ScenePanel::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_Z) {
        if (commandManager.canUndo()) {
            commandManager.undo();
        }
    }
    else if (event->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier) && event->key() == Qt::Key_Z) {
        if (commandManager.canRedo()) {
            commandManager.redo();
        }
    }
    else if (event->key() == Qt::Key_Delete) {
        onDelete();
    }
    else {
        QWidget::keyPressEvent(event);
    }
}