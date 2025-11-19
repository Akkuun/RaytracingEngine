#include "RenderWidget.h"
#include "../core/commands/CommandsManager.h"
#include "../core/camera/Camera.h"
#include <QPainter>
#include <QTimer>
#include <QDebug>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <fstream>

RenderWidget::RenderWidget(QWidget *parent) 
    : QWidget(parent), isRendering(false), mousePressed(false), deltaTime(0.016f)
{
    renderEngine = new RenderEngine();

    // Register callback for scene changes
    CommandsManager::getInstance().addSceneChangedCallback([this]() {
        // Notify render engine that scene changed
        renderEngine->notifySceneChanged();
    });

    // Enable mouse tracking for camera control
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    // Continuous rendering - render as fast as possible
    fpsTimer.start();
    elapsedTimer.start();
    frameCount = 0;
    
    // Start the rendering loop
    QMetaObject::invokeMethod(this, "scheduleNextFrame", Qt::QueuedConnection);
}

void RenderWidget::scheduleNextFrame()
{
    if (!isRendering)
    {
        isRendering = true;
        
        // Calculate delta time
        deltaTime = elapsedTimer.elapsed() / 1000.0f;
        elapsedTimer.restart();
        
        // Update camera with delta time
        Camera& camera = Camera::getInstance();
        camera.update(deltaTime);
        
        // Check if camera moved and reset TAA accumulation if needed
        if (camera.hasMoved() && renderEngine) {
            renderEngine->markCameraDirty();
            camera.clearMovedFlag();
        }
        
        renderFrame();
        isRendering = false;
        
        // Immediately schedule the next frame (render as fast as possible)
        QMetaObject::invokeMethod(this, "scheduleNextFrame", Qt::QueuedConnection);
    }
}

void RenderWidget::renderFrame()
{
    width = QWidget::width();
    height = QWidget::height();
    if (renderedImage.width() != width || renderedImage.height() != height)
    {
        renderedImage = QImage(width, height, QImage::Format_RGB32);
    }

    if (width > 0 && height > 0)
    {
        renderEngine->render(width, height);

        // [RGB, RGB, RGB, ...] GPU <-> CPU with RGB between 0 and 1
        const std::vector<float>& imageData = renderEngine->getImageData();

        // Direct access to QImage bits for performance
        uchar *bits = renderedImage.bits();
        const float *data = imageData.data();
        const int pixelCount = width * height;

        //  pragma omp for to parallelize the loop  of writing pixels
        #pragma omp parallel for
        for (int i = 0; i < pixelCount; ++i)
        {
            int baseIdx = i * 3;
            int baseIdxOut = i * 4; // QImage Format_RGB32 use 4 bytes for each pixel (ARGB)

            // Conversion optimized float → byte
            bits[baseIdxOut + 2] = data[baseIdx] * 255.0f;     // R
            bits[baseIdxOut + 1] = data[baseIdx + 1] * 255.0f; // G
            bits[baseIdxOut + 0] = data[baseIdx + 2] * 255.0f; // B
            bits[baseIdxOut + 3] = 255;                        // Alpha
        }

        updateFPS();
        update();
    }
}

void RenderWidget::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);

    if (!renderedImage.isNull())
    {
        painter.drawImage(rect(), renderedImage);
    }
    else
    {
        painter.fillRect(rect(), Qt::black);
        painter.setPen(Qt::white);
        painter.drawText(rect(), Qt::AlignCenter, "No data...");
    }
}

void RenderWidget::updateFPS()
{
    frameCount++;
    if (fpsTimer.elapsed() >= 1000) // every second
    {
        //qDebug() << "FPS:" << frameCount;
        emit fpsUpdated(frameCount);
        frameCount = 0;
        fpsTimer.restart();
    }
}

void RenderWidget::keyPressEvent(QKeyEvent *event)
{
    Camera::getInstance().handleKeyPress(event->key(), true);
    QWidget::keyPressEvent(event);
}

void RenderWidget::keyReleaseEvent(QKeyEvent *event)
{
    Camera::getInstance().handleKeyPress(event->key(), false);
    QWidget::keyReleaseEvent(event);
}

void RenderWidget::mouseMoveEvent(QMouseEvent *event)
{
    // DISABLED: Mouse rotation temporarily disabled
    // TODO: Re-enable mouse rotation when needed
    /*
    if (mousePressed || Camera::getInstance().isFPS())
    {
        QPoint currentPos = event->pos();
        if (!lastMousePos.isNull())
        {
            float deltaX = currentPos.x() - lastMousePos.x();
            float deltaY = currentPos.y() - lastMousePos.y();
            Camera::getInstance().handleMouseMove(deltaX, deltaY);
        }
        lastMousePos = currentPos;
        
        // Keep mouse centered in FPS mode for continuous rotation
        if (Camera::getInstance().isFPS())
        {
            QPoint center(width / 2, height / 2);
            if ((currentPos - center).manhattanLength() > 100)
            {
                cursor().setPos(mapToGlobal(center));
                lastMousePos = center;
            }
        }
    }
    */
    QWidget::mouseMoveEvent(event);
}

void RenderWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        mousePressed = true;
        lastMousePos = event->pos();
        setCursor(Qt::BlankCursor);
    }
    QWidget::mousePressEvent(event);
}

void RenderWidget::wheelEvent(QWheelEvent *event)
{
    float delta = event->angleDelta().y() / 120.0f; // Standard wheel delta
    Camera::getInstance().handleMouseScroll(delta);
    QWidget::wheelEvent(event);
}

void RenderWidget::focusInEvent(QFocusEvent *event)
{
    QWidget::focusInEvent(event);
}

void RenderWidget::focusOutEvent(QFocusEvent *event)
{
    mousePressed = false;
    setCursor(Qt::ArrowCursor);
    QWidget::focusOutEvent(event);
}