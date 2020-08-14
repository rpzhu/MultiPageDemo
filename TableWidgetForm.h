#ifndef TABLEWIDGETFORM_H
#define TABLEWIDGETFORM_H

#include <PagingWidget.h>
#include <QList>
#include <QWidget>
#include <vector>

namespace Ui {
class TableWidgetForm;
}

struct UserData
{
    std::string RID;
    std::string UserID;
    std::string UserName;
};

class TableWidgetForm : public QWidget
{
    Q_OBJECT

public:
    explicit TableWidgetForm(QWidget* parent = 0);
    ~TableWidgetForm();

    void Init();
    //每页多少行，一共多少行
    void InitTable(int rows, int totalRows);

protected:
    //更新表格
    void UpdateTable(int startRow, int endRow);

    //更新行
    void UpdateRows(UserData* data, int row);

    void SetPages(int rows, int totalRows);

    void ClearData();

    void ClearTable();

private slots:
    void SlotCurrentPageChanged(int page);

private:
    Ui::TableWidgetForm* ui;
    int TotalRows_ = 0;
    int Pages_ = 0;    //最大页数
    int EachRows_ = 0; //每页行数
    PagingWidget* PagingWidget_ = nullptr;
    QList<UserData*> DataList_;
};

#endif // TABLEWIDGETFORM_H
