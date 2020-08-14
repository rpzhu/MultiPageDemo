#include "TableWidgetForm.h"
#include "Widget.h"
#include <QApplication>
#include <QFile>

void
LoadPageQss(QApplication& a)
{
    QString qss;
    QFile file(":/qss/pagewidget");
    //    QString path = "C:\\MyDev\\RCSoft\\MultiPageDemo\\MultiPageDemo\\qss\\pagewidget.qss";
    //    file.setFileName(path);
    file.open(QFile::ReadOnly);
    if (file.isOpen()) {
        qss = QLatin1String(file.readAll());
        a.setStyleSheet(qss);
        file.close();
    }
}

int
main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    // 分页组件的css,这个内容应该放到普通文件中然后加载
    //    QString qss = QString(".QLabel[page=\"true\"] { padding: 1px; }")
    //            + QString(".QLabel[currentPage=\"true\"] { color: rgb(190, 0, 0);}")
    //            + QString(".QLabel[page=\"true\"]:hover { color: white; border-radius: 4px; background-color: qlineargradient(spread:reflect, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(53, 121, 238, 255), stop:1 rgba(0, 202, 237, 255));}");
    //    a.setStyleSheet(qss);

    //  LoadPageQss(a);

    TableWidgetForm* form = new TableWidgetForm();
    form->InitTable(5, 100);
    form->show();
    int ret = a.exec();
    delete form;
    return ret;
}
