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
#include "../core/camera/Camera.h"
#include "../core/systems/DeviceManager/DeviceManager.h"
#include "../core/systems/KernelManager/KernelManager.h"
#include "../core/commands/CommandsManager.h"

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
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void toggleLeftPanel();
    void toggleRightPanel();
    void toggleBothPanels();
    void onUndo();
    void onRedo();

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
    QShortcut *undoShortcut;
    QShortcut *redoShortcut;

    // OpenCL Device Manager
    DeviceManager* deviceManager;

    // Kernel Manager
    KernelManager* kernelManager;
    
    // Commands Manager
    CommandsManager& commandManager;

    bool leftPanelVisible;
    bool rightPanelVisible;
};
