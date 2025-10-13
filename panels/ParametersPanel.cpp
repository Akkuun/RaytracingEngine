#include "ParametersPanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>

ParametersPanel::ParametersPanel(QWidget *parent) : QWidget(parent)
{
    setupUI();
}

void ParametersPanel::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(8);
    layout->setContentsMargins(5, 5, 5, 5);

    // Rays per pixel
    QHBoxLayout *raysLayout = new QHBoxLayout();
    QLabel *raysLabel = new QLabel("RAYS PER PIXEL");
    raysLabel->setStyleSheet("QLabel { font-size: 9px; }");
    QSpinBox *raysSpin = new QSpinBox();
    raysSpin->setRange(1, 1000);
    raysSpin->setValue(1);
    raysSpin->setMaximumWidth(60);
    raysLayout->addWidget(raysLabel);
    raysLayout->addWidget(raysSpin);
    layout->addLayout(raysLayout);

    // Max rebounds
    QHBoxLayout *reboundsLayout = new QHBoxLayout();
    QLabel *reboundsLabel = new QLabel("MAX REBOUNDS");
    reboundsLabel->setStyleSheet("QLabel { font-size: 9px; }");
    QSpinBox *reboundsSpin = new QSpinBox();
    reboundsSpin->setRange(1, 100);
    reboundsSpin->setValue(5);
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

    // Don't set background style on the widget itself - let it inherit from parent
    setStyleSheet("QLabel { color: white; }");
}