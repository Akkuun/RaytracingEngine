#pragma once

#include <QDoubleSpinBox>
#include <QWheelEvent>


// Source - https://stackoverflow.com/a
// Posted by emkey08, modified by community. See post 'Timeline' for change history
// Retrieved 2025-11-27, License - CC BY-SA 3.0

class CustomDoubleSpinBox : public QDoubleSpinBox {

    Q_OBJECT

public:

    CustomDoubleSpinBox(QWidget *parent = 0) : QDoubleSpinBox(parent) {
        setFocusPolicy(Qt::StrongFocus);
    }

protected:

    virtual void wheelEvent(QWheelEvent *event) {
        if (!hasFocus()) {
            event->ignore();
        } else {
            QDoubleSpinBox::wheelEvent(event);
        }
    }
};
