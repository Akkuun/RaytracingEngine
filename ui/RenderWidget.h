#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QWidget>

class RenderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RenderWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // RENDERWIDGET_H