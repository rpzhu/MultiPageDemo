#include "PagingWidget.h"
#include "ui_PagingWidget.h"
#include <QIntValidator>
PagingWidget::PagingWidget(uint32_t maxPage, QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::PagingWidget)
{
    ui->setupUi(this);

    this->Init();
    this->SetCurrentPage(this->CurrentPage_);
    this->SetMaxPage(maxPage);
}

PagingWidget::~PagingWidget()
{
    delete ui;
}

void
PagingWidget::Init()
{
    this->ui->Line_CurrentPage->setValidator(new QIntValidator(1, 10000000, this));
    this->ui->label_Previous->setProperty("page", "true");
    this->ui->label_Next->setProperty("page", "true");
    this->ui->label_Left->setProperty("page", "true");
    this->ui->label_Right->setProperty("page", "true");
    this->ui->label_Left->installEventFilter(this);
    this->ui->label_Right->installEventFilter(this);
    this->ui->label_Previous->installEventFilter(this);
    this->ui->label_Next->installEventFilter(this);
    this->setStyleSheet("QLabel[page=\"true\"] {  padding: 1px; } QLabel[currentPage=\"true\"] {color: rgb(190, 0, 0);}"
                        "QLabel[page=\"true\"]:hover { color: white;border-radius: 4px;background-color: qlineargradient(spread:reflect, x1:0, y1:0, x2:0, y2:1,stop:0 rgba(53, 121, 238, 255),stop:1 rgba(0, 202, 237, 255));}"
                        "QPushButton { color: red }");
}

void
PagingWidget::SetMaxPage(uint32_t maxPage)
{
    if (maxPage < 1)
        return;
    this->MaxPage_ = maxPage;
    //更新总页数label
    this->ui->label_MaxPage->setText("/" + QString::number(this->MaxPage_));

    //最大页数变动时检测当前页是否需要变化
    if (CurrentPage_ > maxPage)
        this->SetCurrentPage(maxPage);
}

bool
PagingWidget::eventFilter(QObject* watched, QEvent* e)
{
    if (e->type() == QEvent::MouseButtonRelease) {
        int page = -1;
        if (watched == this->ui->label_Previous)
            page = this->CurrentPage_ - 1;
        else if (watched == this->ui->label_Next)
            page = this->CurrentPage_ + 1;
        else if (watched == this->ui->label_Left)
            page = 1;
        else if (watched == this->ui->label_Right)
            page = this->MaxPage_;

        if (page > 0) {
            this->SetCurrentPage(page);
            return true;
        }
    }
    return QWidget::eventFilter(watched, e);
}

void
PagingWidget::SetCurrentPage(uint32_t newPage)
{
    this->CurrentPage_ = newPage > this->MaxPage_ ? this->MaxPage_ : newPage;
    this->ui->Line_CurrentPage->setText(QString::number(this->CurrentPage_));
    //通知变化
    emit this->SignalCurrentPageChanged(this->CurrentPage_);
}

void
PagingWidget::on_Line_CurrentPage_returnPressed()
{
    int page = this->ui->Line_CurrentPage->text().toInt();
    if (page > 0 && page != this->CurrentPage_)
        this->SetCurrentPage(page);
}

void
PagingWidget::on_Line_CurrentPage_editingFinished()
{
}
