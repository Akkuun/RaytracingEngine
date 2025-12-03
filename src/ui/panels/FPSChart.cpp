#include "FPSChart.h"
#include <QPainter>
#include <QDebug>

FPSChart::FPSChart(QWidget *parent)
    : QWidget(parent), maxDataPoints(60), maxFPS(130)
{
    setMinimumHeight(100);
    setMaximumHeight(150);
    setStyleSheet("QWidget { background-color: #1a1a1a; border: 1px solid #333; }");
}

void FPSChart::setMaxDataPoints(int max)
{
    maxDataPoints = max;
    if (fpsData.size() > maxDataPoints)
    {
        fpsData.remove(0, fpsData.size() - maxDataPoints);
    }
}

void FPSChart::addFPSValue(int fps)
{
    fpsData.append(fps);

    // Keep only the last N data points to better memory managment
    if (fpsData.size() > maxDataPoints)
    {
        fpsData.removeFirst();
    }

    update(); // Trigger repaint
}

void FPSChart::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Background
    painter.fillRect(rect(), QColor(26, 26, 26));

    if (fpsData.isEmpty())
    {
        painter.setPen(QColor(136, 136, 136));
        painter.drawText(rect(), Qt::AlignCenter, "No FPS data yet");
        return;
    }

    int w = width();
    int h = height();
    int padding = 25;
    int chartWidth = w - 2 * padding;
    int chartHeight = h - 2 * padding;

    // Draw grid lines
    painter.setPen(QColor(60, 60, 60));
    int fpsLabels[] = {130, 90, 45, 0};
    for (int i = 0; i <= 3; i++)
    {
        int y = padding + (chartHeight * i) / 3;
        painter.drawLine(padding, y, w - padding, y);

        // Draw FPS labels
        painter.setPen(QColor(136, 136, 136));
        painter.drawText(2, y + 5, QString::number(fpsLabels[i]));
        painter.setPen(QColor(60, 60, 60));
    }

    // Draw the FPS line
    if (fpsData.size() > 1)
    {
        painter.setPen(QPen(QColor(0, 200, 100), 2));

        float xStep = (float)chartWidth / (maxDataPoints - 1);
        int offset = maxDataPoints - fpsData.size();

        for (int i = 0; i < fpsData.size() - 1; i++)
        {
            float x1 = padding + (i + offset) * xStep;
            float x2 = padding + (i + 1 + offset) * xStep;

            float y1 = h - padding - (fpsData[i] * chartHeight / maxFPS);
            float y2 = h - padding - (fpsData[i + 1] * chartHeight / maxFPS);

            // Clamp values
            y1 = qBound((float)padding, y1, (float)(h - padding));
            y2 = qBound((float)padding, y2, (float)(h - padding));

            painter.drawLine(QPointF(x1, y1), QPointF(x2, y2));
        }

        // Draw current FPS text
        painter.setPen(QColor(0, 255, 120));
        int currentFPS = fpsData.last();
        QString fpsText = QString("FPS: %1").arg(currentFPS);
        painter.drawText(w - 70, 15, fpsText);
    }

    // Draw border
    painter.setPen(QColor(51, 51, 51));
    painter.drawRect(0, 0, w - 1, h - 1);
}
