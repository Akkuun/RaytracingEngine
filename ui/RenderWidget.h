#pragma once

#include <QWidget>

class RenderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RenderWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
};
