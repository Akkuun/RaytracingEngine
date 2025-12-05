#include "MainWindow.h"
#include "RenderWidget.h"
#include "CollapsiblePanel.h"
#include "./panels/ScenePanel.h"
#include "./panels/ObjectPanel.h"
#include "./panels/ObjectPropertiesPanel.h"
#include "./panels/FPSChart.h"
#include "./panels/CameraPanel.h"
#include "./panels/ParametersPanel.h"
#include "../core/input/Keybinds.h"
#include "../core/commands/CommandsManager.h"
#include "../core/commands/actionsCommands/camera/CameraResetCommand.h"
#include "../core/systems/FileManager/FileManager.h"
#include <QVBoxLayout>
#include <QShortcut>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      commandManager(CommandsManager::getInstance()),
      leftPanelVisible(true),
      rightPanelVisible(true)
{
    // Initialize animations
    leftPanelAnimation = new QPropertyAnimation(this);
    rightPanelAnimation = new QPropertyAnimation(this);
    leftButtonAnimation = new QPropertyAnimation(this);
    rightButtonAnimation = new QPropertyAnimation(this);

    deviceManager = DeviceManager::getInstance();

    kernelManager = &KernelManager::getInstance();
    setupUI();

    // Setup keyboard shortcuts from Keybinds singleton
    Keybinds &keybinds = Keybinds::getInstance();

    togglePanelsShortcut = new QShortcut(keybinds.getKeybind(KB_TOGGLE_PANELS), this);
    connect(togglePanelsShortcut, &QShortcut::activated, this, &MainWindow::toggleBothPanels);

    applyOnAllAxisShortcut = new QShortcut(keybinds.getKeybind(KB_APPLY_ON_ALL_AXIS), this);
    connect(applyOnAllAxisShortcut, &QShortcut::activated, this, &MainWindow::ApplyUniformScaling);

    undoShortcut = new QShortcut(keybinds.getKeybind(KB_UNDO), this);
    connect(undoShortcut, &QShortcut::activated, this, &MainWindow::onUndo);

    redoShortcut = new QShortcut(keybinds.getKeybind(KB_REDO), this);
    connect(redoShortcut, &QShortcut::activated, this, &MainWindow::onRedo);

    resetCameraShortcut = new QShortcut(keybinds.getKeybind(KB_RESET_CAMERA), this);
    connect(resetCameraShortcut, &QShortcut::activated, this, &MainWindow::onResetCamera);

    saveFileShortcut = new QShortcut(keybinds.getKeybind(KB_SAVE), this);
    connect(saveFileShortcut, &QShortcut::activated, this, []()
            { FileManager::getInstance().saveProject(); });

    toggleFPSModeShortcut = new QShortcut(keybinds.getKeybind(KB_TOGGLE_FPS_MODE), this);
    connect(toggleFPSModeShortcut, &QShortcut::activated, this, &MainWindow::toggleFPSMode);
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Main container - just the render widget, panels will overlay
    renderWidget = new RenderWidget(centralWidget);
    renderWidget->setGeometry(0, 0, centralWidget->width(), centralWidget->height());

    // Setup panels as overlays
    setupLeftPanel();
    setupRightPanel();

    // Left toggle button
    leftToggleBtn = new QPushButton("◀", centralWidget);
    leftToggleBtn->setMaximumWidth(20);
    leftToggleBtn->setMaximumHeight(100);
    leftToggleBtn->setStyleSheet("QPushButton { background-color: #555; color: white; border: none; }");
    connect(leftToggleBtn, &QPushButton::clicked, this, &MainWindow::toggleLeftPanel);

    // Right toggle button
    rightToggleBtn = new QPushButton("▶", centralWidget);
    rightToggleBtn->setMaximumWidth(20);
    rightToggleBtn->setMaximumHeight(100);
    rightToggleBtn->setStyleSheet("QPushButton { background-color: #555; color: white; border: none; }");
    connect(rightToggleBtn, &QPushButton::clicked, this, &MainWindow::toggleRightPanel);

    // Install event filter to handle resizing and positioning
    centralWidget->installEventFilter(this);

    setWindowTitle("Raytracing Interface");
    resize(1200, 700);

    // Position overlays initially
    updateOverlayPositions();
}

