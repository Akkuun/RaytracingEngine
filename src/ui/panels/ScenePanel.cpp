#include "ScenePanel.h"
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QTreeWidgetItem>

ScenePanel::ScenePanel(QWidget *parent) : QWidget(parent)
{
    setupUI();
}

void ScenePanel::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    // Create tree widget
    sceneTree = new QTreeWidget(this);
    sceneTree->setHeaderHidden(true);
    sceneTree->setIndentation(20);

    // Simple, clean styling
    sceneTree->setStyleSheet(
        "QTreeWidget {"
        "    background-color: transparent;"
        "    color: white;"
        "    border: none;"
        "}"
        "QTreeWidget::item {"
        "    padding: 3px;"
        "}"
        "QTreeWidget::item:hover {"
        "    background-color: rgba(64, 64, 64, 150);"
        "}"
        "QTreeWidget::item:selected {"
        "    background-color: rgba(80, 80, 80, 200);"
        "}");

    // Create root items with checkboxes
    QTreeWidgetItem *environmentRoot = new QTreeWidgetItem(sceneTree);
    environmentRoot->setText(0, "Environment");
    environmentRoot->setFlags(environmentRoot->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsAutoTristate);
    environmentRoot->setCheckState(0, Qt::Checked);
    environmentRoot->setExpanded(true);

    QTreeWidgetItem *objectsRoot = new QTreeWidgetItem(sceneTree);
    objectsRoot->setText(0, "Objects");
    objectsRoot->setFlags(objectsRoot->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsAutoTristate);
    objectsRoot->setCheckState(0, Qt::Checked);
    objectsRoot->setExpanded(true);

    // Environment children
    QStringList envItems = {"BG SKY", "WORLD", "WALLS", "FLOOR", "LIGHT"};
    for (const QString &item : envItems)
    {
        QTreeWidgetItem *child = new QTreeWidgetItem(environmentRoot);
        child->setText(0, item);
        child->setFlags(child->flags() | Qt::ItemIsUserCheckable);
        child->setCheckState(0, Qt::Checked);
    }

    // Walls subcategory (3rd child of Environment)
    QTreeWidgetItem *wallsItem = environmentRoot->child(2);
    wallsItem->setFlags(wallsItem->flags() | Qt::ItemIsAutoTristate);
    wallsItem->setExpanded(true);

    QStringList wallItems = {"WALL 1", "WALL 2", "WALL BG"};
    for (const QString &wall : wallItems)
    {
        QTreeWidgetItem *wallChild = new QTreeWidgetItem(wallsItem);
        wallChild->setText(0, wall);
        wallChild->setFlags(wallChild->flags() | Qt::ItemIsUserCheckable);
        wallChild->setCheckState(0, Qt::Checked);
    }

    // Objects children
    QTreeWidgetItem *spheresItem = new QTreeWidgetItem(objectsRoot);
    spheresItem->setText(0, "SPHERES");
    spheresItem->setFlags(spheresItem->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsAutoTristate);
    spheresItem->setCheckState(0, Qt::Checked);
    spheresItem->setExpanded(true);

    for (int i = 1; i <= 2; ++i)
    {
        QTreeWidgetItem *sphere = new QTreeWidgetItem(spheresItem);
        sphere->setText(0, QString("Sphere %1").arg(i));
        sphere->setFlags(sphere->flags() | Qt::ItemIsUserCheckable);
        sphere->setCheckState(0, Qt::Checked);
    }

    layout->addWidget(sceneTree);
}