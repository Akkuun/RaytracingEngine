#pragma once

#include <QWidget>
#include <QElapsedTimer>
#include "../core/systems/RenderEngine/RenderEngine.h"

class RenderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RenderWidget(QWidget *parent = nullptr);

signals:
    void fpsUpdated(int fps);

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private slots:
    void scheduleNextFrame();

private:
    QElapsedTimer elapsedTimer;
    QElapsedTimer fpsTimer;
    bool colorToggle;
    QImage renderedImage; // output image
    RenderEngine* renderEngine;
    void renderFrame();
    void updateFPS();

    int width;
    int height;

    int frameCount;
    bool isRendering;
    
    // Mouse tracking for camera
    QPoint lastMousePos;
    bool mousePressed;
    float deltaTime;
};
