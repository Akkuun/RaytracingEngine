#pragma once

#include <QWidget>
#include "../../core/commands/CommandsManager.h"
#include "../../core/material/Material.h"
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QFrame>
#include <QMap>
#include <QLabel>
class RenderWidget;

class ObjectPropertiesPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ObjectPropertiesPanel(QWidget *parent = nullptr);
    void setRenderWidget(RenderWidget *widget);
    void handleKeyPress(int key, bool pressed);

public slots:
    void onShapeSelectionChanged(int shapeID); // Receiver of the signal emitted when the selected shape changes (from SceneTreeWidget)

signals:
    void materialChanged();

private:
    void setupUI();
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
    QDoubleSpinBox *reflectionSpinBox;
    QDoubleSpinBox *refractionSpinBox;
    QSpinBox *emissiveSpinBox;
    QDoubleSpinBox *refractionIndexSpinBox;

    QMap<int, bool> keysPressed;
    bool isShortcutPressed() const;
};