void MainWindow::setupLeftPanel()
{
    leftScrollArea = new QScrollArea(centralWidget);
    leftScrollArea->setWidgetResizable(true);
    leftScrollArea->setMaximumWidth(320);
    leftScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    leftScrollArea->setStyleSheet("QScrollArea { background-color: #1e1e1e; border: none; }");

    leftPanel = new QWidget();
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setAlignment(Qt::AlignTop);
    leftLayout->setContentsMargins(5, 5, 5, 5);
    leftLayout->setSpacing(5);

    // Scene Panel
    CollapsiblePanel *scenePanelCollapsible = new CollapsiblePanel("▼ SCENE");
    ScenePanel *scenePanel = new ScenePanel();
    scenePanelCollapsible->setContent(scenePanel);
    leftLayout->addWidget(scenePanelCollapsible);

    // Object Panel
    CollapsiblePanel *objectPanelCollapsible = new CollapsiblePanel("▼ OBJECT TRANSFORM");
    objectPanel = new ObjectPanel();
    objectPanelCollapsible->setContent(objectPanel);
    leftLayout->addWidget(objectPanelCollapsible);

    // Object properties
    CollapsiblePanel *objectPropertiesPanelCollapsible = new CollapsiblePanel("▼ OBJECT PROPERTIES");
    objectPropertiesPanel = new ObjectPropertiesPanel();
    objectPropertiesPanelCollapsible->setContent(objectPropertiesPanel);
    leftLayout->addWidget(objectPropertiesPanelCollapsible);

    connect(scenePanel, &ScenePanel::shapeSelectionChanged,
            objectPropertiesPanel, &ObjectPropertiesPanel::onShapeSelectionChanged);

    // FPS Chart Panel
    FPSChart *fpsChart = new FPSChart();
    fpsChart->setMaxDataPoints(60);
    leftLayout->addWidget(fpsChart);
    connect(renderWidget, &RenderWidget::fpsUpdated, fpsChart, &FPSChart::addFPSValue);

    // Connect ScenePanel shape selection to ObjectPanel
    connect(scenePanel, &ScenePanel::shapeSelectionChanged,
            objectPanel, &ObjectPanel::onShapeSelectionChanged);

    leftLayout->addStretch();

    leftScrollArea->setWidget(leftPanel);
}

void MainWindow::setupRightPanel()
{
    rightPanel = new QWidget(centralWidget);
    rightPanel->setStyleSheet("QWidget { background-color: #1e1e1e; }");
    rightPanel->setMaximumWidth(300);
    rightPanel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);

    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setAlignment(Qt::AlignTop);
    rightLayout->setSpacing(5);
    rightLayout->setContentsMargins(5, 5, 5, 5);
    rightLayout->setSizeConstraint(QLayout::SetMinimumSize);

    // Camera Panel
    CollapsiblePanel *cameraPanel = new CollapsiblePanel("▼ CAMERA");
    cameraPanel->setContent(new CameraPanel());
    rightLayout->addWidget(cameraPanel);

    // Parameters Panel
    CollapsiblePanel *paramsPanel = new CollapsiblePanel("▼ PARAMETERS");
    ParametersPanel *parametersPanel = new ParametersPanel();
    paramsPanel->setContent(parametersPanel);
    rightLayout->addWidget(paramsPanel);

    connect(parametersPanel, &ParametersPanel::screenshotButtonClicked, this, &MainWindow::onScreenshotButtonClicked);
}

