#pragma once

#include <QWidget>
#include <QElapsedTimer>
#include "../core/systems/RenderEngine/RenderEngine.h"

class RenderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RenderWidget(QWidget *parent = nullptr);

protected:
    void paintEvent( QPaintEvent *event) override;

private:
    QTimer *renderTimer; // Qt timer to launch a render
    QElapsedTimer elapsedTimer;
    bool colorToggle;
    QImage renderedImage; // output image
    RenderEngine* renderEngine;
    void renderFrame();
    void updateFPS();

    int width;
    int height;

    int frameCount;
    
};
