#include "RenderWidget.h"
#include <QPainter>
#include <QTimer>
#include <QDebug>
#include <iostream>
RenderWidget::RenderWidget(QWidget *parent) : QWidget(parent)
{
    renderEngine = new RenderEngine();

    // main loop , each 16 ms ( ~60 FPS) we call renderFrame
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &RenderWidget::renderFrame);
    timer->start(1); // asap
    elapsedTimer.start();
    frameCount = 0;
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
        std::vector<float> imageData = renderEngine->getImageData();

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

void RenderWidget::paintEvent(QPaintEvent *event)
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
    if (elapsedTimer.elapsed() >= 1000) // every second
    {
        qDebug() << "FPS:" << frameCount;
        frameCount = 0;
        elapsedTimer.restart();
    }
}