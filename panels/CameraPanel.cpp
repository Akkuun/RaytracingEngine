#include "CameraPanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QCheckBox>

CameraPanel::CameraPanel(QWidget *parent) : QWidget(parent)
{
    setupUI();
}

void CameraPanel::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(5);

    // Position
    layout->addWidget(new QLabel("POSITION"));
    QHBoxLayout *posLayout = new QHBoxLayout();
    QDoubleSpinBox *posX = new QDoubleSpinBox();
    QDoubleSpinBox *posY = new QDoubleSpinBox();
    QDoubleSpinBox *posZ = new QDoubleSpinBox();
    posX->setPrefix("X: ");
    posY->setPrefix("Y: ");
    posZ->setPrefix("Z: ");
    posX->setRange(-1000, 1000);
    posY->setRange(-1000, 1000);
    posZ->setRange(-1000, 1000);
    posLayout->addWidget(posX);
    posLayout->addWidget(posY);
    posLayout->addWidget(posZ);
    layout->addLayout(posLayout);

    // Rotation
    layout->addWidget(new QLabel("ROTATION"));
    QHBoxLayout *rotLayout = new QHBoxLayout();
    QDoubleSpinBox *rotX = new QDoubleSpinBox();
    QDoubleSpinBox *rotY = new QDoubleSpinBox();
    QDoubleSpinBox *rotZ = new QDoubleSpinBox();
    rotX->setPrefix("X: ");
    rotY->setPrefix("Y: ");
    rotZ->setPrefix("Z: ");
    rotX->setRange(-360, 360);
    rotY->setRange(-360, 360);
    rotZ->setRange(-360, 360);
    rotLayout->addWidget(rotX);
    rotLayout->addWidget(rotY);
    rotLayout->addWidget(rotZ);
    layout->addLayout(rotLayout);

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