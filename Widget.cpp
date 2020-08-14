#include "Widget.h"
#include "PageWidget.h"
#include "PagingWidget.h"
#include "ui_Widget.h"
#include <QIntValidator>
Widget::Widget(QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::Widget)
{
    ui->setupUi(this);

    ui->lineEdit->setValidator(new QIntValidator(1, 10000000, this));

    // 把分页的组件加入到当前窗口
    pageWidget = new PageWidget();
    this->layout()->addWidget(pageWidget);
    PagingWidget_ = new PagingWidget();
    this->layout()->addWidget(PagingWidget_);

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(changeMaxPage()));
    connect(ui->lineEdit, SIGNAL(returnPressed()), this, SLOT(changeMaxPage()));
}

Widget::~Widget()
{
    delete ui;
}

void
Widget::changeMaxPage()
{
    pageWidget->SetMaxPage(ui->lineEdit->text().toInt());
    pageWidget->SetCurrentPage(1);

    PagingWidget_->SetMaxPage(ui->lineEdit->text().toInt());
}
