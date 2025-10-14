#pragma once

#include <QWidget>
#include <QElapsedTimer>

class RenderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RenderWidget(QWidget *parent = nullptr);

protected:
    void paintEvent( QPaintEvent *event) override;

private:
    QTimer *renderTimer;
    QElapsedTimer elapsedTimer;
    bool colorToggle;
};
