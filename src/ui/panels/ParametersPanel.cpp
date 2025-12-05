#include "ParametersPanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include "../../core/camera/Camera.h"
#include "../../core/commands/CommandsManager.h"
#include "../../core/commands/actionsCommands/camera/CameraNbBouncesCommand.h"
#include "../../core/commands/actionsCommands/camera/CameraRPPCommand.h"

ParametersPanel::ParametersPanel(QWidget *parent) : QWidget(parent)
{
    setupUI();
}

void ParametersPanel::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(8);
    layout->setContentsMargins(5, 5, 5, 5);

    Camera &camera = Camera::getInstance();
    CommandsManager &commandManager = CommandsManager::getInstance();

    // Rays per pixel
    QHBoxLayout *raysLayout = new QHBoxLayout();
    QLabel *raysLabel = new QLabel("RAYS PER PIXEL");
    raysLabel->setStyleSheet("QLabel { font-size: 9px; }");
    raysSpin = new QSpinBox();
    raysSpin->setRange(1, 1000);
    raysSpin->setValue(camera.getRaysPerPixel());
    raysSpin->setMaximumWidth(60);
    raysLayout->addWidget(raysLabel);
    raysLayout->addWidget(raysSpin);
    layout->addLayout(raysLayout);

    // Max rebounds
    QHBoxLayout *reboundsLayout = new QHBoxLayout();
    QLabel *reboundsLabel = new QLabel("MAX REBOUNDS");
    reboundsLabel->setStyleSheet("QLabel { font-size: 9px; }");
    reboundsSpin = new QSpinBox();
    reboundsSpin->setRange(1, 100);
    reboundsSpin->setValue(camera.getNbBounces());
    reboundsSpin->setMaximumWidth(60);
    reboundsLayout->addWidget(reboundsLabel);
    reboundsLayout->addWidget(reboundsSpin);
    layout->addLayout(reboundsLayout);

    // Denoising
    QHBoxLayout *denoisingLayout = new QHBoxLayout();
    QLabel *denoisingLabel = new QLabel("DENOISING");
    denoisingLabel->setStyleSheet("QLabel { font-size: 9px; }");
    QCheckBox *denoisingCheck = new QCheckBox();
    denoisingLayout->addWidget(denoisingLabel);
    denoisingLayout->addWidget(denoisingCheck);
    denoisingLayout->addStretch();
    layout->addLayout(denoisingLayout);

    QComboBox *bufferOptions = new QComboBox();
    bufferOptions->addItem("Final Image");
    bufferOptions->addItem("Albedo");
    bufferOptions->addItem("Depth");
    bufferOptions->addItem("Normals");

    QPushButton *screenShotButton = new QPushButton("", this);
    screenShotButton->setIcon(QIcon("assets/app/icons/iconScreenshot.png"));
    layout->addWidget(screenShotButton);

    connect(reboundsSpin, QOverload<int>::of(&QSpinBox::valueChanged), [this, &camera, &commandManager](int newBounces)
            { commandManager.executeCommand(new CameraNbBouncesCommand(camera, newBounces)); });

    connect(raysSpin, QOverload<int>::of(&QSpinBox::valueChanged), [this, &camera, &commandManager](int newRPP)
            { commandManager.executeCommand(new CameraRPPCommand(camera, newRPP)); });
    connect(bufferOptions, QOverload<int>::of(&QComboBox::currentIndexChanged), [&camera](int index)
            { camera.setBufferType(index); });

    connect(&camera, &Camera::bufferTypeChanged, [bufferOptions](int type)
            {
        bufferOptions->blockSignals(true);
        bufferOptions->setCurrentIndex(type);
        bufferOptions->blockSignals(false); });

    connect(screenShotButton, &QPushButton::clicked, this, &ParametersPanel::screenshotButtonClicked);

    layout->addWidget(bufferOptions);

    connect(&camera, &Camera::nbBouncesChanged, this, &ParametersPanel::onCameraNBouncesChanged);
    connect(&camera, &Camera::raysPerPixelChanged, this, &ParametersPanel::onCameraRaysPerPixelChanged);

    // Don't set background style on the widget itself - let it inherit from parent
    setStyleSheet("QLabel { color: white; }");
}

void ParametersPanel::onCameraNBouncesChanged(int bounces)
{
    reboundsSpin->blockSignals(true);
    reboundsSpin->setValue(bounces);
    reboundsSpin->blockSignals(false);
}

void ParametersPanel::onCameraRaysPerPixelChanged(int rpp)
{
    raysSpin->blockSignals(true);
    raysSpin->setValue(rpp);
    raysSpin->blockSignals(false);
}