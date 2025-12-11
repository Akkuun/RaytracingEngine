#pragma once

#include <QWidget>
#include "../../core/commands/CommandsManager.h"
#include "../../core/material/Material.h"
#include "./CustomDoubleSpinBox.h"
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QFrame>
#include <QMap>
#include <QLabel>
#include <QLineEdit>

class RenderWidget;

class ObjectPropertiesPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ObjectPropertiesPanel(QWidget *parent = nullptr);
    void setRenderWidget(RenderWidget *widget);
    void handleKeyPress(int key, bool pressed);

public slots:
    void onShapeSelectionChanged(int shapeID); // Receiver of the signal emitted when the selected shape changes (from SceneTreeWidget  
    void onShapeAdded(); // Receiver of the signal emitted when a new shape is added

signals:
    void materialChanged();

private:
    void setupUI();
    void RGBChanged();
    int currentSelectedShapeID;
    CommandsManager &commandManager;

    void onTextureSelectionChanged(const Material *material);

    QFrame *texturePreviewFrame;
    QPushButton *loadTextureBtn;
    QPushButton *clearTextureBtn;

    QFrame *normalPreviewFrame;
    QPushButton *loadNormalBtn;
    QPushButton *clearNormalBtn;

    QFrame *metalPreviewFrame;
    QPushButton *loadMetalBtn;
    QPushButton *clearMetalBtn;

    QFrame *emissivePreviewFrame;
    QPushButton *loadEmissiveBtn;
    QPushButton *clearEmissiveBtn;

    // Material properties
    
    QFrame *colorFrame;
    QLabel *colorPreview;
    CustomDoubleSpinBox *redSpinBox;
    CustomDoubleSpinBox *greenSpinBox;
    CustomDoubleSpinBox *blueSpinBox;
    QDoubleSpinBox *reflectionSpinBox;
    QDoubleSpinBox *refractionSpinBox;
    QSpinBox *emissiveSpinBox;
    QDoubleSpinBox *refractionIndexSpinBox;

    QMap<int, bool> keysPressed;
    bool isShortcutPressed() const;
};
