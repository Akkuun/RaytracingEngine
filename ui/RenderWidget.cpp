#include "RenderWidget.h"
#include <QPainter>
#include <QTimer>

RenderWidget::RenderWidget(QWidget *parent) : QWidget(parent)
{
    setMinimumSize(400, 300);

    elapsedTimer.start(); // Démarre le timer au lancement

    renderTimer = new QTimer(this);
    connect(renderTimer, &QTimer::timeout, this, QOverload<>::of(&QWidget::update));
    renderTimer->start(16); // ~60 FPS
}

void RenderWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    // Changer de couleur toutes les 1000ms (1 seconde)
    const qint64 toggleIntervalMs = 1000;

    qint64 elapsed = elapsedTimer.elapsed();
    qint64 cyclePosition = elapsed % (toggleIntervalMs * 2); // Cycle complet = 2 secondes

    colorToggle = (cyclePosition >= toggleIntervalMs);

    // Alterner entre blanc et rouge
    QColor currentColor = colorToggle ? Qt::red : Qt::black;
    painter.fillRect(rect(), currentColor);
}