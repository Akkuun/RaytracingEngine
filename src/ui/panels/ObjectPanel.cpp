#include "ObjectPanel.h"
#include "../../core/systems/KernelManager/KernelManager.h"
#include "../../core/systems/DeviceManager/DeviceManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QPixmap>
#include <QPainter>
#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>
#include <QMessageBox>
#include "../../core/commands/actionsCommands/MoveShapeCommand.h"
#include "../../core/commands/actionsCommands/ScaleShapeCommand.h"
#include "../../core/commands/actionsCommands/RotateShapeCommand.h"
#include "../../core/commands/actionsCommands/SetTextureShape.h"
#include "../../core/commands/actionsCommands/ClearTextureShape.h"
#include "../../core/systems/SceneManager/SceneManager.h"
#include <QKeyEvent>

ObjectPanel::ObjectPanel(QWidget *parent) : QWidget(parent), currentSelectedShapeID(SceneManager::getInstance().getShapes().front()->getID()), commandManager(CommandsManager::getInstance())
{
    setupUI();
}

void ObjectPanel::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(5);

    Shape *initialShape = SceneManager::getInstance().getShapeByID(currentSelectedShapeID);

    // Position
    layout->addWidget(new QLabel("POSITION"));
    QHBoxLayout *posLayout = new QHBoxLayout();
    posX = new QDoubleSpinBox();
    posY = new QDoubleSpinBox();
    posZ = new QDoubleSpinBox();
    posX->setPrefix("X: ");
    posY->setPrefix("Y: ");
    posZ->setPrefix("Z: ");
    posX->setRange(-1000, 1000);
    posY->setRange(-1000, 1000);
    posZ->setRange(-1000, 1000);
    posX->setSingleStep(0.1);
    posY->setSingleStep(0.1);
    posZ->setSingleStep(0.1);
    posX->setMaximumWidth(90);
    posY->setMaximumWidth(90);
    posZ->setMaximumWidth(90);
    posLayout->addWidget(posX);
    posLayout->addWidget(posY);
    posLayout->addWidget(posZ);
    posX->setValue(initialShape->getPosition().x);
    posY->setValue(initialShape->getPosition().y);
    posZ->setValue(initialShape->getPosition().z);
    layout->addLayout(posLayout);

    // Rotation
    layout->addWidget(new QLabel("ROTATION"));
    QHBoxLayout *rotLayout = new QHBoxLayout();
    rotX = new QDoubleSpinBox();
    rotY = new QDoubleSpinBox();
    rotZ = new QDoubleSpinBox();
    rotX->setPrefix("X: ");
    rotY->setPrefix("Y: ");
    rotZ->setPrefix("Z: ");
    rotX->setRange(-360, 360);
    rotY->setRange(-360, 360);
    rotZ->setRange(-360, 360);
    rotX->setSingleStep(1.0);
    rotY->setSingleStep(1.0);
    rotZ->setSingleStep(1.0);
    rotX->setValue(initialShape->getRotation().x);
    rotY->setValue(initialShape->getRotation().y);
    rotZ->setValue(initialShape->getRotation().z);
    rotX->setMaximumWidth(90);
    rotY->setMaximumWidth(90);
    rotZ->setMaximumWidth(90);
    rotLayout->addWidget(rotX);
    rotLayout->addWidget(rotY);
    rotLayout->addWidget(rotZ);
    layout->addLayout(rotLayout);

    // Scale
    layout->addWidget(new QLabel("SCALE"));
    QHBoxLayout *scaleLayout = new QHBoxLayout();
    scaleX = new QDoubleSpinBox();
    scaleY = new QDoubleSpinBox();
    scaleZ = new QDoubleSpinBox();
    scaleX->setPrefix("X: ");
    scaleY->setPrefix("Y: ");
    scaleZ->setPrefix("Z: ");
    scaleX->setRange(0.01, 100);
    scaleY->setRange(0.01, 100);
    scaleZ->setRange(0.01, 100);
    scaleX->setSingleStep(0.005); // ??? why not 0.05
    scaleY->setSingleStep(0.005);
    scaleZ->setSingleStep(0.005);
    scaleX->setValue(1);
    scaleY->setValue(1);
    scaleZ->setValue(1);
    scaleX->setMaximumWidth(90);
    scaleY->setMaximumWidth(90);
    scaleZ->setMaximumWidth(90);
    scaleLayout->addWidget(scaleX);
    scaleLayout->addWidget(scaleY);
    scaleLayout->addWidget(scaleZ);
    scaleX->setValue(initialShape->getScale().x);
    scaleY->setValue(initialShape->getScale().y);
    scaleZ->setValue(initialShape->getScale().z);
    layout->addLayout(scaleLayout);

    // Only style the text color, inherit background from parent
    setStyleSheet("QLabel { color: white; }");

    // Connect position spin boxes changes to execute the correct command (Move command)
    connect(posX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double newX)
            { commandManager.executeCommand(new MoveShapeCommand(currentSelectedShapeID, newX, posY->value(), posZ->value())); });

    connect(posY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double newY)
            { commandManager.executeCommand(new MoveShapeCommand(currentSelectedShapeID, posX->value(), newY, posZ->value())); });

    connect(posZ, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double newZ)
            { commandManager.executeCommand(new MoveShapeCommand(currentSelectedShapeID, posX->value(), posY->value(), newZ)); });

    // Connection rotation spin boxes changes
    connect(rotX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double newX)
            {
                if (SceneManager::getInstance().getShapes().empty()) return;
                commandManager.executeCommand(new RotateShapeCommand(currentSelectedShapeID, newX, rotY->value(), rotZ->value()));
            });

    connect(rotY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double newY)
            {
                if (SceneManager::getInstance().getShapes().empty()) return;
                commandManager.executeCommand(new RotateShapeCommand(currentSelectedShapeID, rotX->value(), newY, rotZ->value()));
            });

    connect(rotZ, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double newZ)
            {
                if (SceneManager::getInstance().getShapes().empty()) return;
                commandManager.executeCommand(new RotateShapeCommand(currentSelectedShapeID, rotX->value(), rotY->value(), newZ));
            });

    // Lambda function to synchronize scale for all axis for sphere shapes ONLY
    auto applyUniformScalling = [this](double value)
    {
        scaleX->blockSignals(true);
        scaleY->blockSignals(true);
        scaleZ->blockSignals(true);
        scaleX->setValue(value);
        scaleY->setValue(value);
        scaleZ->setValue(value);
        scaleX->blockSignals(false);
        scaleY->blockSignals(false);
        scaleZ->blockSignals(false);
        commandManager.executeCommand(new ScaleShapeCommand(currentSelectedShapeID, value, value, value));
    };

    // Connection scale spin boxes changes, IF shape is SPHERE or CTRL is pressed we apply uniform scaling
    connect(scaleX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this, applyUniformScalling](double newX)
            {
        Shape *shape = SceneManager::getInstance().getShapeByID(currentSelectedShapeID);
        if (shape && (shape->getType() == ShapeType::SPHERE || isShortcutPressed())) {
            applyUniformScalling(newX);
        } else {
            if (SceneManager::getInstance().getShapes().empty()) return;
            commandManager.executeCommand(new ScaleShapeCommand(currentSelectedShapeID, newX, scaleY->value(), scaleZ->value()));
        } });

    connect(scaleY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this, applyUniformScalling](double newY)
            {
        Shape *shape = SceneManager::getInstance().getShapeByID(currentSelectedShapeID);
        if (shape && (shape->getType() == ShapeType::SPHERE || isShortcutPressed())) {
            applyUniformScalling(newY);
        } else {
            if (SceneManager::getInstance().getShapes().empty()) return;
            commandManager.executeCommand(new ScaleShapeCommand(currentSelectedShapeID, scaleX->value(), newY, scaleZ->value()));
        } });

    connect(scaleZ, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this, applyUniformScalling](double newZ)
            {
        Shape* shape = SceneManager::getInstance().getShapeByID(currentSelectedShapeID);
        if (shape && (shape->getType() == ShapeType::SPHERE || isShortcutPressed())) {
            applyUniformScalling(newZ);
        } else {
            if (SceneManager::getInstance().getShapes().empty()) return;
            commandManager.executeCommand(new ScaleShapeCommand(currentSelectedShapeID, scaleX->value(), scaleY->value(), newZ));
        } });
}

