#include <QApplication>
#include <QStandardItemModel>
#include <QFile>
#include <QTextStream>

#include "FreezeTableview.h"

int main(int argc, char* argv[])
{
    Q_INIT_RESOURCE(grades);

    QApplication app( argc, argv );
    QStandardItemModel *model=new QStandardItemModel();

    QFile file(":/grades.txt");
    if (file.open(QFile::ReadOnly)) {
        QTextStream stream(&file);

        QString line = stream.readLine();
        QStringList list = line.simplified().split(',');
        model->setHorizontalHeaderLabels(list);

        int row = 0;
        QStandardItem *newItem = nullptr;
        while (!stream.atEnd()) {
            line = stream.readLine();
            if (!line.startsWith('#') && line.contains(',')) {
                list = line.simplified().split(',');
                for (int col = 0; col < list.length(); ++col){
                    newItem = new QStandardItem(list.at(col));
                    model->setItem(row, col, newItem);
                }
                ++row;
            }
        }
    }
    file.close();

    FreezeSortTableView *tableView = new FreezeSortTableView(model);
    tableView->setSortAble(true);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setAlternatingRowColors(true);

    tableView->setWindowTitle(QObject::tr("Frozen Column Example"));
    tableView->resize(560, 680);
    tableView->show();
    return app.exec();
}

