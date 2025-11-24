#include <CL/opencl.hpp>
#include "FPSChartPanel.h"
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

FPSChartPanel::FPSChartPanel(QWidget *parent) : QWidget(parent), fpsChart(nullptr)
{
    setupUI();
}

void FPSChartPanel::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(5);
    
    // FPS Chart
    layout->addSpacing(15);
    layout->addWidget(new QLabel("FPS MONITOR"));
    fpsChart = new FPSChart();
    fpsChart->setMaxDataPoints(60); // Show last 60 FPS values
    layout->addWidget(fpsChart);

    // Only style the text color, inherit background from parent
    setStyleSheet("QLabel { color: white; }");
}

void FPSChartPanel::setRenderWidget(RenderWidget *widget)
{
    if (widget && fpsChart)
    {
        // send the signal to update the FPS chart
        connect(widget, &RenderWidget::fpsUpdated, fpsChart, &FPSChart::addFPSValue);
    }
}