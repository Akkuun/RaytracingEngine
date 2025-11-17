#include "SceneTreeWidget.h"
#include "../../core/commands/actionsCommands/DeleteShapeCommand.h"
#include "../../core/systems/SceneManager/SceneManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QKeyEvent>

SceneTreeWidget::SceneTreeWidget(QWidget *parent)
    : QWidget(parent)
    , commandManager(CommandsManager::getInstance())
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(8);
    mainLayout->setContentsMargins(0, 0, 0, 0);

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

    // Connect signals
    connect(undoBtn, &QToolButton::clicked, this, &SceneTreeWidget::onUndo);
    connect(redoBtn, &QToolButton::clicked, this, &SceneTreeWidget::onRedo);
    connect(deleteBtn, &QToolButton::clicked, this, &SceneTreeWidget::onDelete);
    connect(sceneTree, &QTreeWidget::itemSelectionChanged, this, &SceneTreeWidget::onItemSelectionChanged);
    
    setFocusPolicy(Qt::StrongFocus);
    
    // Register callback for scene changes
    commandManager.addSceneChangedCallback([this]() {
        updateSceneTree();
        updateUndoRedoButtons();
    });
    
    // Initialize tree with existing shapes
    updateSceneTree();
    updateUndoRedoButtons();
}

void SceneTreeWidget::onUndo()
{
    commandManager.undo();
}

void SceneTreeWidget::onRedo()
{
    commandManager.redo();
}

void SceneTreeWidget::onDelete()
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

void SceneTreeWidget::onItemSelectionChanged()
{
    deleteBtn->setEnabled(sceneTree->currentItem() != nullptr);
}

void SceneTreeWidget::updateSceneTree()
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
        QString shapeLabel = QString(shape->getName().c_str()); // get the name of the shape

        shapeItem->setText(0, shapeLabel);
        shapeItem->setFlags(shapeItem->flags() | Qt::ItemIsUserCheckable);
        shapeItem->setCheckState(0, Qt::Checked);
        
        // Store shape pointer as user data for later retrieval
        shapeItem->setData(0, Qt::UserRole, QVariant::fromValue(reinterpret_cast<quintptr>(shape)));
    }
}

void SceneTreeWidget::updateUndoRedoButtons()
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

void SceneTreeWidget::keyPressEvent(QKeyEvent *event)
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

int SceneTreeWidget::getSelectedShapeID() const
{
    QTreeWidgetItem *selected = sceneTree->currentItem();
    if (!selected) return -1;

    QVariant shapeData = selected->data(0, Qt::UserRole);
    if (!shapeData.isValid()) return -1;

    Shape* shape = reinterpret_cast<Shape*>(shapeData.value<quintptr>());
    if (shape) {
        return shape->getID();
    }
    return -1;
}