void MainWindow::updateOverlayPositions()
{
    if (!renderWidget || !centralWidget)
        return;

    // Viewport stays static and fills the entire central widget
    renderWidget->setGeometry(0, 0, centralWidget->width(), centralWidget->height());
    renderWidget->lower();

    // Adjust right panel to fit its content
    rightPanel->adjustSize();

    // Position left panel and toggle
    if (leftPanelVisible)
    {
        leftScrollArea->setGeometry(0, 0, 330, centralWidget->height());
        leftToggleBtn->move(320, centralWidget->height() / 4 - leftToggleBtn->height() / 2);
    }
    else
    {
        leftScrollArea->setGeometry(-320, 0, 330, centralWidget->height());
        leftToggleBtn->move(0, centralWidget->height() / 4 - leftToggleBtn->height() / 2);
    }

    // Position right panel and toggle
    if (rightPanelVisible)
    {
        rightPanel->move(centralWidget->width() - 300, 0);
        rightToggleBtn->move(centralWidget->width() - 300 - rightToggleBtn->width(),
                             centralWidget->height() / 4 - rightToggleBtn->height() / 2);
    }
    else
    {
        rightPanel->move(centralWidget->width(), 0);
        rightToggleBtn->move(centralWidget->width() - rightToggleBtn->width(),
                             centralWidget->height() / 4 - rightToggleBtn->height() / 2);
    }

    // Raise overlays to be on top of viewport
    leftScrollArea->raise();
    rightPanel->raise();
    leftToggleBtn->raise();
    rightToggleBtn->raise();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Resize)
    {
        updateOverlayPositions();
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::toggleLeftPanel()
{
    leftPanelVisible = !leftPanelVisible;
    leftToggleBtn->setText(leftPanelVisible ? "◀" : "▶");
    animateLeftPanel(leftPanelVisible);
}

void MainWindow::toggleRightPanel()
{
    rightPanelVisible = !rightPanelVisible;
    rightToggleBtn->setText(rightPanelVisible ? "▶" : "◀");
    animateRightPanel(rightPanelVisible);
}

void MainWindow::animateLeftPanel(bool show)
{
    // Stop any running animations
    leftPanelAnimation->stop();
    leftButtonAnimation->stop();

    // Set up panel animation
    leftPanelAnimation->setTargetObject(leftScrollArea);
    leftPanelAnimation->setPropertyName("geometry");
    leftPanelAnimation->setDuration(300);
    leftPanelAnimation->setEasingCurve(QEasingCurve::OutQuart);

    // Set up button animation
    leftButtonAnimation->setTargetObject(leftToggleBtn);
    leftButtonAnimation->setPropertyName("pos");
    leftButtonAnimation->setDuration(300);
    leftButtonAnimation->setEasingCurve(QEasingCurve::OutQuart);

    if (show)
    {
        // Animate from hidden to visible
        QRect startGeometry(-330, 0, 330, centralWidget->height());
        QRect endGeometry(0, 0, 330, centralWidget->height());
        QPoint startPos(0, centralWidget->height() / 4 - leftToggleBtn->height() / 2);
        QPoint endPos(320, centralWidget->height() / 4 - leftToggleBtn->height() / 2);

        leftScrollArea->setGeometry(startGeometry);
        leftScrollArea->setVisible(true);

        leftPanelAnimation->setStartValue(startGeometry);
        leftPanelAnimation->setEndValue(endGeometry);
        leftButtonAnimation->setStartValue(startPos);
        leftButtonAnimation->setEndValue(endPos);
    }
    else
    {
        // Animate from visible to hidden
        QRect startGeometry(0, 0, 330, centralWidget->height());
        QRect endGeometry(-330, 0, 330, centralWidget->height());
        QPoint startPos(320, centralWidget->height() / 4 - leftToggleBtn->height() / 2);
        QPoint endPos(0, centralWidget->height() / 4 - leftToggleBtn->height() / 2);

        leftPanelAnimation->setStartValue(startGeometry);
        leftPanelAnimation->setEndValue(endGeometry);
        leftButtonAnimation->setStartValue(startPos);
        leftButtonAnimation->setEndValue(endPos);

        // Hide panel when animation finishes
        connect(leftPanelAnimation, &QPropertyAnimation::finished, [this]()
                {
            if (!leftPanelVisible) {
                leftScrollArea->setVisible(false);
            } });
    }

    leftPanelAnimation->start();
    leftButtonAnimation->start();
}

void MainWindow::animateRightPanel(bool show)
{
    // Stop any running animations
    rightPanelAnimation->stop();
    rightButtonAnimation->stop();

    // Set up panel animation
    rightPanelAnimation->setTargetObject(rightPanel);
    rightPanelAnimation->setPropertyName("pos");
    rightPanelAnimation->setDuration(300);
    rightPanelAnimation->setEasingCurve(QEasingCurve::OutQuart);

    // Set up button animation
    rightButtonAnimation->setTargetObject(rightToggleBtn);
    rightButtonAnimation->setPropertyName("pos");
    rightButtonAnimation->setDuration(300);
    rightButtonAnimation->setEasingCurve(QEasingCurve::OutQuart);

    if (show)
    {
        // Animate from hidden to visible
        QPoint startPos(centralWidget->width(), 0);
        QPoint endPos(centralWidget->width() - 300, 0);
        QPoint startBtnPos(centralWidget->width() - rightToggleBtn->width(),
                           centralWidget->height() / 4 - rightToggleBtn->height() / 2);
        QPoint endBtnPos(centralWidget->width() - 300 - rightToggleBtn->width(),
                         centralWidget->height() / 4 - rightToggleBtn->height() / 2);

        rightPanel->move(startPos);
        rightPanel->setVisible(true);

        rightPanelAnimation->setStartValue(startPos);
        rightPanelAnimation->setEndValue(endPos);
        rightButtonAnimation->setStartValue(startBtnPos);
        rightButtonAnimation->setEndValue(endBtnPos);
    }
    else
    {
        // Animate from visible to hidden
        QPoint startPos(centralWidget->width() - 300, 0);
        QPoint endPos(centralWidget->width(), 0);
        QPoint startBtnPos(centralWidget->width() - 300 - rightToggleBtn->width(),
                           centralWidget->height() / 4 - rightToggleBtn->height() / 2);
        QPoint endBtnPos(centralWidget->width() - rightToggleBtn->width(),
                         centralWidget->height() / 4 - rightToggleBtn->height() / 2);

        rightPanelAnimation->setStartValue(startPos);
        rightPanelAnimation->setEndValue(endPos);
        rightButtonAnimation->setStartValue(startBtnPos);
        rightButtonAnimation->setEndValue(endBtnPos);

        // Hide panel when animation finishes
        connect(rightPanelAnimation, &QPropertyAnimation::finished, [this]()
                {
            if (!rightPanelVisible) {
                rightPanel->setVisible(false);
            } });
    }

    rightPanelAnimation->start();
    rightButtonAnimation->start();
}

void MainWindow::toggleBothPanels()
{
    // Hide both panels if either one is visible, otherwise show both
    bool shouldHide = leftPanelVisible || rightPanelVisible;

    if (shouldHide)
    {
        // Hide both panels
        if (leftPanelVisible)
        {
            leftPanelVisible = false;
            leftToggleBtn->setText("▶");
            animateLeftPanel(false);
        }
        if (rightPanelVisible)
        {
            rightPanelVisible = false;
            rightToggleBtn->setText("◀");
            animateRightPanel(false);
        }
    }
    else
    {
        // Show both panels
        leftPanelVisible = true;
        leftToggleBtn->setText("◀");
        animateLeftPanel(true);

        rightPanelVisible = true;
        rightToggleBtn->setText("▶");
        animateRightPanel(true);
    }
}

void MainWindow::onUndo()
{
    if (commandManager.canUndo())
    {
        commandManager.undo();
    }
}

void MainWindow::onRedo()
{
    if (commandManager.canRedo())
    {
        commandManager.redo();
    }
}

void MainWindow::onResetCamera()
{
    // Create and execute camera reset command (allows undo)
    commandManager.executeCommand(new CameraResetCommand());
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    // Forward key events to Camera for simple key tracking
    Camera::getInstance().handleKeyPress(event->key(), true);
    // Forward key events to ObjectPanel for shortcut tracking
    if (objectPanel)
        objectPanel->handleKeyPress(event->key(), true);
    // Let the base class handle other keys (and trigger shortcuts)
    QMainWindow::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    // Forward key events to Camera for simple key tracking
    Camera::getInstance().handleKeyPress(event->key(), false);
    // Forward key events to ObjectPanel for shortcut tracking
    if (objectPanel)
        objectPanel->handleKeyPress(event->key(), false);
    // Let the base class handle other keys
    QMainWindow::keyReleaseEvent(event);
}

void MainWindow::ApplyUniformScaling()
{
    objectPanel->setApplyOnAllAxis(true);
}

void MainWindow::onScreenshotButtonClicked()
{
    renderWidget->captureScreenshot();
}

void MainWindow::toggleFPSMode()
{
    Camera::getInstance().onToggleFPSMode();
}