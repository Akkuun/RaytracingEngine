#include "CollapsiblePanel.h"

CollapsiblePanel::CollapsiblePanel(const QString &title, QWidget *parent)
    : QWidget(parent), collapsed(false)
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(2);

    toggleButton = new QPushButton(title, this);
    toggleButton->setCheckable(true);
    toggleButton->setStyleSheet(
        "QPushButton { "
        "text-align: left; "
        "padding: 5px; "
        "background-color: #2b2b2b; "
        "color: white; "
        "border: 1px solid #444; "
        "border-radius: 3px; "
        "}");

    contentWidget = new QWidget(this);

    mainLayout->addWidget(toggleButton);
    mainLayout->addWidget(contentWidget);

    connect(toggleButton, &QPushButton::clicked, this, &CollapsiblePanel::toggleCollapse);
}

void CollapsiblePanel::setContent(QWidget *content)
{
    QVBoxLayout *layout = new QVBoxLayout(contentWidget);
    layout->setContentsMargins(5, 5, 5, 5);
    layout->addWidget(content);
}

void CollapsiblePanel::toggleCollapse()
{
    collapsed = !collapsed;
    contentWidget->setVisible(!collapsed);
    toggleButton->setText(collapsed ? "▶ " + toggleButton->text().mid(2) : "▼ " + toggleButton->text().mid(2));
}