#include "TableWidgetForm.h"
#include "ui_TableWidgetForm.h"

TableWidgetForm::TableWidgetForm(QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::TableWidgetForm)
{
    ui->setupUi(this);
    this->PagingWidget_ = new PagingWidget();
    connect(PagingWidget_, &PagingWidget::SignalCurrentPageChanged, this, &TableWidgetForm::SlotCurrentPageChanged);
    this->layout()->addWidget(this->PagingWidget_);
    this->Init();
}

TableWidgetForm::~TableWidgetForm()
{
    delete ui;
    if (this->PagingWidget_) {
        disconnect(PagingWidget_, &PagingWidget::SignalCurrentPageChanged, this, &TableWidgetForm::SlotCurrentPageChanged);
        delete this->PagingWidget_;
        this->PagingWidget_ = nullptr;
    }
    this->ClearData();
}

void
TableWidgetForm::Init()
{
    QStringList headers;
    headers << QObject::tr("rid")
            << QObject::tr("userid")
            << QObject::tr("name");
    this->ui->tableWidget->setColumnCount(headers.size());                                          //列数量
    this->ui->tableWidget->setHorizontalHeaderLabels(headers);                                      //列名称
    this->ui->tableWidget->setAlternatingRowColors(true);                                           //行间隔色
    this->ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents); //自动列宽
    this->ui->tableWidget->installEventFilter(this);                                                // obj->installeventfilter(this), 即是对此对象源安装百了本对象(*this)的事件过滤器度
                                                                                                    //相应的，在这里表示在tablewidget对象安装了本对象（*this）的事件过滤器，看
}

//初始化表格数据
void
TableWidgetForm::InitTable(int rows, int totalRows)
{
    if (rows < 0 || totalRows < 0)
        return;
    //根据条件创建对应的表格数据
    int show = 1, count = 0;
    std::string tmpstr = std::to_string(show);
    ;
    for (int i = 0; i < totalRows; ++i) {
        if (count == rows) {
            tmpstr = std::to_string(++show);
            count = 0;
        }
        UserData* data = new UserData();
        data->RID = tmpstr;
        data->UserID = tmpstr;
        data->UserName = tmpstr;
        this->DataList_.push_back(data);
        count++;
    }

    //设置分页栏页面数据
    this->SetPages(rows, totalRows);
    this->SlotCurrentPageChanged(1);
}

void
TableWidgetForm::UpdateTable(int startRow, int endRow)
{
    this->ClearTable();
    int row = 0;
    for (int i = startRow; i < this->DataList_.size(); ++i) {
        if (i > endRow)
            break;
        UserData* data = this->DataList_.at(i);
        if (data) {
            this->ui->tableWidget->insertRow(row);
            this->UpdateRows(data, row);
            row++;
        }
    }
}

void
TableWidgetForm::UpdateRows(UserData* data, int row)
{
    if (!data || row < 0)
        return;
    int colindex = 0;
    this->ui->tableWidget->setItem(row, colindex++, new QTableWidgetItem(QString::fromStdString(data->RID)));
    this->ui->tableWidget->setItem(row, colindex++, new QTableWidgetItem(QString::fromStdString(data->UserID)));
    this->ui->tableWidget->setItem(row, colindex++, new QTableWidgetItem(QString::fromStdString(data->UserName)));
}

//设置分页栏信息
void
TableWidgetForm::SetPages(int rows, int totalRows)
{
    if (rows < 0 || totalRows < 0)
        return;
    int offset = (totalRows % rows) > 0 ? 1 : 0;
    //计算总页数
    this->Pages_ = totalRows / rows + offset;
    //总行数
    this->TotalRows_ = totalRows;
    //每页行数
    this->EachRows_ = rows;
    //设置分页栏最大页面数
    this->PagingWidget_->SetMaxPage(this->Pages_);
}

void
TableWidgetForm::ClearData()
{
    for (UserData* data : this->DataList_)
        if (data)
            delete data;
    this->DataList_.clear();
}

void
TableWidgetForm::ClearTable()
{
    //清空表格数据
    this->ui->tableWidget->setRowCount(0);
    this->ui->tableWidget->clearContents();
}

//当前页回调
void
TableWidgetForm::SlotCurrentPageChanged(int page)
{
    //根据当前页决定要显示数据范围
    int rowstart = (page - 1) * this->EachRows_;
    int rowend = page * this->EachRows_ - 1;
    //根据起点和终点行更新表格页面
    this->UpdateTable(rowstart, rowend);
}
