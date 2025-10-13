#ifndef SCENEPANEL_H
#define SCENEPANEL_H

#include <QWidget>
#include <QTreeWidget>

class ScenePanel : public QWidget
{
    Q_OBJECT

public:
    explicit ScenePanel(QWidget *parent = nullptr);

private:
    void setupUI();
    QTreeWidget *sceneTree;
};

#endif // SCENEPANEL_H