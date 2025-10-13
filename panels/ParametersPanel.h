#pragma once

#include <QWidget>

class ParametersPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ParametersPanel(QWidget *parent = nullptr);

private:
    void setupUI();
};
