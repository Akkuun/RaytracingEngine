#pragma once

#include <QWidget>
#include <QVector>

class FPSChart : public QWidget
{
    Q_OBJECT

public:
    explicit FPSChart(QWidget *parent = nullptr);

    // lines values
    void setMaxDataPoints(int max);

public slots:
    void addFPSValue(int fps);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<int> fpsData;
    int maxDataPoints;
    int maxFPS;
};
