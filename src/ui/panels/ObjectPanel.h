#pragma once

#include <QWidget>

class FPSChart;
class RenderWidget;

class ObjectPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ObjectPanel(QWidget *parent = nullptr);
    void setRenderWidget(RenderWidget *widget);

private:
    void setupUI();
    FPSChart *fpsChart;
};
