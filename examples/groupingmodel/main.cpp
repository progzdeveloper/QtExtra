#include "modelwidget.h"
#include <QApplication>
#include <QtSql>

static bool createExampleDatabase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
    if (!db.open()) {
        QMessageBox::critical(0, qApp->tr("Cannot open database"),
                              qApp->tr("Unable to establish a database connection.\n"
                                       "This example needs SQLite support. Please read "
                                       "the Qt SQL driver documentation for information how "
                                       "to build it.\n\n"
                                       "Click Cancel to exit."), QMessageBox::Cancel);
        return false;
    }

    QSqlQuery query;
    query.exec("create table people (id int primary key, firstname varchar(20), lastname varchar(20), timestamp datetime)");
    query.exec("insert into people values(101, 'Михаил', 'Иванов', '2021-12-12T08:09:01')");
    query.exec("insert into people values(102, 'Михаил', 'Петров', '2020-12-12T08:09:01')");
    query.exec("insert into people values(103, 'Михаил', 'Сидоров', '2021-12-12T08:09:02')");
    query.exec("insert into people values(104, 'Михаил', 'Смирнов', '2020-11-12T08:09:01')");
    query.exec("insert into people values(105, 'Михаил', 'Воробьев', '2021-11-12T08:09:01')");
    query.exec("insert into people values(106, 'Михаил', 'Орлов', '2020-11-12T08:09:01')");
    query.exec("insert into people values(107, 'Михаил', 'Медведев', '2021-12-11T08:09:01')");
    query.exec("insert into people values(108, 'Михаил', 'Волков', '2021-11-11T08:09:01')");

    query.exec("insert into people values(111, 'Юрий', 'Иванов', '2021-11-12T08:09:01')");
    query.exec("insert into people values(112, 'Юрий', 'Петров', '2021-12-12T08:09:01')");
    query.exec("insert into people values(113, 'Юрий', 'Сидоров', '2020-11-12T08:09:01')");
    query.exec("insert into people values(114, 'Юрий', 'Смирнов', '2020-11-12T08:09:01')");
    query.exec("insert into people values(115, 'Юрий', 'Воробьев', '2020-12-12T08:09:01')");
    query.exec("insert into people values(116, 'Юрий', 'Орлов', '2020-11-12T08:09:01')");
    query.exec("insert into people values(117, 'Юрий', 'Медведев', '2021-11-11T08:09:01')");
    query.exec("insert into people values(118, 'Юрий', 'Волков', '2021-11-11T08:09:01')");

    query.exec("insert into people values(121, 'Александр', 'Иванов', '2020-12-12T08:09:01')");
    query.exec("insert into people values(122, 'Александр', 'Петров', '2020-12-12T08:09:01')");
    query.exec("insert into people values(123, 'Александр', 'Сидоров', '2020-12-12T08:09:01')");
    query.exec("insert into people values(124, 'Александр', 'Смирнов', '2020-12-12T08:09:01')");
    query.exec("insert into people values(125, 'Александр', 'Воробьев', '2020-12-12T08:09:01')");
    query.exec("insert into people values(126, 'Александр', 'Орлов', '2020-12-12T08:09:01')");
    query.exec("insert into people values(127, 'Александр', 'Медведев', '2020-12-12T08:09:01')");
    query.exec("insert into people values(128, 'Александр', 'Волков', '2020-12-12T08:09:01')");

    return true;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    createExampleDatabase();

    ModelWidget w;
    w.show();

    return a.exec();
}
