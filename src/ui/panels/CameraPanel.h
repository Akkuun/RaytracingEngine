#pragma once
#include <QWidget>

class QDoubleSpinBox;
class QSpinBox;

class CameraPanel : public QWidget
{
    Q_OBJECT

public:
    explicit CameraPanel(QWidget *parent = nullptr);

private slots:
    void onCameraPositionChanged(float x, float y, float z);
    void onCameraRotationChanged(float x, float y, float z);
    void onCameraFOVChanged(float fov);

private:
    void setupUI();
    
    // Store references to spin boxes to update them from signals
    QDoubleSpinBox *posX;
    QDoubleSpinBox *posY;
    QDoubleSpinBox *posZ;
    QDoubleSpinBox *rotX;
    QDoubleSpinBox *rotY;
    QDoubleSpinBox *rotZ;
    QSpinBox *fovSpin;
};
