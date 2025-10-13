#ifndef OBJECTPANEL_H
#define OBJECTPANEL_H

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

#endif // OBJECTPANEL_H