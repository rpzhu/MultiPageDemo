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
    setBlockSize(blockSize);
    initialize();

    maxPage = 0;
    setMaxPage(1);
}

PageWidget::~PageWidget()
{
    delete ui;
    delete pageLabels;
}

bool
PageWidget::eventFilter(QObject* watched, QEvent* e)
{
    if (e->type() == QEvent::MouseButtonRelease) {
        int page = -1;
        if (watched == ui->previousPageLabel) {
            page = getCurrentPage() - 1;
        }

        if (watched == ui->nextPageLabel) {
            page = getCurrentPage() + 1;
        }

        for (int i = 0; i < pageLabels->count(); ++i) {
            if (watched == pageLabels->at(i)) {
                page = pageLabels->at(i)->text().toInt();
                break;
            }
        }

        if (-1 != page) {
            setCurrentPage(page, true);
            return true;
        }
    }

    if (watched == ui->pageLineEdit && e->type() == QEvent::KeyRelease) {
        QKeyEvent* ke = static_cast<QKeyEvent*>(e);
        if (ke->key() == Qt::Key_Enter || ke->key() == Qt::Key_Return) {
            setCurrentPage(ui->pageLineEdit->text().toInt(), true);
            //setMaxPage(ui->pageLineEdit->text().toInt()); // 测试生成多个页码
            return true;
        }
    }

    return QWidget::eventFilter(watched, e);
}

int
PageWidget::getBlockSize() const
{
    return blockSize;
}

int
PageWidget::getMaxPage() const
{
    return maxPage;
}

int
PageWidget::getCurrentPage() const
{
    return currentPage;
}

void
PageWidget::setMaxPage(int page)
{
    page = qMax(page, 1);

    if (maxPage != page) {
        this->maxPage = page;
        this->currentPage = 1;
        updatePageLabels();
    }
}

void
PageWidget::setCurrentPage(int page, bool signalEmitted)
{
    page = qMax(page, 1);
    page = qMin(page, maxPage);

    if (page != this->currentPage) {
        this->currentPage = page;
        updatePageLabels();

        if (signalEmitted) {
            emit currentPageChanged(page);
        }
    }
}

void
PageWidget::setBlockSize(int blockSize)
{
    // 为了便于计算, block size 必须是奇数, 且最小为3
    blockSize = qMax(blockSize, 3);
    if (blockSize % 2 == 0) {
        ++blockSize;
    }
    this->blockSize = blockSize;
}

// 初始化页码的labels
// 分成三个部分, 左...中...右
void
PageWidget::initialize()
{
    ui->pageLineEdit->installEventFilter(this);
    ui->pageLineEdit->setValidator(new QIntValidator(1, 10000000, this));

    ui->nextPageLabel->setProperty("page", "true");
    ui->previousPageLabel->setProperty("page", "true");
    ui->nextPageLabel->installEventFilter(this);
    ui->previousPageLabel->installEventFilter(this);

    pageLabels = new QList<QLabel*>();

    QHBoxLayout* leftLayout = new QHBoxLayout();
    QHBoxLayout* centerLayout = new QHBoxLayout();
    QHBoxLayout* rightLayout = new QHBoxLayout();
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(0);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    centerLayout->setSpacing(0);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);

    for (int i = 0; i < blockSize * 3; ++i) {
        QLabel* label = new QLabel(QString::number(i + 1));
        label->setProperty("page", "true");
        label->installEventFilter(this);

        pageLabels->append(label);

        if (i < blockSize) {
            leftLayout->addWidget(label);
        } else if (i < blockSize * 2) {
            centerLayout->addWidget(label);
        } else {
            rightLayout->addWidget(label);
        }
    }

    ui->leftPagesWidget->setLayout(leftLayout);
    ui->centerPagesWidget->setLayout(centerLayout);
    ui->rightPagesWidget->setLayout(rightLayout);
}

void
PageWidget::updatePageLabels()
{
    ui->leftSeparateLabel->hide();
    ui->rightSeparateLabel->hide();

    if (maxPage <= blockSize * 3) {
        for (int i = 0; i < pageLabels->count(); i += 1) {
            QLabel* label = pageLabels->at(i);

            if (i < maxPage) {
                label->setText(QString::number(i + 1));
                label->show();
            } else {
                label->hide();
            }

            if (currentPage - 1 == i) {
                label->setProperty("currentPage", "true");
            } else {
                label->setProperty("currentPage", "false");
            }

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

    int c = currentPage;
    int n = blockSize;
    int m = maxPage;
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
        pageLabels->at(i)->setText(QString::number(i + 1));                   // 前面 n 个
        pageLabels->at(n + i)->setText(QString::number(centerStartPage + i)); // 中间 n 个
        pageLabels->at(3 * n - i - 1)->setText(QString::number(m - i));       // 后面 n 个
    }

    for (int i = 0; i < pageLabels->count(); ++i) {
        QLabel* label = pageLabels->at(i);
        int page = label->text().toInt();
        if (page == currentPage) {
            label->setProperty("currentPage", "true");
        } else {
            label->setProperty("currentPage", "false");
        }

        label->setStyleSheet("/**/");
        label->show();
    }
}
