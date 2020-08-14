#ifndef PAGEWIDGET_H
#define PAGEWIDGET_H

#include <QWidget>
#include <QList>

class QLabel;
class QEvent;

namespace Ui {
    class PageWidget;
}

class PageWidget : public QWidget
{
    Q_OBJECT

signals:
    void SignalCurrentPageChanged(int page);

public:
    explicit PageWidget(int blockSize = 3, QWidget *parent = 0);
    ~PageWidget();

    int GetBlockSize() const;
    int GetMaxPage() const;
    int GetCurrentPage() const;

    // 其他组件只需要调用这两个函数即可
    void SetMaxPage(int maxPage);   // 当总页数改变时调用
    void SetCurrentPage(int currentPage, bool signalEmitted = true); // 修改当前页时调用

protected:
    virtual bool eventFilter(QObject *watched, QEvent *e)override;

private:
    void SetBlockSize(int blockSize);
    void UpdatePageLabels();
    void Init();

private:
    Ui::PageWidget *ui;

    //分组 每组三个
    int BlockSize_;
    //最大页数
    int MaxPage_;
    //当前页数
    int CurrentPage_;
    QList<QLabel *> PageLabels_;

};

#endif // PAGEWIDGET_H
