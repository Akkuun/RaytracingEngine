#pragma once

#include <QWidget>
#include "../../core/commands/CommandsManager.h"
#include "../../core/utils/imageLoader/ImageLoader.h"
#include "../../core/material/Material.h"
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QFrame>
#include <QMap>
class FPSChart;
class RenderWidget;

class ObjectPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ObjectPanel(QWidget *parent = nullptr);
    void setApplyOnAllAxis(bool apply);
    void handleKeyPress(int key, bool pressed);

public slots:
    void onShapeSelectionChanged(int shapeID); // Receiver of the signal emitted when the selected shape changes (from SceneTreeWidget)

signals:
    // detect when shape X position change
    void shapeXPositionChanged(int shapeID, float newX);
    // detect when shape Y position change
    void shapeYPositionChanged(int shapeID, float newY);
    // detect when shape Z position change
    void shapeZPositionChanged(int shapeID, float newZ);

private:
    void setupUI();
    int currentSelectedShapeID;
    CommandsManager &commandManager;

    // Current position values of the selected shape
    QDoubleSpinBox *posX;
    QDoubleSpinBox *posY;
    QDoubleSpinBox *posZ;
    // Current rotation values of the selected shape
    QDoubleSpinBox *rotX;
    QDoubleSpinBox *rotY;
    QDoubleSpinBox *rotZ;
    // Current scale values of the selected shape
    QDoubleSpinBox *scaleX;
    QDoubleSpinBox *scaleY;
    QDoubleSpinBox *scaleZ;

    bool applyOnAllAxis = false;
    QMap<int, bool> keysPressed;
    bool isShortcutPressed() const;
};
