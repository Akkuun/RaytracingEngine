#pragma once

#include <QWidget>
#include "../../core/commands/CommandsManager.h"
#include <QDoubleSpinBox>
class FPSChart;
class RenderWidget;

class ObjectPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ObjectPanel(QWidget *parent = nullptr);
    void setRenderWidget(RenderWidget *widget);

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
    FPSChart *fpsChart;
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
};
