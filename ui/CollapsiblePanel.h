#ifndef COLLAPSIBLEPANEL_H
#define COLLAPSIBLEPANEL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>

class CollapsiblePanel : public QWidget
{
    Q_OBJECT

public:
    explicit CollapsiblePanel(const QString &title, QWidget *parent = nullptr);
    void setContent(QWidget *content);

private slots:
    void toggleCollapse();

private:
    QPushButton *toggleButton;
    QWidget *contentWidget;
    QVBoxLayout *mainLayout;
    bool collapsed;
};

#endif // COLLAPSIBLEPANEL_H