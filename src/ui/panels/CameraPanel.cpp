#include "CameraPanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QCheckBox>
#include "../../core/commands/actionsCommands/CameraMoveCommand.h"
#include "../../core/commands/actionsCommands/CameraRotationCommand.h"
#include "../../core/systems/RenderEngine/RenderEngine.h"
#include "../../core/camera/Camera.h"
#include "../../core/commands/CommandsManager.h"

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
    QDoubleSpinBox *posX = new QDoubleSpinBox();
    QDoubleSpinBox *posY = new QDoubleSpinBox();
    QDoubleSpinBox *posZ = new QDoubleSpinBox();
    posX->setPrefix("X: ");
    posX->setSingleStep(0.1);
    posY->setPrefix("Y: ");
    posY->setSingleStep(0.1);
    posZ->setPrefix("Z: ");
    posZ->setSingleStep(0.1);
    posX->setRange(-1000, 1000);
    posY->setRange(-1000, 1000);
    posZ->setRange(-1000, 1000);
    posLayout->addWidget(posX);
    posLayout->addWidget(posY);
    posLayout->addWidget(posZ);
    layout->addLayout(posLayout);

    connect(posX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this, &camera, &commandManager, posY, posZ](double newX){
        commandManager.executeCommand(new CameraMoveCommand(camera, newX, posY->value(), posZ->value()));
    });
    connect(posY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this, &camera, &commandManager, posX, posZ](double newY){
        commandManager.executeCommand(new CameraMoveCommand(camera, posX->value(), newY, posZ->value()));
    });
    connect(posZ, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this, &camera, &commandManager, posX, posY](double newZ){
        commandManager.executeCommand(new CameraMoveCommand(camera, posX->value(), posY->value(), newZ));
    });

    // Rotation
    layout->addWidget(new QLabel("ROTATION"));
    QHBoxLayout *rotLayout = new QHBoxLayout();
    QDoubleSpinBox *rotX = new QDoubleSpinBox();
    QDoubleSpinBox *rotY = new QDoubleSpinBox();
    QDoubleSpinBox *rotZ = new QDoubleSpinBox();
    rotX->setPrefix("X: ");
    rotX->setSingleStep(0.1);
    rotY->setPrefix("Y: ");
    rotY->setSingleStep(0.1);
    rotZ->setPrefix("Z: ");
    rotZ->setSingleStep(0.1);
    rotX->setRange(-360, 360);
    rotY->setRange(-360, 360);
    rotZ->setRange(-360, 360);
    rotLayout->addWidget(rotX);
    rotLayout->addWidget(rotY);
    rotLayout->addWidget(rotZ);
    layout->addLayout(rotLayout);

    connect(rotX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this, &camera, &commandManager, rotY, rotZ](double newX){
        commandManager.executeCommand(new CameraRotationCommand(camera, newX, rotY->value(), rotZ->value()));
    });
    connect(rotY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this, &camera, &commandManager, rotX, rotZ](double newY){
        commandManager.executeCommand(new CameraRotationCommand(camera, rotX->value(), newY, rotZ->value()));
    });
    connect(rotZ, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this, &camera, &commandManager, rotX, rotY](double newZ){
        commandManager.executeCommand(new CameraRotationCommand(camera, rotX->value(), rotY->value(), newZ));
    });

    // FOV
    layout->addWidget(new QLabel("FOV"));
    QHBoxLayout *fovLayout = new QHBoxLayout();
    QSpinBox *fovSpin = new QSpinBox();
    fovSpin->setRange(1, 180);
    fovSpin->setValue(60);
    QCheckBox *focusCheck = new QCheckBox("FOCUS");
    fovLayout->addWidget(fovSpin);
    fovLayout->addWidget(focusCheck);
    layout->addLayout(fovLayout);

    // Only style the text color, inherit background from parent
    setStyleSheet("QLabel { color: white; }");
}