#ifndef PARAMETERSPANEL_H
#define PARAMETERSPANEL_H

#include <QWidget>

class ParametersPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ParametersPanel(QWidget *parent = nullptr);

private:
    void setupUI();
};

#endif // PARAMETERSPANEL_H