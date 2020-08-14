#ifndef PAGINGWIDGET_H
#define PAGINGWIDGET_H

#include <QWidget>

namespace Ui {
class PagingWidget;
}

class PagingWidget : public QWidget
{
    Q_OBJECT
signals:
    void SignalCurrentPageChanged(int page);

public:
    explicit PagingWidget(uint32_t maxPage = 1, QWidget *parent = 0);
    ~PagingWidget();

    void Init();

    void SetMaxPage(uint32_t maxPage);

protected:
    virtual bool eventFilter(QObject *watched, QEvent *e)override;
    //设置当前页
    void SetCurrentPage(uint32_t newPage);

private slots:
    void on_Line_CurrentPage_returnPressed();

    void on_Line_CurrentPage_editingFinished();

private:
    Ui::PagingWidget *ui;

    uint32_t MaxPage_ = 1;
    uint32_t CurrentPage_ = 1;

};

#endif // PAGINGWIDGET_H
