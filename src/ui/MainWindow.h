#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QEvent>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QShortcut>
#include "../core/systems/DeviceManager/DeviceManager.h"
#include "../core/systems/KernelManager/KernelManager.h"

class RenderWidget;
class ObjectPanel;

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
    void toggleBothPanels();

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
    
    // Panel references
    ObjectPanel *objectPanel;

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
    
    // Shortcuts
    QShortcut *togglePanelsShortcut;

    // OpenCL Device Manager
    DeviceManager* deviceManager;

    // Kernel Manager
    KernelManager* kernelManager;

    bool leftPanelVisible;
    bool rightPanelVisible;
};
