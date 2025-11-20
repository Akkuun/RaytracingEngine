#include "CameraPanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include "../../core/commands/actionsCommands/CameraMoveCommand.h"
#include "../../core/commands/actionsCommands/CameraRotationCommand.h"
#include "../../core/commands/actionsCommands/CameraFOVCommand.h"
#include "../../core/commands/actionsCommands/CameraResetCommand.h"
#include "../../core/systems/RenderEngine/RenderEngine.h"
#include "../../core/camera/Camera.h"
#include "../../core/commands/CommandsManager.h"
#include "../../core/input/Keybinds.hpp"

CameraPanel::CameraPanel(QWidget *parent) : QWidget(parent)
{
    setupUI();
}

void CameraPanel::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(5);


    Camera& camera = Camera::getInstance();
    CommandsManager& commandManager = CommandsManager::getInstance();

    // Position
    layout->addWidget(new QLabel("POSITION"));
    QHBoxLayout *posLayout = new QHBoxLayout();
    posX = new QDoubleSpinBox();
    posY = new QDoubleSpinBox();
    posZ = new QDoubleSpinBox();
    posX->setPrefix("X: ");
    posX->setSingleStep(0.1);
    posY->setPrefix("Y: ");
    posY->setSingleStep(0.1);
    posZ->setPrefix("Z: ");
    posZ->setSingleStep(0.1);
    posX->setRange(-1000, 1000);
    posY->setRange(-1000, 1000);
    posZ->setRange(-1000, 1000);
    // Set initial values from camera
    auto pos = camera.getPosition();
    posX->setValue(pos.x);
    posY->setValue(pos.y);
    posZ->setValue(pos.z);
    posLayout->addWidget(posX);
    posLayout->addWidget(posY);
    posLayout->addWidget(posZ);
    layout->addLayout(posLayout);

    connect(posX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this, &camera, &commandManager](double newX){
        commandManager.executeCommand(new CameraMoveCommand(camera, newX, this->posY->value(), this->posZ->value()));
    });
    connect(posY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this, &camera, &commandManager](double newY){
        commandManager.executeCommand(new CameraMoveCommand(camera, this->posX->value(), newY, this->posZ->value()));
    });
    connect(posZ, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this, &camera, &commandManager](double newZ){
        commandManager.executeCommand(new CameraMoveCommand(camera, this->posX->value(), this->posY->value(), newZ));
    });

    // Rotation
    layout->addWidget(new QLabel("ROTATION"));
    QHBoxLayout *rotLayout = new QHBoxLayout();
    rotX = new QDoubleSpinBox();
    rotY = new QDoubleSpinBox();
    rotZ = new QDoubleSpinBox();
    rotX->setPrefix("X: ");
    rotX->setSingleStep(1.0);
    rotY->setPrefix("Y: ");
    rotY->setSingleStep(1.0);
    rotZ->setPrefix("Z: ");
    rotZ->setSingleStep(1.0);
    rotX->setRange(-360, 360);
    rotY->setRange(-360, 360);
    rotZ->setRange(-360, 360);
    // Set initial values from camera (in degrees)
    auto rot = camera.getRotationEuler();
    rotX->setValue(glm::degrees(rot.x));
    rotY->setValue(glm::degrees(rot.y));
    rotZ->setValue(glm::degrees(rot.z));
    rotLayout->addWidget(rotX);
    rotLayout->addWidget(rotY);
    rotLayout->addWidget(rotZ);
    layout->addLayout(rotLayout);

    connect(rotX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this, &camera, &commandManager](double newX){
        // Convert degrees to radians for camera
        commandManager.executeCommand(new CameraRotationCommand(camera, glm::radians(newX), glm::radians(this->rotY->value()), glm::radians(this->rotZ->value())));
    });
    connect(rotY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this, &camera, &commandManager](double newY){
        // Convert degrees to radians for camera
        commandManager.executeCommand(new CameraRotationCommand(camera, glm::radians(this->rotX->value()), glm::radians(newY), glm::radians(this->rotZ->value())));
    });
    connect(rotZ, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this, &camera, &commandManager](double newZ){
        // Convert degrees to radians for camera
        commandManager.executeCommand(new CameraRotationCommand(camera, glm::radians(this->rotX->value()), glm::radians(this->rotY->value()), glm::radians(newZ)));
    });

    // FOV
    layout->addWidget(new QLabel("FOV"));
    QHBoxLayout *fovLayout = new QHBoxLayout();
    fovSpin = new QSpinBox();
    fovSpin->setRange(1, 180);
    fovSpin->setValue(camera.getFOV()); // Set initial value from camera
    QCheckBox *focusCheck = new QCheckBox("FOCUS");
    fovLayout->addWidget(fovSpin);
    fovLayout->addWidget(focusCheck);
    layout->addLayout(fovLayout);
    
    connect(fovSpin, QOverload<int>::of(&QSpinBox::valueChanged), [this, &camera, &commandManager](int newFOV){
        commandManager.executeCommand(new CameraFOVCommand(camera, static_cast<float>(newFOV)));
    });

    // Reset Button
    QPushButton *resetButton = new QPushButton("Reset Camera");
    Keybinds& keybinds = Keybinds::getInstance();
    resetButton->setToolTip(QString("Reset camera to default position (%1)").arg(keybinds.getKeybind(KB_RESET_CAMERA).toString()));
    connect(resetButton, &QPushButton::clicked, [&camera, &commandManager](){
        commandManager.executeCommand(new CameraResetCommand(camera));
    });
    layout->addWidget(resetButton);

    // Connect camera signals to update UI when camera changes externally
    connect(&camera, &Camera::positionChanged, this, &CameraPanel::onCameraPositionChanged);
    connect(&camera, &Camera::rotationChanged, this, &CameraPanel::onCameraRotationChanged);
    connect(&camera, &Camera::fovChanged, this, &CameraPanel::onCameraFOVChanged);

    // Only style the text color, inherit background from parent
    setStyleSheet("QLabel { color: white; }");
}

void CameraPanel::onCameraPositionChanged(float x, float y, float z)
{
    // Block signals to prevent feedback loop
    posX->blockSignals(true);
    posY->blockSignals(true);
    posZ->blockSignals(true);
    
    posX->setValue(x);
    posY->setValue(y);
    posZ->setValue(z);
    
    posX->blockSignals(false);
    posY->blockSignals(false);
    posZ->blockSignals(false);
}

void CameraPanel::onCameraRotationChanged(float x, float y, float z)
{
    // Block signals to prevent feedback loop
    rotX->blockSignals(true);
    rotY->blockSignals(true);
    rotZ->blockSignals(true);
    
    rotX->setValue(x);
    rotY->setValue(y);
    rotZ->setValue(z);
    
    rotX->blockSignals(false);
    rotY->blockSignals(false);
    rotZ->blockSignals(false);
}

void CameraPanel::onCameraFOVChanged(float fov)
{
    // Block signals to prevent feedback loop
    fovSpin->blockSignals(true);
    fovSpin->setValue(static_cast<int>(fov));
    fovSpin->blockSignals(false);
}