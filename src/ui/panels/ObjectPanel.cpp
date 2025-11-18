#include <CL/opencl.hpp>
#include "ObjectPanel.h"
#include "FPSChart.h"
#include "../RenderWidget.h"
#include "../../core/systems/KernelManager/KernelManager.h"
#include "../../core/systems/DeviceManager/DeviceManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QPixmap>
#include <QPainter>
#include <QFileDialog>
#include <QFrame>
#include <QFileInfo>
#include <QDebug>
#include <QMessageBox>
#include <fstream>
#include <vector>
#include "../../core/commands/actionsCommands/MoveShapeCommand.h"
#include "../../core/commands/actionsCommands/ScaleShapeCommand.h"
#include "../../core/commands/actionsCommands/RotateShapeCommand.h"
ObjectPanel::ObjectPanel(QWidget *parent) : QWidget(parent), fpsChart(nullptr), currentSelectedShapeID(-1), commandManager(CommandsManager::getInstance())
{
    setupUI();
}

void ObjectPanel::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(5);

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
    rotX->setSingleStep(0.1);
    rotY->setSingleStep(0.1);
    rotZ->setSingleStep(0.1);
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
    scaleX->setSingleStep(0.1);
    scaleY->setSingleStep(0.1);
    scaleZ->setSingleStep(0.1);
    scaleX->setValue(1);
    scaleY->setValue(1);
    scaleZ->setValue(1);
    scaleX->setMaximumWidth(90);
    scaleY->setMaximumWidth(90);
    scaleZ->setMaximumWidth(90);
    scaleLayout->addWidget(scaleX);
    scaleLayout->addWidget(scaleY);
    scaleLayout->addWidget(scaleZ);
    layout->addLayout(scaleLayout);

    // Texture
    layout->addWidget(new QLabel("TEXTURE"));

    // Texture preview block
    QFrame *texturePreviewFrame = new QFrame();
    texturePreviewFrame->setFrameStyle(QFrame::Box | QFrame::Raised);
    texturePreviewFrame->setMaximumHeight(100);
    texturePreviewFrame->setStyleSheet("QFrame { background-color: #2a2a2a; border: 2px solid #555; }");

    QVBoxLayout *previewLayout = new QVBoxLayout(texturePreviewFrame);
    previewLayout->setSpacing(5);
    previewLayout->setContentsMargins(5, 5, 5, 5);

    // Texture preview label (shows image or placeholder)
    QLabel *texturePreview = new QLabel();
    texturePreview->setMinimumHeight(60);
    texturePreview->setMaximumHeight(60);
    texturePreview->setScaledContents(false);
    texturePreview->setAlignment(Qt::AlignCenter);
    texturePreview->setStyleSheet("QLabel { background-color: #1a1a1a; border: 1px solid #333; color: #888; }");
    texturePreview->setText("No Texture\nClick to Load");

    // Create a default checkerboard pattern
    QPixmap checkerboard(64, 64);
    checkerboard.fill(Qt::gray);
    QPainter painter(&checkerboard);
    painter.fillRect(0, 0, 32, 32, Qt::darkGray);
    painter.fillRect(32, 32, 32, 32, Qt::darkGray);
    texturePreview->setPixmap(checkerboard);

    previewLayout->addWidget(texturePreview);

    // Texture controls layout
    QHBoxLayout *textureControlsLayout = new QHBoxLayout();

    // Load texture button
    QPushButton *loadTextureBtn = new QPushButton("Load");
    loadTextureBtn->setMaximumWidth(50);
    loadTextureBtn->setStyleSheet("QPushButton { background-color: #444; color: white; border: 1px solid #666; padding: 2px; }");

    // Clear texture button
    QPushButton *clearTextureBtn = new QPushButton("Clear");
    clearTextureBtn->setMaximumWidth(50);
    clearTextureBtn->setStyleSheet("QPushButton { background-color: #444; color: white; border: 1px solid #666; padding: 2px; }");

    // Texture name label
    QLabel *textureNameLabel = new QLabel("Default Pattern");
    textureNameLabel->setStyleSheet("QLabel { color: #ccc; font-size: 10px; }");

    textureControlsLayout->addWidget(loadTextureBtn);
    textureControlsLayout->addWidget(clearTextureBtn);
    textureControlsLayout->addWidget(textureNameLabel);
    textureControlsLayout->addStretch();

    previewLayout->addLayout(textureControlsLayout);

    // Connect buttons (basic functionality)
    connect(loadTextureBtn, &QPushButton::clicked, [texturePreview, textureNameLabel]()
            {
        QString fileName = QFileDialog::getOpenFileName(nullptr, "Load Texture", "", "Image Files (*.png *.jpg *.jpeg *.bmp *.tiff)");
        if (!fileName.isEmpty()) {
            QPixmap pixmap(fileName);
            if (!pixmap.isNull()) {
                // Scale to fit the preview area while maintaining aspect ratio
                QPixmap scaledPixmap = pixmap.scaled(texturePreview->width() - 2, texturePreview->height() - 2, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                texturePreview->setPixmap(scaledPixmap);
                QFileInfo fileInfo(fileName);
                textureNameLabel->setText(fileInfo.baseName());
            }
        } });

    connect(clearTextureBtn, &QPushButton::clicked, [texturePreview, textureNameLabel, checkerboard]()
            {
        texturePreview->setPixmap(checkerboard);
        textureNameLabel->setText("Default Pattern"); });

    layout->addWidget(texturePreviewFrame);

    // Material Properties
    layout->addWidget(new QLabel("MATERIAL PROPERTIES"));

    // Reflection
    QHBoxLayout *reflectionLayout = new QHBoxLayout();
    QLabel *reflectionLabel = new QLabel("METALLIC:");
    QDoubleSpinBox *reflectionSpinBox = new QDoubleSpinBox();
    reflectionSpinBox->setRange(0.0, 1.0);
    reflectionSpinBox->setSingleStep(0.01);
    reflectionSpinBox->setDecimals(2);
    reflectionSpinBox->setValue(0.0);
    reflectionSpinBox->setMaximumWidth(90);
    reflectionLayout->addWidget(reflectionLabel);
    reflectionLayout->addWidget(reflectionSpinBox);
    reflectionLayout->addStretch();
    layout->addLayout(reflectionLayout);

    // Refraction
    QHBoxLayout *refractionLayout = new QHBoxLayout();
    QLabel *refractionLabel = new QLabel("OPACITY:");
    QDoubleSpinBox *refractionSpinBox = new QDoubleSpinBox();
    refractionSpinBox->setRange(0.0, 1.0);
    refractionSpinBox->setSingleStep(0.01);
    refractionSpinBox->setDecimals(2);
    refractionSpinBox->setValue(0.0);
    refractionSpinBox->setMaximumWidth(90);
    refractionLayout->addWidget(refractionLabel);
    refractionLayout->addWidget(refractionSpinBox);
    refractionLayout->addStretch();
    layout->addLayout(refractionLayout);

    // Emissive
    QHBoxLayout *emissiveLayout = new QHBoxLayout();
    QLabel *emissiveLabel = new QLabel("EMISSIVE:");
    QSpinBox *emissiveSpinBox = new QSpinBox();
    emissiveSpinBox->setRange(0, 1000);
    emissiveSpinBox->setValue(0);
    emissiveSpinBox->setMaximumWidth(90);
    emissiveLayout->addWidget(emissiveLabel);
    emissiveLayout->addWidget(emissiveSpinBox);
    emissiveLayout->addStretch();
    layout->addLayout(emissiveLayout);

    // Refraction Index
    QHBoxLayout *refractionIndexLayout = new QHBoxLayout();
    QLabel *refractionIndexLabel = new QLabel("REFRACTION INDEX:");
    QDoubleSpinBox *refractionIndexSpinBox = new QDoubleSpinBox();
    refractionIndexSpinBox->setRange(0.0, 10.0);
    refractionIndexSpinBox->setSingleStep(0.001);
    refractionIndexSpinBox->setDecimals(3);
    refractionIndexSpinBox->setValue(1.000);
    refractionIndexSpinBox->setMaximumWidth(90);
    refractionIndexLayout->addWidget(refractionIndexLabel);
    refractionIndexLayout->addWidget(refractionIndexSpinBox);
    refractionIndexLayout->addStretch();
    layout->addLayout(refractionIndexLayout);

    // FPS Chart
    layout->addSpacing(15);
    layout->addWidget(new QLabel("FPS MONITOR"));
    fpsChart = new FPSChart();
    fpsChart->setMaxDataPoints(60); // Show last 60 FPS values
    layout->addWidget(fpsChart);

    // Only style the text color, inherit background from parent
    setStyleSheet("QLabel { color: white; }");

    // Connect position spin boxes changes to execute the correct command (Move command)
    connect(posX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double newX){
        commandManager.executeCommand(new MoveShapeCommand(currentSelectedShapeID, newX, posY->value(), posZ->value()));
    });

    connect(posY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double newY){
        commandManager.executeCommand(new MoveShapeCommand(currentSelectedShapeID, posX->value(), newY, posZ->value()));
    });

    connect(posZ, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double newZ){
        commandManager.executeCommand(new MoveShapeCommand(currentSelectedShapeID, posX->value(), posY->value(), newZ));
    });

    // Connection rotation spin boxes changes
    connect(rotX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double newX){
        commandManager.executeCommand(new RotateShapeCommand(currentSelectedShapeID, newX, rotY->value(), rotZ->value()));
    });

    connect(rotY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double newY){
        commandManager.executeCommand(new RotateShapeCommand(currentSelectedShapeID, rotX->value(), newY, rotZ->value()));
    });

    connect(rotZ, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double newZ){
        commandManager.executeCommand(new RotateShapeCommand(currentSelectedShapeID, rotX->value(), rotY->value(), newZ));
    });

    // Connection scale spin boxes changes
    connect(scaleX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double newX){
        commandManager.executeCommand(new ScaleShapeCommand(currentSelectedShapeID, newX, scaleY->value(), scaleZ->value()));
    });

    connect(scaleY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double newY){
        commandManager.executeCommand(new ScaleShapeCommand(currentSelectedShapeID, scaleX->value(), newY, scaleZ->value()));
    });

    connect(scaleZ, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double newZ){
        commandManager.executeCommand(new ScaleShapeCommand(currentSelectedShapeID, scaleX->value(), scaleY->value(), newZ));
    });

}

void ObjectPanel::setRenderWidget(RenderWidget *widget)
{
    if (widget && fpsChart)
    {
        // send the signal to update the FPS chart
        connect(widget, &RenderWidget::fpsUpdated, fpsChart, &FPSChart::addFPSValue);
    }
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
    Shape* shape = SceneManager::getInstance().getShapeByID(shapeID);
    
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
}


