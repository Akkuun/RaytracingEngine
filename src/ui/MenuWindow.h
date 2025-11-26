#pragma once
#include <QWidget>
#include <QLabel>

class MenuWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MenuWindow(QWidget *parent = nullptr);

private:
    QList<QPair<QString, QString>> projectsData;
    QList<QPair<QString, QString>> examplesData;

    void listRecentProjects(); // loop though the dataFolder and sort each name+file in chronological order

    void createNewProject();                              // open a dialog to create a new project (choose name + location) and create the cube scene
    void openProjectFromPath(const QString &projectPath); // load project from dialog
    void loadProject();                                   // open a file dialog to load a project

    QLabel *titleLabel; // change to "Recent Projects" or "Examples" depending on the view
private slots:
    void openMainWindow();

signals:
    void loadProject(const QString &projectPath); // send a signal to send the current project path to FileManager instance
};