// function to update the current selected shape properties in the panel
void ObjectPanel::onShapeSelectionChanged(int shapeID)
{
    // If no shape is selected (shapeID == -1), do nothing
    if (shapeID == -1)
    {
        return;
    }

    // Get the shape from SceneManager
    Shape *shape = SceneManager::getInstance().getShapeByID(shapeID);

    // Check if the shape still exists (it might have been deleted)
    if (shape == nullptr)
    {
        return;
    }

    // Update current selected shape ID
    currentSelectedShapeID = shapeID;

    // Block signals to avoid triggering valueChanged while updating from code
    posX->blockSignals(true);
    posY->blockSignals(true);
    posZ->blockSignals(true);
    rotX->blockSignals(true);
    rotY->blockSignals(true);
    rotZ->blockSignals(true);
    scaleX->blockSignals(true);
    scaleY->blockSignals(true);
    scaleZ->blockSignals(true);

    // Set the current selected shape position
    posX->setValue(shape->getPosition().x);
    posY->setValue(shape->getPosition().y);
    posZ->setValue(shape->getPosition().z);

    // Set the current selected shape rotation
    rotX->setValue(shape->getRotation().x);
    rotY->setValue(shape->getRotation().y);
    rotZ->setValue(shape->getRotation().z);

    // Set the current selected shape scale
    scaleX->setValue(shape->getScale().x);
    scaleY->setValue(shape->getScale().y);
    scaleZ->setValue(shape->getScale().z);

    // Unblock signals
    posX->blockSignals(false);
    posY->blockSignals(false);
    posZ->blockSignals(false);
    rotX->blockSignals(false);
    rotY->blockSignals(false);
    rotZ->blockSignals(false);
    scaleX->blockSignals(false);
    scaleY->blockSignals(false);
    scaleZ->blockSignals(false);

    emit selectionShapeChanged(shapeID);
}

// apply the scale on all axis
void ObjectPanel::setApplyOnAllAxis(bool apply)
{
    applyOnAllAxis = apply;
}

// set true if the key is actually pressed
void ObjectPanel::handleKeyPress(int key, bool pressed)
{
    keysPressed[key] = pressed;
}
// true when pressed
bool ObjectPanel::isShortcutPressed() const
{
    return keysPressed[Qt::Key_Control];
}
