#pragma once

#include <QWidget>

class QSpinBox;


class ParametersPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ParametersPanel(QWidget *parent = nullptr);

private slots:
    void onCameraNBouncesChanged(int bounces);
    void onCameraRaysPerPixelChanged(int rpp);

private:
    void setupUI();

    QSpinBox *raysSpin;
    QSpinBox *reboundsSpin;
};
