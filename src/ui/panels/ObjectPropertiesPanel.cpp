#include "ObjectPropertiesPanel.h"
#include "../RenderWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QPixmap>
#include <QPainter>
#include <QFileDialog>
#include <QFileInfo>
#include "../../core/commands/actionsCommands/materials/SetTextureShape.h"
#include "../../core/commands/actionsCommands/materials/SetNormalShape.h"
#include "../../core/commands/actionsCommands/materials/SetMetallicShape.h"
#include "../../core/commands/actionsCommands/materials/ClearTextureShape.h"
#include "../../core/commands/actionsCommands/materials/ClearNormalShape.h"
#include "../../core/commands/actionsCommands/materials/ClearMetallicShape.h"
#include "../../core/commands/actionsCommands/materials/MaterialTransparencyCommand.h"
#include "../../core/commands/actionsCommands/materials/MaterialIORCommand.h"
#include "../../core/commands/actionsCommands/materials/MaterialMetalnessCommand.h"
#include "../../core/commands/actionsCommands/materials/MaterialEmissiveCommand.h"
#include "../../core/commands/actionsCommands/materials/MaterialDiffuseColorCommand.h"
#include "../../core/systems/SceneManager/SceneManager.h"
#include "./CustomDoubleSpinBox.h"

ObjectPropertiesPanel::ObjectPropertiesPanel(QWidget *parent) : QWidget(parent), currentSelectedShapeID(SceneManager::getInstance().getShapes().front()->getID()), commandManager(CommandsManager::getInstance())
{
    setupUI();

    // Register callback for material changes (undo/redo)
    commandManager.addMaterialChangedCallback([this]()
                                              { this->onMaterialChanged(); });
}

