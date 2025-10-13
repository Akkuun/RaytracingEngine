#ifndef CAMERAPANEL_H
#define CAMERAPANEL_H

#include <QWidget>

class CameraPanel : public QWidget
{
    Q_OBJECT

public:
    explicit CameraPanel(QWidget *parent = nullptr);

private:
    void setupUI();
};

#endif // CAMERAPANEL_H