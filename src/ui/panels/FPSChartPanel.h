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

class FPSChartPanel : public QWidget
{
    Q_OBJECT

public:
    explicit FPSChartPanel(QWidget *parent = nullptr);
    void setRenderWidget(RenderWidget *widget);

private:
    void setupUI();
    FPSChart *fpsChart;
};