void ObjectPropertiesPanel::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(5);

    // Diffuse
    layout->addWidget(new QLabel("DIFFUSE"));

    colorFrame = new QFrame();
    colorFrame->setFrameStyle(QFrame::Box | QFrame::Raised);
    colorFrame->setMaximumHeight(100);
    colorFrame->setMinimumHeight(100);
    colorFrame->setStyleSheet("QFrame { background-color: #2a2a2a; border: 2px solid #555; }");

    QVBoxLayout *colorLayout = new QVBoxLayout(colorFrame);
    colorLayout->setSpacing(5);
    colorLayout->setContentsMargins(5, 5, 5, 5);

    colorPreview = new QLabel();
    colorPreview->setMinimumHeight(60);
    colorPreview->setMaximumHeight(60);
    colorPreview->setScaledContents(false);
    colorPreview->setAlignment(Qt::AlignCenter);
    colorPreview->setStyleSheet("QLabel { background-color: #1a1a1a; border: 1px solid #333; color: #888; }");

    colorLayout->addWidget(colorPreview);

    QHBoxLayout *colorControlsLayout = new QHBoxLayout();
    // Red spinbox
    redSpinBox = new CustomDoubleSpinBox();
    redSpinBox->setRange(0, 255);
    redSpinBox->setPrefix("R: ");
    redSpinBox->setValue(255);
    redSpinBox->setFocusPolicy(Qt::ClickFocus);
    redSpinBox->installEventFilter(this);

    // Green spinbox
    greenSpinBox = new CustomDoubleSpinBox();
    greenSpinBox->setRange(0, 255);
    greenSpinBox->setPrefix("G: ");
    greenSpinBox->setValue(255);
    greenSpinBox->setFocusPolicy(Qt::ClickFocus);
    greenSpinBox->installEventFilter(this);

    // Blue spinbox
    blueSpinBox = new CustomDoubleSpinBox();
    blueSpinBox->setRange(0, 255);
    blueSpinBox->setPrefix("B: ");
    blueSpinBox->setValue(255);
    blueSpinBox->setFocusPolicy(Qt::ClickFocus);
    blueSpinBox->installEventFilter(this);

    connect(redSpinBox, QOverload<double>::of(&CustomDoubleSpinBox::valueChanged), this, &ObjectPropertiesPanel::RGBChanged);
    connect(greenSpinBox, QOverload<double>::of(&CustomDoubleSpinBox::valueChanged), this, &ObjectPropertiesPanel::RGBChanged);
    connect(blueSpinBox, QOverload<double>::of(&CustomDoubleSpinBox::valueChanged), this, &ObjectPropertiesPanel::RGBChanged);

    colorControlsLayout->addWidget(redSpinBox);
    colorControlsLayout->addWidget(greenSpinBox);
    colorControlsLayout->addWidget(blueSpinBox);

    colorLayout->addLayout(colorControlsLayout);

    layout->addWidget(colorFrame);

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
                commandManager.executeCommand(new SetTextureShape(SceneManager::getInstance().getShapeByID(currentSelectedShapeID), image, fileName.toStdString()));
            }
        } });

    connect(clearTextureBtn, &QPushButton::clicked, [this, texturePreview, textureNameLabel, checkerboard]()
            { commandManager.executeCommand(new ClearTextureShape(SceneManager::getInstance().getShapeByID(currentSelectedShapeID))); });

    connect(clearTextureBtn, &QPushButton::clicked, [texturePreview, textureNameLabel, checkerboard]()
            {
        texturePreview->setPixmap(checkerboard);
        textureNameLabel->setText("Default Pattern"); });

    layout->addWidget(texturePreviewFrame);

    // Normal map
    layout->addWidget(new QLabel("NORMAL MAP"));

    // Normal preview block
    normalPreviewFrame = new QFrame();
    normalPreviewFrame->setFrameStyle(QFrame::Box | QFrame::Raised);
    normalPreviewFrame->setMaximumHeight(100);
    normalPreviewFrame->setStyleSheet("QFrame { background-color: #2a2a2a; border: 2px solid #555; }");

    QVBoxLayout *normalPreviewLayout = new QVBoxLayout(normalPreviewFrame);
    normalPreviewLayout->setSpacing(5);
    normalPreviewLayout->setContentsMargins(5, 5, 5, 5);

    // Texture preview label (shows image or placeholder)
    QLabel *normalPreview = new QLabel();
    normalPreview->setMinimumHeight(60);
    normalPreview->setMaximumHeight(60);
    normalPreview->setScaledContents(false);
    normalPreview->setAlignment(Qt::AlignCenter);
    normalPreview->setStyleSheet("QLabel { background-color: #1a1a1a; border: 1px solid #333; color: #888; }");
    normalPreview->setText("No Texture\nClick to Load");

    // Create a default full #8080ff image
    QPixmap flatnormal(64, 64);
    flatnormal.fill(QColor(128, 128, 255));
    normalPreview->setPixmap(flatnormal);

    normalPreviewLayout->addWidget(normalPreview);

    // Texture controls layout
    QHBoxLayout *normalControlsLayout = new QHBoxLayout();

    // Load texture button
    loadNormalBtn = new QPushButton("Load");
    loadNormalBtn->setMaximumWidth(50);
    loadNormalBtn->setStyleSheet("QPushButton { background-color: #444; color: white; border: 1px solid #666; padding: 2px; }");

    // Clear texture button
    clearNormalBtn = new QPushButton("Clear");
    clearNormalBtn->setMaximumWidth(50);
    clearNormalBtn->setStyleSheet("QPushButton { background-color: #444; color: white; border: 1px solid #666; padding: 2px; }");

    // Texture name label
    QLabel *normalNameLabel = new QLabel("Default Pattern");
    normalNameLabel->setStyleSheet("QLabel { color: #ccc; font-size: 10px; }");
    normalControlsLayout->addWidget(loadNormalBtn);
    normalControlsLayout->addWidget(clearNormalBtn);
    normalControlsLayout->addWidget(normalNameLabel);
    normalControlsLayout->addStretch();
    normalPreviewLayout->addLayout(normalControlsLayout);

    // Connect buttons (basic functionality)
    connect(loadNormalBtn, &QPushButton::clicked, [this, normalPreview, normalNameLabel]()
            {
        QFileDialog dialog(nullptr, "Load Texture", "", "Image Files (*.ppm)");
        dialog.setOption(QFileDialog::DontUseNativeDialog, true);
        QString fileName;
        if (dialog.exec()) {
            fileName = dialog.selectedFiles().first();
        }
        if (!fileName.isEmpty()) {
            QPixmap pixmap(fileName);
            if (!pixmap.isNull()) {
                // Scale to fit the preview area while maintaining aspect ratio
                QPixmap scaledPixmap = pixmap.scaled(normalPreview->width() - 2, normalPreview->height() - 2, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                normalPreview->setPixmap(scaledPixmap);
                QFileInfo fileInfo(fileName);
                normalNameLabel->setText(fileInfo.baseName());

                ppmLoader::ImageRGB image;
                ppmLoader::load_ppm(image, fileName.toStdString());
                commandManager.executeCommand(new SetNormalShape(SceneManager::getInstance().getShapeByID(currentSelectedShapeID), image, fileName.toStdString()));
            }
        } });

    connect(clearNormalBtn, &QPushButton::clicked, [this, normalPreview, normalNameLabel, flatnormal]()
            { commandManager.executeCommand(new ClearNormalShape(SceneManager::getInstance().getShapeByID(currentSelectedShapeID))); });

    connect(clearNormalBtn, &QPushButton::clicked, [normalPreview, normalNameLabel, flatnormal]()
            {
        normalPreview->setPixmap(flatnormal);
        normalNameLabel->setText("Default Pattern"); });

    layout->addWidget(normalPreviewFrame);

    // Reflection / metallicity
    QHBoxLayout *reflectionLayout = new QHBoxLayout();
    QLabel *reflectionLabel = new QLabel("METALLIC:");
    reflectionSpinBox = new CustomDoubleSpinBox();
    reflectionSpinBox->setRange(0.0, 1.0);
    reflectionSpinBox->setSingleStep(0.01);
    reflectionSpinBox->setDecimals(2);
    reflectionSpinBox->setValue(0.0);
    reflectionSpinBox->setMaximumWidth(90);
    reflectionLayout->addWidget(reflectionLabel);
    reflectionLayout->addWidget(reflectionSpinBox);
    reflectionLayout->addStretch();
    layout->addLayout(reflectionLayout);

    // Metallicity preview block
    metalPreviewFrame = new QFrame();
    metalPreviewFrame->setFrameStyle(QFrame::Box | QFrame::Raised);
    metalPreviewFrame->setMaximumHeight(100);
    metalPreviewFrame->setStyleSheet("QFrame { background-color: #2a2a2a; border: 2px solid #555; }");

    QVBoxLayout *metalPreviewLayout = new QVBoxLayout(metalPreviewFrame);
    metalPreviewLayout->setSpacing(5);
    metalPreviewLayout->setContentsMargins(5, 5, 5, 5);

    // Texture preview label (shows image or placeholder)
    QLabel *metalPreview = new QLabel();
    metalPreview->setMinimumHeight(60);
    metalPreview->setMaximumHeight(60);
    metalPreview->setScaledContents(false);
    metalPreview->setAlignment(Qt::AlignCenter);
    metalPreview->setStyleSheet("QLabel { background-color: #1a1a1a; border: 1px solid #333; color: #888; }");
    metalPreview->setText("No Texture\nClick to Load");

    // Create a default full black image
    QPixmap blackimage(64, 64);
    blackimage.fill(Qt::black);
    metalPreview->setPixmap(blackimage);

    metalPreviewLayout->addWidget(metalPreview);

    // Texture controls layout
    QHBoxLayout *metalControlsLayout = new QHBoxLayout();

    // Load texture button
    loadMetalBtn = new QPushButton("Load");
    loadMetalBtn->setMaximumWidth(50);
    loadMetalBtn->setStyleSheet("QPushButton { background-color: #444; color: white; border: 1px solid #666; padding: 2px; }");

    // Clear texture button
    clearMetalBtn = new QPushButton("Clear");
    clearMetalBtn->setMaximumWidth(50);
    clearMetalBtn->setStyleSheet("QPushButton { background-color: #444; color: white; border: 1px solid #666; padding: 2px; }");

    // Texture name label
    QLabel *metalNameLabel = new QLabel("Default Pattern");
    metalNameLabel->setStyleSheet("QLabel { color: #ccc; font-size: 10px; }");

    metalControlsLayout->addWidget(loadMetalBtn);
    metalControlsLayout->addWidget(clearMetalBtn);
    metalControlsLayout->addWidget(metalNameLabel);
    metalControlsLayout->addStretch();

    metalPreviewLayout->addLayout(metalControlsLayout);

    // Connect buttons (basic functionality)
    connect(loadMetalBtn, &QPushButton::clicked, [this, metalPreview, metalNameLabel]()
            {
        QString fileName = QFileDialog::getOpenFileName(nullptr, "Load Texture", "", "Image Files (*.ppm)");
        if (!fileName.isEmpty()) {
            QPixmap pixmap(fileName);
            if (!pixmap.isNull()) {
                // Scale to fit the preview area while maintaining aspect ratio
                QPixmap scaledPixmap = pixmap.scaled(metalPreview->width() - 2, metalPreview->height() - 2, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                metalPreview->setPixmap(scaledPixmap);
                QFileInfo fileInfo(fileName);
                metalNameLabel->setText(fileInfo.baseName());

                ppmLoader::ImageRGB image;
                ppmLoader::load_ppm(image, fileName.toStdString());
                commandManager.executeCommand(new SetMetallicShape(SceneManager::getInstance().getShapeByID(currentSelectedShapeID), image, fileName.toStdString()));
            }
        } });

    connect(clearMetalBtn, &QPushButton::clicked, [this, metalPreview, metalNameLabel, blackimage]()
            { commandManager.executeCommand(new ClearMetallicShape(SceneManager::getInstance().getShapeByID(currentSelectedShapeID))); });

    connect(clearMetalBtn, &QPushButton::clicked, [metalPreview, metalNameLabel, blackimage]()
            {
        metalPreview->setPixmap(blackimage);
        metalNameLabel->setText("Default Pattern"); });

    layout->addWidget(metalPreviewFrame);

    // Emissive
    QHBoxLayout *emissiveLayout = new QHBoxLayout();
    QLabel *emissiveLabel = new QLabel("EMISSIVE:");
    emissiveSpinBox = new QSpinBox();
    emissiveSpinBox->setRange(1, 1000);
    emissiveSpinBox->setValue(1);
    emissiveSpinBox->setMaximumWidth(90);
    emissiveLayout->addWidget(emissiveLabel);
    emissiveLayout->addWidget(emissiveSpinBox);
    emissiveLayout->addStretch();
    layout->addLayout(emissiveLayout);

    // Emissive preview block
    emissivePreviewFrame = new QFrame();
    emissivePreviewFrame->setFrameStyle(QFrame::Box | QFrame::Raised);
    emissivePreviewFrame->setMaximumHeight(100);
    emissivePreviewFrame->setStyleSheet("QFrame { background-color: #2a2a2a; border: 2px solid #555; }");

    QVBoxLayout *emissivePreviewLayout = new QVBoxLayout(emissivePreviewFrame);
    emissivePreviewLayout->setSpacing(5);
    emissivePreviewLayout->setContentsMargins(5, 5, 5, 5);

    // Texture preview label (shows image or placeholder)
    QLabel *emissivePreview = new QLabel();
    emissivePreview->setMinimumHeight(60);
    emissivePreview->setMaximumHeight(60);
    emissivePreview->setScaledContents(false);
    emissivePreview->setAlignment(Qt::AlignCenter);
    emissivePreview->setStyleSheet("QLabel { background-color: #1a1a1a; border: 1px solid #333; color: #888; }");
    emissivePreview->setText("No Texture\nClick to Load");

    emissivePreview->setPixmap(blackimage);

    emissivePreviewLayout->addWidget(emissivePreview);

    // Texture controls layout
    QHBoxLayout *emissiveControlsLayout = new QHBoxLayout();

    // Load texture button
    loadEmissiveBtn = new QPushButton("Load");
    loadEmissiveBtn->setMaximumWidth(50);
    loadEmissiveBtn->setStyleSheet("QPushButton { background-color: #444; color: white; border: 1px solid #666; padding: 2px; }");

    // Clear texture button
    clearEmissiveBtn = new QPushButton("Clear");
    clearEmissiveBtn->setMaximumWidth(50);
    clearEmissiveBtn->setStyleSheet("QPushButton { background-color: #444; color: white; border: 1px solid #666; padding: 2px; }");

    // Texture name label
    QLabel *emissiveNameLabel = new QLabel("Default Pattern");
    emissiveNameLabel->setStyleSheet("QLabel { color: #ccc; font-size: 10px; }");
    emissiveControlsLayout->addWidget(loadEmissiveBtn);
    emissiveControlsLayout->addWidget(clearEmissiveBtn);
    emissiveControlsLayout->addWidget(emissiveNameLabel);
    emissiveControlsLayout->addStretch();
    emissivePreviewLayout->addLayout(emissiveControlsLayout);
    // Connect buttons (basic functionality)
    connect(loadEmissiveBtn, &QPushButton::clicked, [this, emissivePreview, emissiveNameLabel]()
            {
        QString fileName = QFileDialog::getOpenFileName(nullptr, "Load Texture", "", "Image Files (*.ppm)");
        if (!fileName.isEmpty()) {
            QPixmap pixmap(fileName);
            if (!pixmap.isNull()) {
                // Scale to fit the preview area while maintaining aspect ratio
                QPixmap scaledPixmap = pixmap.scaled(emissivePreview->width() - 2, emissivePreview->height() - 2, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                emissivePreview->setPixmap(scaledPixmap);
                QFileInfo fileInfo(fileName);
                emissiveNameLabel->setText(fileInfo.baseName());

                ppmLoader::ImageRGB image;
                ppmLoader::load_ppm(image, fileName.toStdString());
               // commandManager.executeCommand(new SetTextureShape(SceneManager::getInstance().getShapeByID(currentSelectedShapeID), image));
                // TODO CREATE EMISSIVE MAP COMMANDS
            }
        } });

    connect(clearEmissiveBtn, &QPushButton::clicked, [this, emissivePreview, emissiveNameLabel, blackimage]()
            {
                // commandManager.executeCommand(new ClearTextureShape(SceneManager::getInstance().getShapeByID(currentSelectedShapeID)));
                // TODO CREATE EMISSIVE MAP COMMANDS
            });

    connect(clearEmissiveBtn, &QPushButton::clicked, [emissivePreview, emissiveNameLabel, blackimage]()
            {
        emissivePreview->setPixmap(blackimage);
        emissiveNameLabel->setText("Default Pattern"); });

    layout->addWidget(emissivePreviewFrame);

    // Refraction
    QHBoxLayout *refractionLayout = new QHBoxLayout();
    QLabel *refractionLabel = new QLabel("OPACITY:");
    refractionSpinBox = new CustomDoubleSpinBox();
    refractionSpinBox->setRange(0.0, 1.0);
    refractionSpinBox->setSingleStep(0.01);
    refractionSpinBox->setDecimals(2);
    refractionSpinBox->setValue(0.0);
    refractionSpinBox->setMaximumWidth(90);
    refractionLayout->addWidget(refractionLabel);
    refractionLayout->addWidget(refractionSpinBox);
    refractionLayout->addStretch();
    layout->addLayout(refractionLayout);

    // Refraction Index
    QHBoxLayout *refractionIndexLayout = new QHBoxLayout();
    QLabel *refractionIndexLabel = new QLabel("REFRACTION INDEX:");
    refractionIndexSpinBox = new CustomDoubleSpinBox();
    refractionIndexSpinBox->setRange(0.0, 10.0);
    refractionIndexSpinBox->setSingleStep(0.001);
    refractionIndexSpinBox->setDecimals(3);
    refractionIndexSpinBox->setValue(1.000);
    refractionIndexSpinBox->setMaximumWidth(90);
    refractionIndexLayout->addWidget(refractionIndexLabel);
    refractionIndexLayout->addWidget(refractionIndexSpinBox);
    refractionIndexLayout->addStretch();
    layout->addLayout(refractionIndexLayout);

    // Only style the text color, inherit background from parent
    setStyleSheet("QLabel { color: white; }");

    // Connect material spin boxes
    connect(reflectionSpinBox, QOverload<double>::of(&CustomDoubleSpinBox::valueChanged), [this](double value)
            {
        Shape *shape = SceneManager::getInstance().getShapeByID(currentSelectedShapeID);
        if (shape) {
            Material *mat = shape->getMaterial();
            if (mat) {
                mat->setMetalness(value);
                CommandsManager::getInstance().executeCommand(new MaterialMetalnessCommand(*mat, value));
            }
        } });

    connect(refractionSpinBox, QOverload<double>::of(&CustomDoubleSpinBox::valueChanged), [this](double value)
            {
        Shape *shape = SceneManager::getInstance().getShapeByID(currentSelectedShapeID);
        if (shape) {
            Material *mat = shape->getMaterial();
            if (mat) {
                mat->setTransparency(value);
                CommandsManager::getInstance().executeCommand(new MaterialTransparencyCommand(*mat, value));
            }
        } });

    connect(emissiveSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value)
            {
        Shape *shape = SceneManager::getInstance().getShapeByID(currentSelectedShapeID);
        if (shape) {
            Material *mat = shape->getMaterial();
            if (mat) {
                CommandsManager::getInstance().executeCommand(new MaterialEmissiveCommand(*mat, value));
            }
        } });

    connect(refractionIndexSpinBox, QOverload<double>::of(&CustomDoubleSpinBox::valueChanged), [this](double value)
            {
        Shape *shape = SceneManager::getInstance().getShapeByID(currentSelectedShapeID);
        if (shape) {
            Material *mat = shape->getMaterial();
            if (mat) {
                mat->setIndexMedium(value);
                CommandsManager::getInstance().executeCommand(new MaterialIORCommand(*mat, value));
            }
        } });
}

void ObjectPropertiesPanel::setRenderWidget(RenderWidget *widget)
{
    if (widget)
    {
        // send the signal to update the FPS chart
        connect(this, &ObjectPropertiesPanel::materialChanged, widget, &RenderWidget::markMaterialDirty);
    }
}
// function to update the current selected shape properties in the panel
void ObjectPropertiesPanel::onShapeSelectionChanged(int shapeID)
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

    // Update material properties
    reflectionSpinBox->blockSignals(true);
    refractionSpinBox->blockSignals(true);
    emissiveSpinBox->blockSignals(true);
    refractionIndexSpinBox->blockSignals(true);

    Material *mat = shape->getMaterial();
    if (mat)
    {
        reflectionSpinBox->setValue(mat->getMetalness());
        refractionSpinBox->setValue(mat->getTransparency());
        emissiveSpinBox->setValue(mat->getLightIntensity());
        refractionIndexSpinBox->setValue(mat->getIndexMedium());

        colorPreview->setStyleSheet(QString("QLabel { background-color: rgb(%1, %2, %3); border: 1px solid #333; color: #888; }")
                                        .arg(static_cast<int>(shape->getMaterial()->getDiffuse().x * 255))
                                        .arg(static_cast<int>(shape->getMaterial()->getDiffuse().y * 255))
                                        .arg(static_cast<int>(shape->getMaterial()->getDiffuse().z * 255)));

        redSpinBox->blockSignals(true);
        greenSpinBox->blockSignals(true);
        blueSpinBox->blockSignals(true);

        redSpinBox->setValue(static_cast<int>(shape->getMaterial()->getDiffuse().x * 255));
        greenSpinBox->setValue(static_cast<int>(shape->getMaterial()->getDiffuse().y * 255));
        blueSpinBox->setValue(static_cast<int>(shape->getMaterial()->getDiffuse().z * 255));

        redSpinBox->blockSignals(false);
        greenSpinBox->blockSignals(false);
        blueSpinBox->blockSignals(false);
    }
    else
    {
        colorPreview->setStyleSheet("QLabel { background-color: rgb(255, 255, 255); border: 1px solid #333; color: #888; }");

        redSpinBox->blockSignals(true);
        greenSpinBox->blockSignals(true);
        blueSpinBox->blockSignals(true);

        redSpinBox->setValue(255);
        greenSpinBox->setValue(255);
        blueSpinBox->setValue(255);

        redSpinBox->blockSignals(false);
        greenSpinBox->blockSignals(false);
        blueSpinBox->blockSignals(false);
    }

    reflectionSpinBox->blockSignals(false);
    refractionSpinBox->blockSignals(false);
    emissiveSpinBox->blockSignals(false);
    refractionIndexSpinBox->blockSignals(false);

    // Update texture
    onTextureSelectionChanged(mat);
}

