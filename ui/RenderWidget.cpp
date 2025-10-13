#include "RenderWidget.h"
#include <QPainter>

RenderWidget::RenderWidget(QWidget *parent) : QWidget(parent)
{
    setMinimumSize(400, 300);
}

void RenderWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);

    // Placeholder for raytraced scene
    painter.setPen(Qt::white);
    painter.drawText(rect(), Qt::AlignCenter, "Raytracing Viewport");
}