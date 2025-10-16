#include "RenderWidget.h"
#include <QPainter>
#include <QTimer>
#include <QDebug>

RenderWidget::RenderWidget(QWidget *parent) : QWidget(parent)
{
    renderEngine = new RenderEngine();

    // main loop , each 16 ms ( ~60 FPS) we call renderFrame
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &RenderWidget::renderFrame);
    timer->start(16); // ~60 FPS
}

void RenderWidget::renderFrame()
{
    width = QWidget::width();
    height = QWidget::height();

    if (width > 0 && height > 0)
    {
        renderEngine->render(width, height);

        // [RGB, RGB, RGB, ...] GPU <-> CPU with RGB between 0 and 1 
        std::vector<float> imageData = renderEngine->getImageData();


        QImage image(width, height, QImage::Format_RGB32);
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                int idx = (y * width + x) * 3;
                // // qdebug the value of the pixel


                // [0-1] > [0-255]
                int r = static_cast<int>(imageData[idx] * 255.0f);
                int g = static_cast<int>(imageData[idx + 1] * 255.0f);
                int b = static_cast<int>(imageData[idx + 2] * 255.0f);

                // Clamp values
                r = std::max(0, std::min(255, r));
                g = std::max(0, std::min(255, g));
                b = std::max(0, std::min(255, b));

                image.setPixel(x, y, qRgb(r, g, b));
            }
        }

        renderedImage = image;
        update(); // Trigger repaint
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