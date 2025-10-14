#pragma once
#include <QWidget>

class CameraPanel : public QWidget
{
    Q_OBJECT

public:
    explicit CameraPanel(QWidget *parent = nullptr);

private:
    void setupUI();
};
