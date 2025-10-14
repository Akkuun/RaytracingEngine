#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QEvent>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include "../src/core/DeviceManager/DeviceManager.h"
#include "../src/core/KernelManager/KernelManager.h"

class RenderWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void toggleLeftPanel();
    void toggleRightPanel();

private:
    void setupUI();
    void setupLeftPanel();
    void setupRightPanel();
    void updateOverlayPositions();
    void animateLeftPanel(bool show);
    void animateRightPanel(bool show);

    // Main widgets
    QWidget *centralWidget;

    // Side panels
    QWidget *leftPanel;
    QWidget *rightPanel;
    QScrollArea *leftScrollArea;

    // Render area
    RenderWidget *renderWidget;

    // Toggle buttons
    QPushButton *leftToggleBtn;
    QPushButton *rightToggleBtn;

    // Animations
    QPropertyAnimation *leftPanelAnimation;
    QPropertyAnimation *rightPanelAnimation;
    QPropertyAnimation *leftButtonAnimation;
    QPropertyAnimation *rightButtonAnimation;

    // OpenCL Device Manager
    DeviceManager* deviceManager;

    // Kernel Manager
    KernelManager* kernelManager;

    bool leftPanelVisible;
    bool rightPanelVisible;
};
