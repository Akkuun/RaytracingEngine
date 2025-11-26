#include "MenuWindow.h"
#include "MainWindow.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QFrame>
#include <QFileDialog>
#include "../../src/core/systems/FileManager/FileManager.h"

MenuWindow::MenuWindow(QWidget *parent)
    : QWidget(parent), titleLabel(new QLabel("Recent Projects"))
{
    setWindowTitle("Raytracing Engine - Menu");
    setMinimumSize(700, 400);

    auto *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(40, 30, 40, 30);
    mainLayout->setSpacing(30);

    // Left part (buttons)
    auto *leftPanel = new QVBoxLayout();
    leftPanel->setSpacing(20);

    auto *createBtn = new QPushButton("Create Project...");
    auto *openBtn = new QPushButton("Open Project...");
    auto *projectsBtn = new QPushButton("Projects");
    auto *examplesBtn = new QPushButton("Examples");

    QString btnStyle = "QPushButton {"
                       "background-color: #2ecc40;"
                       "color: black;"
                       "font-weight: bold;"
                       "border-radius: 8px;"
                       "padding: 6px 0;"
                       "margin-bottom: 6px;"
                       "min-height: 32px;"
                       "}"
                       "QPushButton:pressed { background-color: #27ae36; }";
    projectsBtn->setStyleSheet(btnStyle);
    createBtn->setStyleSheet(btnStyle);
    openBtn->setStyleSheet(btnStyle);
    examplesBtn->setStyleSheet(btnStyle);

    leftPanel->addWidget(createBtn);
    leftPanel->addWidget(openBtn);
    leftPanel->addSpacing(18);
    leftPanel->addWidget(projectsBtn);
    leftPanel->addWidget(examplesBtn);
    leftPanel->addStretch();

    auto *leftWidget = new QWidget;
    leftWidget->setLayout(leftPanel);
    leftWidget->setFixedWidth(170);
    leftWidget->setStyleSheet("background: #232323; border-radius: 12px;");

    // Right part (project/example list)
    auto *rightPanel = new QVBoxLayout();
    rightPanel->setSpacing(14);


    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: white; margin-bottom: 8px;");
    rightPanel->addWidget(titleLabel);

    auto *projectList = new QListWidget();
    // Use a folder icon for each project
    QIcon folderIcon = QIcon("assets/app/icons/iconFolder.png");
    projectList->setStyleSheet(
        "QListWidget {"
        "color: white;"
        "background-color: #353535;"
        "font-size: 14px;"
        "border-radius: 8px;"
        "padding: 6px;"
        "}"
        "QListWidget::item {"
        "margin-bottom: 6px;"
        "border-bottom: 1px solid #444;"
        "}");
    projectList->setFixedWidth(600); 
    projectList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    projectList->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    rightPanel->addWidget(projectList);
    rightPanel->addStretch();

    auto *rightWidget = new QWidget;
    rightWidget->setLayout(rightPanel);
    rightWidget->setFixedWidth(640);
    rightWidget->setStyleSheet("background: #232323; border-radius: 12px;");

    // Centering and height adjustments
    auto *centerLayout = new QHBoxLayout;
    centerLayout->addStretch();
    centerLayout->addWidget(leftWidget);
    centerLayout->addSpacing(30);
    centerLayout->addWidget(rightWidget);
    centerLayout->addStretch();

    // Set fixed height for panels
    int panelHeight = 340;
    leftWidget->setFixedHeight(panelHeight);
    rightWidget->setFixedHeight(panelHeight);

    // Replace mainLayout with centerLayout
    delete layout();
    setLayout(centerLayout);

    // put recent projects in the projectsData attribute
    listRecentProjects();
    this->examplesData = {
        {"Cornell Box", "/home/mathis/Programming/RaytracingEngine/saves/exampleScenes/CornellBox.json"}};

    // Use a folder icon for each project

    // TODO : make this in listRecentProjects function
    auto loadProjectList = [projectList, folderIcon](const QList<QPair<QString, QString>> &data)
    {
        projectList->clear();
        for (const auto &pair : data)
        {
            projectList->addItem(new QListWidgetItem(folderIcon, pair.first + "\n" + pair.second));
        }
    };



    loadProjectList(this->projectsData);

    connect(projectsBtn, &QPushButton::clicked, this, [this, loadProjectList]()
            { loadProjectList(this->projectsData); });
    connect(examplesBtn, &QPushButton::clicked, this, [this, loadProjectList]()
            { loadProjectList(this->examplesData); });

    // Add slot for loading a project
    connect(openBtn, &QPushButton::clicked, this, [this]()
            {
                loadProject();
            });

    // connect project selection to openMainWindow
    connect(projectList, &QListWidget::itemClicked, this, &MenuWindow::openMainWindow); // TODO change to loadProject from the path of the item selected


}

void MenuWindow::openMainWindow()
{
    auto *mainWin = new MainWindow();
    mainWin->show();
    this->close();
}

void MenuWindow::loadProject()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open Project", "saves/userSaves", "JSON Files (*.json)");
    if (!filePath.isEmpty()) {
        emit loadProject(filePath);
        openMainWindow();
    } 
}

void MenuWindow::openProjectFromPath(const QString &projectPath){
    emit loadProject(projectPath);
    openMainWindow();
}


void MenuWindow::listRecentProjects()
{
    // list all JSON file in recentsaves.JSON that contains all the filePath used in the app
    QDir dir("saves/userSaves");
    QFileInfoList files = dir.entryInfoList(QStringList() << "*.json", QDir::Files);
    std::sort(files.begin(), files.end(), [](const QFileInfo &a, const QFileInfo &b) {
        return a.lastModified() > b.lastModified();
    });
    for (const QFileInfo &fileInfo : files) {
        QString projectName = fileInfo.completeBaseName();
        this->projectsData.append(qMakePair(projectName, fileInfo.absoluteFilePath()));
    }
}