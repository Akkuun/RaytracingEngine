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
#include <QPixmap>
#include <QPainter>
#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>
#include <QMessageBox>
#include <fstream>
#include <vector>
#include "../../core/commands/actionsCommands/MoveShapeCommand.h"
#include "../../core/commands/actionsCommands/ScaleShapeCommand.h"
#include "../../core/commands/actionsCommands/RotateShapeCommand.h"
#include "../../core/commands/actionsCommands/SetTextureShape.h"
#include "../../core/commands/actionsCommands/ClearTextureShape.h"
#include "../../core/systems/SceneManager/SceneManager.h"
#include <QKeyEvent>

ObjectPanel::ObjectPanel(QWidget *parent) : QWidget(parent), fpsChart(nullptr), currentSelectedShapeID(SceneManager::getInstance().getShapes().front()->getID()), commandManager(CommandsManager::getInstance())
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

    // Texture
    layout->addWidget(new QLabel("TEXTURE"));

    // Texture preview block
    texturePreviewFrame = new QFrame();
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
    loadTextureBtn = new QPushButton("Load");
    loadTextureBtn->setMaximumWidth(50);
    loadTextureBtn->setStyleSheet("QPushButton { background-color: #444; color: white; border: 1px solid #666; padding: 2px; }");

    // Clear texture button
    clearTextureBtn = new QPushButton("Clear");
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
    connect(loadTextureBtn, &QPushButton::clicked, [this, texturePreview, textureNameLabel]()
    {
        QString fileName = QFileDialog::getOpenFileName(nullptr, "Load Texture", "", "Image Files (*.ppm)");
        if (!fileName.isEmpty()) {
            QPixmap pixmap(fileName);
            if (!pixmap.isNull()) {
                // Scale to fit the preview area while maintaining aspect ratio
                QPixmap scaledPixmap = pixmap.scaled(texturePreview->width() - 2, texturePreview->height() - 2, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                texturePreview->setPixmap(scaledPixmap);
                QFileInfo fileInfo(fileName);
                textureNameLabel->setText(fileInfo.baseName());

                ppmLoader::ImageRGB image;
                ppmLoader::load_ppm(image, fileName.toStdString());
                commandManager.executeCommand(new SetTextureShape(SceneManager::getInstance().getShapeByID(currentSelectedShapeID), image));
            }
        }
    });

    connect(clearTextureBtn, &QPushButton::clicked, [this, texturePreview, textureNameLabel, checkerboard]()
    {
        commandManager.executeCommand(new ClearTextureShape(SceneManager::getInstance().getShapeByID(currentSelectedShapeID)));
    });

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
    reflectionSpinBox = new QDoubleSpinBox();
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
    refractionSpinBox = new QDoubleSpinBox();
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
    emissiveSpinBox = new QSpinBox();
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
    refractionIndexSpinBox = new QDoubleSpinBox();
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

    // Connect material spin boxes
    connect(reflectionSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        Shape *shape = SceneManager::getInstance().getShapeByID(currentSelectedShapeID);
        if (shape) {
            Material *mat = shape->getMaterial();
            if (mat) {
                mat->setSpecular(vec3(value, value, value));
                emit materialChanged();
            }
        }
    });

    connect(refractionSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        Shape *shape = SceneManager::getInstance().getShapeByID(currentSelectedShapeID);
        if (shape) {
            Material *mat = shape->getMaterial();
            if (mat) {
                mat->setTransparency(value);
                emit materialChanged();
            }
        }
    });

    connect(emissiveSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
        Shape *shape = SceneManager::getInstance().getShapeByID(currentSelectedShapeID);
        if (shape) {
            Material *mat = shape->getMaterial();
            if (mat) {
                mat->setLightIntensity(value);
                mat->setEmissive(value > 0);
                emit materialChanged();
            }
        }
    });

    connect(refractionIndexSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        Shape *shape = SceneManager::getInstance().getShapeByID(currentSelectedShapeID);
        if (shape) {
            Material *mat = shape->getMaterial();
            if (mat) {
                mat->setIndexMedium(value);
                emit materialChanged();
            }
        }
    });
}

void ObjectPanel::setRenderWidget(RenderWidget *widget)
{
    if (widget && fpsChart)
    {
        // send the signal to update the FPS chart
        connect(widget, &RenderWidget::fpsUpdated, fpsChart, &FPSChart::addFPSValue);
        connect(this, &ObjectPanel::materialChanged, widget, &RenderWidget::markMaterialDirty);
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

    // Update material properties
    reflectionSpinBox->blockSignals(true);
    refractionSpinBox->blockSignals(true);
    emissiveSpinBox->blockSignals(true);
    refractionIndexSpinBox->blockSignals(true);

    Material *mat = shape->getMaterial();
    if (mat) {
        reflectionSpinBox->setValue(mat->getSpecular().x);
        refractionSpinBox->setValue(mat->getTransparency());
        emissiveSpinBox->setValue(mat->getLightIntensity());
        refractionIndexSpinBox->setValue(mat->getIndexMedium());
    }

    reflectionSpinBox->blockSignals(false);
    refractionSpinBox->blockSignals(false);
    emissiveSpinBox->blockSignals(false);
    refractionIndexSpinBox->blockSignals(false);

    // Update texture
    onTextureSelectionChanged(shape->getMaterial());
}

void ObjectPanel::onTextureSelectionChanged(const Material *material)
{
    if (material != nullptr) {
        QImage image(reinterpret_cast<const uchar*>(material->getImage().data.data()), material->getImage().w, material->getImage().h, QImage::Format_RGB888);
        if (!image.isNull()) {
            QPixmap pixmap = QPixmap::fromImage(image);
            texturePreviewFrame->findChild<QLabel*>()->setPixmap(pixmap.scaled(texturePreviewFrame->width() - 2, texturePreviewFrame->height() - 2, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            return;
        }
    }
    // If image is null, set to default checkerboard
    QPixmap checkerboard(64, 64);
    checkerboard.fill(Qt::gray);
    QPainter painter(&checkerboard);
    painter.fillRect(0, 0, 32, 32, Qt::darkGray);
    painter.fillRect(32, 32, 32, 32, Qt::darkGray);
    texturePreviewFrame->findChild<QLabel*>()->setPixmap(checkerboard);
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
