#pragma once
#include <QMainWindow>
#include <QPushButton>
#include <QLabel>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr);
private slots:
    void runKernel();
private:
    QPushButton* runButton;
    QLabel* resultLabel;
};
