#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QEvent>
#include <QPropertyAnimation>
#include <QEasingCurve>

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

    bool leftPanelVisible;
    bool rightPanelVisible;
};

#endif // MAINWINDOW_H