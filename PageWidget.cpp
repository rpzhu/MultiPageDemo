#include "PageWidget.h"
#include "ui_PageWidget.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QtGlobal>
PageWidget::PageWidget(int blockSize, QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::PageWidget)
{
    ui->setupUi(this);
    SetBlockSize(blockSize);
    this->Init();

    MaxPage_ = 0;
    SetMaxPage(1);
}

PageWidget::~PageWidget()
{
    delete ui;
    PageLabels_.clear();
}

bool
PageWidget::eventFilter(QObject* watched, QEvent* e)
{
    //找到当前点击的是哪个页面label，并更新页面
    if (e->type() == QEvent::MouseButtonRelease) {
        int page = -1;
        if (watched == ui->previousPageLabel)
            page = GetCurrentPage() - 1;
        if (watched == ui->nextPageLabel)
            page = GetCurrentPage() + 1;

        for (int i = 0; i < PageLabels_.count(); ++i) {
            if (watched == PageLabels_.at(i)) {
                page = PageLabels_.at(i)->text().toInt();
                break;
            }
        }

        if (page > 0) {
            this->SetCurrentPage(page, true);
            return true;
        }
    }
    //跳转至该页面
    if (watched == ui->pageLineEdit && e->type() == QEvent::KeyRelease) {
        QKeyEvent* ke = static_cast<QKeyEvent*>(e);
        if (ke->key() == Qt::Key_Enter || ke->key() == Qt::Key_Return) {
            this->SetCurrentPage(ui->pageLineEdit->text().toInt(), true);
            return true;
        }
    }
    //继续传递事件
    return QWidget::eventFilter(watched, e);
}

int
PageWidget::GetBlockSize() const
{
    return BlockSize_;
}

int
PageWidget::GetMaxPage() const
{
    return MaxPage_;
}

int
PageWidget::GetCurrentPage() const
{
    return CurrentPage_;
}

void
PageWidget::SetMaxPage(int maxPage)
{
    maxPage = qMax(maxPage, 1);

    if (MaxPage_ != maxPage) {
        this->MaxPage_ = maxPage;
        this->CurrentPage_ = 1;
        UpdatePageLabels();
    }
}

void
PageWidget::SetCurrentPage(int currentPage, bool signalEmitted)
{
    //确保不越界
    currentPage = qMax(currentPage, 1);
    currentPage = qMin(currentPage, MaxPage_);

    if (currentPage != this->CurrentPage_) {
        this->CurrentPage_ = currentPage;
        //更新每个页面label样式
        UpdatePageLabels();
        //通知页面已经改变
        if (signalEmitted)
            emit this->SignalCurrentPageChanged(currentPage);
    }
}

void
PageWidget::SetBlockSize(int blockSize)
{
    // 为了便于计算, block size 必须是奇数, 且最小为3
    blockSize = qMax(blockSize, 3);
    if (blockSize % 2 == 0)
        ++blockSize;
    this->BlockSize_ = blockSize;
}

// 初始化页码的labels
// 分成三个部分, 左...中...右
void
PageWidget::Init()
{
    ui->pageLineEdit->installEventFilter(this);
    ui->pageLineEdit->setValidator(new QIntValidator(1, 10000000, this));

    ui->nextPageLabel->setProperty("page", "true");
    ui->previousPageLabel->setProperty("page", "true");
    ui->nextPageLabel->installEventFilter(this);
    ui->previousPageLabel->installEventFilter(this);

    PageLabels_.clear();

    QHBoxLayout* leftLayout = new QHBoxLayout();
    QHBoxLayout* centerLayout = new QHBoxLayout();
    QHBoxLayout* rightLayout = new QHBoxLayout();
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(0);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    centerLayout->setSpacing(0);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);

    for (int i = 0; i < BlockSize_ * 3; ++i) {
        QLabel* label = new QLabel(QString::number(i + 1));
        label->setProperty("page", "true");
        label->installEventFilter(this);
        PageLabels_.push_back(label);
        if (i < BlockSize_)
            leftLayout->addWidget(label);
        else if (i < BlockSize_ * 2)
            centerLayout->addWidget(label);
        else
            rightLayout->addWidget(label);
    }

    ui->leftPagesWidget->setLayout(leftLayout);
    ui->centerPagesWidget->setLayout(centerLayout);
    ui->rightPagesWidget->setLayout(rightLayout);
}

void
PageWidget::UpdatePageLabels()
{
    ui->leftSeparateLabel->hide();
    ui->rightSeparateLabel->hide();
    if (MaxPage_ <= BlockSize_ * 3) {
        for (int i = 0; i < PageLabels_.count(); i += 1) {
            QLabel* label = PageLabels_.at(i);

            if (i < MaxPage_) {
                label->setText(QString::number(i + 1));
                label->show();
            } else
                label->hide();

            if (CurrentPage_ - 1 == i)
                label->setProperty("currentPage", "true");
            else
                label->setProperty("currentPage", "false");

            label->setStyleSheet("/**/");
        }
        return;
    }

    // 以下情况为maxPageNumber大于blockSize * 3, 所有的页码label都要显示
    // c 为 currentPage
    // n 为 block size
    // m 为 maxPage

    // 1. c ∈ [1, n + n/2 + 1]: 显示前 n * 2 个, 后 n 个: 只显示右边的分隔符
    // 2. c ∈ [m - n - n/2, m]: 显示前 n 个, 后 n * 2 个: 只显示左边的分隔符
    // 3. 显示[1, n], [c - n/2, c + n/2], [m - 2*n + 1, m]: 两个分隔符都显示

    int c = CurrentPage_;
    int n = BlockSize_;
    int m = MaxPage_;
    int centerStartPage = 0;

    if (c >= 1 && c <= n + n / 2 + 1) {
        // 1. c ∈ [1, n + n/2 + 1]: 显示前 n * 2 个, 后 n 个: 只显示右边的分隔符
        centerStartPage = n + 1;
        ui->rightSeparateLabel->show();
    } else if (c >= m - n - n / 2 && c <= m) {
        // 2. c ∈ [m - n - n/2, m]: 显示前 n 个, 后 n * 2 个: 只显示左边的分隔符
        centerStartPage = m - n - n + 1;
        ui->leftSeparateLabel->show();
    } else {
        // 3. 显示[1, n], [c - n/2, c + n/2], [m - n + 1, m]: 两个分隔符都显示
        centerStartPage = c - n / 2;
        ui->rightSeparateLabel->show();
        ui->leftSeparateLabel->show();
    }

    for (int i = 0; i < n; ++i) {
        PageLabels_.at(i)->setText(QString::number(i + 1));                   // 前面 n 个
        PageLabels_.at(n + i)->setText(QString::number(centerStartPage + i)); // 中间 n 个
        PageLabels_.at(3 * n - i - 1)->setText(QString::number(m - i));       // 后面 n 个
    }

    for (int i = 0; i < PageLabels_.count(); ++i) {
        QLabel* label = PageLabels_.at(i);
        int page = label->text().toInt();
        if (page == CurrentPage_)
            label->setProperty("currentPage", "true");
        else
            label->setProperty("currentPage", "false");

        label->setStyleSheet("/**/");
        label->show();
    }
}