void defaultTexturePreview(QFrame *frame)
{
    QPixmap checkerboard(64, 64);
    checkerboard.fill(Qt::gray);
    QPainter painter(&checkerboard);
    painter.fillRect(0, 0, 32, 32, Qt::darkGray);
    painter.fillRect(32, 32, 32, 32, Qt::darkGray);
    frame->findChild<QLabel *>()->setPixmap(checkerboard);
}

void defaultNormalPreview(QFrame *frame)
{
    QPixmap flatnormal(64, 64);
    flatnormal.fill(QColor(128, 128, 255));
    frame->findChild<QLabel *>()->setPixmap(flatnormal);
}

void defaultBlackPreview(QFrame *frame)
{
    QPixmap blackimage(64, 64);
    blackimage.fill(Qt::black);
    frame->findChild<QLabel *>()->setPixmap(blackimage);
}

void ObjectPropertiesPanel::onTextureSelectionChanged(const Material *material)
{
    if (material != nullptr)
    {
        QImage image(reinterpret_cast<const uchar *>(material->getImage().data.data()), material->getImage().w, material->getImage().h, QImage::Format_RGB888);
        if (!image.isNull())
        {
            QPixmap pixmap = QPixmap::fromImage(image);
            texturePreviewFrame->findChild<QLabel *>()->setPixmap(pixmap.scaled(texturePreviewFrame->width() - 2, texturePreviewFrame->height() - 2, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
        else
        {
            defaultTexturePreview(texturePreviewFrame);
        }

        QImage normalimage(reinterpret_cast<const uchar *>(material->getNormals().data.data()), material->getNormals().w, material->getNormals().h, QImage::Format_RGB888);
        if (!normalimage.isNull())
        {
            QPixmap pixmap = QPixmap::fromImage(normalimage);
            normalPreviewFrame->findChild<QLabel *>()->setPixmap(pixmap.scaled(normalPreviewFrame->width() - 2, normalPreviewFrame->height() - 2, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
        else
        {
            defaultNormalPreview(normalPreviewFrame);
        }

        QImage metallicImage(reinterpret_cast<const uchar *>(material->getMetallic().data.data()), material->getMetallic().w, material->getMetallic().h, QImage::Format_RGB888);
        if (!metallicImage.isNull())
        {
            QPixmap pixmap = QPixmap::fromImage(metallicImage);
            metalPreviewFrame->findChild<QLabel *>()->setPixmap(pixmap.scaled(metalPreviewFrame->width() - 2, metalPreviewFrame->height() - 2, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
        else
        {
            defaultBlackPreview(metalPreviewFrame);
        }

        QImage emissiveImage(reinterpret_cast<const uchar *>(material->getEmissive().data.data()), material->getEmissive().w, material->getEmissive().h, QImage::Format_RGB888);
        if (!emissiveImage.isNull())
        {
            QPixmap pixmap = QPixmap::fromImage(emissiveImage);
            emissivePreviewFrame->findChild<QLabel *>()->setPixmap(pixmap.scaled(emissivePreviewFrame->width() - 2, emissivePreviewFrame->height() - 2, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
        else
        {
            defaultBlackPreview(emissivePreviewFrame);
        }
    }
    else
    {
        // If material is null, set to defaults
        defaultTexturePreview(texturePreviewFrame);
        defaultNormalPreview(normalPreviewFrame);
        defaultBlackPreview(metalPreviewFrame);
        defaultBlackPreview(emissivePreviewFrame);
    }
}

// set true if the key is actually pressed
void ObjectPropertiesPanel::handleKeyPress(int key, bool pressed)
{
    keysPressed[key] = pressed;
}
// true when pressed
bool ObjectPropertiesPanel::isShortcutPressed() const
{
    return keysPressed[Qt::Key_Control];
}

void ObjectPropertiesPanel::RGBChanged()
{
    Material *material = SceneManager::getInstance().getShapeByID(currentSelectedShapeID)->getMaterial();
    if (material)
    {
        int r = redSpinBox->value();
        int g = greenSpinBox->value();
        int b = blueSpinBox->value();

        // Convert RGB to vec3 (0-1 range)
        vec3 newColor(r / 255.0f, g / 255.0f, b / 255.0f);

        // Execute command for undo/redo support
        commandManager.executeCommand(new MaterialDiffuseColorCommand(*material, newColor));

        // Update color preview
        colorPreview->setStyleSheet(QString("QLabel { background-color: rgb(%1, %2, %3); border: 1px solid #333; color: #888; }").arg(r).arg(g).arg(b));

        emit materialChanged();
    }
}

void ObjectPropertiesPanel::onShapeAdded()
{
    currentSelectedShapeID = SceneManager::getInstance().getShapes().back()->getID();
    onShapeSelectionChanged(currentSelectedShapeID);
}

void ObjectPropertiesPanel::onMaterialChanged()
{
    // Update UI widgets when material changes externally (e.g., undo/redo)
    Shape *shape = SceneManager::getInstance().getShapeByID(currentSelectedShapeID);
    if (shape == nullptr)
    {
        return;
    }

    Material *mat = shape->getMaterial();
    if (mat)
    {
        // Block signals to prevent feedback loop
        redSpinBox->blockSignals(true);
        greenSpinBox->blockSignals(true);
        blueSpinBox->blockSignals(true);
        reflectionSpinBox->blockSignals(true);
        refractionSpinBox->blockSignals(true);
        emissiveSpinBox->blockSignals(true);
        refractionIndexSpinBox->blockSignals(true);

        // Update RGB spinboxes
        redSpinBox->setValue(static_cast<int>(mat->getDiffuse().x * 255));
        greenSpinBox->setValue(static_cast<int>(mat->getDiffuse().y * 255));
        blueSpinBox->setValue(static_cast<int>(mat->getDiffuse().z * 255));

        // Update color preview
        colorPreview->setStyleSheet(QString("QLabel { background-color: rgb(%1, %2, %3); border: 1px solid #333; color: #888; }")
                                        .arg(static_cast<int>(mat->getDiffuse().x * 255))
                                        .arg(static_cast<int>(mat->getDiffuse().y * 255))
                                        .arg(static_cast<int>(mat->getDiffuse().z * 255)));

        // Update other material properties
        reflectionSpinBox->setValue(mat->getMetalness());
        refractionSpinBox->setValue(mat->getTransparency());
        emissiveSpinBox->setValue(mat->getLightIntensity());
        refractionIndexSpinBox->setValue(mat->getIndexMedium());

        // Unblock signals
        redSpinBox->blockSignals(false);
        greenSpinBox->blockSignals(false);
        blueSpinBox->blockSignals(false);
        reflectionSpinBox->blockSignals(false);
        refractionSpinBox->blockSignals(false);
        emissiveSpinBox->blockSignals(false);
        refractionIndexSpinBox->blockSignals(false);

        // Update texture previews
        onTextureSelectionChanged(mat);
    }
}