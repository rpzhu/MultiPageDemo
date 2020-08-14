#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "PagingWidget.h"

class PageWidget;

namespace Ui {
    class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void changeMaxPage();

private:
    Ui::Widget *ui;
    PageWidget *pageWidget;

    PagingWidget *PagingWidget_ = nullptr;

};

#endif // WIDGET_H
