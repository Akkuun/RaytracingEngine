#pragma once

#include <QWidget>

class ObjectPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ObjectPanel(QWidget *parent = nullptr);

private:
    void setupUI();
    void testKernel();
};
