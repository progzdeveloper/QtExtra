#include <QtGlobal>
#include <QApplication>

#include <QtGlobalExtra>
#include <QDebug>
#include <QFactoryInterface>
#include <QtSql>
#include <QtSqlUtils>
#include <QtSqlBuilder>

#include "qtsqlitembrowser.h"

#define qCountOf(x) (sizeof((x)) / sizeof((x[0])))


int createSampleDatabase()
{
    QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE", "sample");
    database.setDatabaseName("sample.sqlite");
    if (!database.open()) {
        qCritical() << "failed to open database: " << database.lastError().text();
        return 1;
    }

    QHash<QString, QString> scripts;


    QSet<QString> tables = database.tables(QSql::AllTables).toSet();
    for (auto it = scripts.begin(); it != scripts.end(); ++it)
    {
        QString tableName = it.key();
        QString sql = it.value();
        if (tables.contains(tableName))
            continue;

        QSqlQuery query(database);
        if (!query.exec(sql)) {
            qCritical() << "failed to execute SQL statement: " << sql << database.lastError().text();
            return 1;
        }
    }
    return 0;
}



int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    /*qDebug() << "Qt::className<QSqlDriver>():     " << Qt::className<QSqlDriver>();
    qDebug() << "Qt::className(&app):             " << Qt::className(&app);
    qDebug() << "Qt::className(app.metaObject()): " << Qt::className(app.metaObject());
    qDebug() << "";
    qDebug() << "Qt::metaObject<QCoreApplication>(): " << ((void*)Qt::metaObject<QCoreApplication>());
    qDebug() << "Qt::metaObject(&app):               " << ((void*)Qt::metaObject(&app));
    qDebug() << "";
    qDebug() << "Qt::inherits<QCoreApplication>(&app):        " << Qt::inherits<QCoreApplication>(&app);
    qDebug() << "Qt::inherits<QObject>(&app):                 " << Qt::inherits<QObject>(&app);
    qDebug() << "Qt::inherits<QSqlDriver>(&app):              " << Qt::inherits<QSqlDriver>(&app);
    qDebug() << "Qt::inherits<QObject>(Qt::metaObject(&app)): " << Qt::inherits<QObject>(Qt::metaObject(&app));

    qDebug() << "\n";
    qDebug() << "Installed ODBC Drivers";
    qDebug() << QtSql::ODBCDriversList();
    qDebug() << "\n";

    qDebug() << "MS Sql:                 " << QtSql::driverName("Ms Sql");
    qDebug() << "Microsoft SQL Server:   " << QtSql::driverName("Microsoft SQL Server");
    qDebug() << "OdBc:                   " << QtSql::driverName("OdBc");
    qDebug() << "My SQL:                 " << QtSql::driverName("My SQL");
    qDebug() << "MySql:                  " << QtSql::driverName("MySql");
    qDebug() << "Postgre Sql:            " << QtSql::driverName("Postgre Sql");
    qDebug() << "PSql:                   " << QtSql::driverName("PSql");
    qDebug() << "PostGreS:               " << QtSql::driverName("Postgre Sql");
    qDebug() << "Oracle:                 " << QtSql::driverName("Oracle");
    qDebug() << "oCi:                    " << QtSql::driverName("oCi");
    qDebug() << "Borland InterBase:      " << QtSql::driverName("Borland InterBase");
    qDebug() << "iBase:                  " << QtSql::driverName("iBase");
    qDebug() << "Sybase Adaptive Server: " << QtSql::driverName("Sybase Adaptive Server");
    qDebug() << "TDs:                    " << QtSql::driverName("TDs");
    qDebug() << "SQLite:                 " << QtSql::driverName("SQLite");
    qDebug() << "QSqLite:                " << QtSql::driverName("QSqLite");
    qDebug() << "qOci:                   " << QtSql::driverName("qOci");
    qDebug() << "IBM DB2                 " << QtSql::driverName("IBM DB2");

    qDebug() << "QODBC    " << QtSql::driverVendor("QODBC");
    qDebug() << "QODBC2   " << QtSql::driverVendor("QODBC2");
    qDebug() << "QODBC3   " << QtSql::driverVendor("QODBC3");
    qDebug() << "QMYSQL3  " << QtSql::driverVendor("QMYSQL3");
    qDebug() << "QOCI     " << QtSql::driverVendor("QOCI");
    qDebug() << "QOCI5    " << QtSql::driverVendor("QOCI5");
    qDebug() << "QSQLITE3 " << QtSql::driverVendor("QSQLITE3");
    qDebug() << "QSQLITE  " << QtSql::driverVendor("QSQLITE");
    qDebug() << "QPSQL    " << QtSql::driverVendor("QPSQL");
    qDebug() << "QPSQL9   " << QtSql::driverVendor("QPSQL9");
    qDebug() << "QTDS     " << QtSql::driverVendor("QPSQL9");
    qDebug() << "QIBASE   " << QtSql::driverVendor("QIBASE");
    qDebug() << "QDB2     " << QtSql::driverVendor("QDB2");

    QStringList drivers = QSqlDatabase::drivers();
    qDebug() << "Available SQL drivers:";
    for (auto it = drivers.begin(); it != drivers.end(); ++it) {
        qDebug() << qPrintable(*it) << '\t' << QtSql::driverVendor(*it);
    }
    qDebug() << "";*/

    QString sqlDriver = "QSQLITE";
    QSqlDatabase database = QSqlDatabase::addDatabase(sqlDriver, "testdb.sqlite");
    database.setDatabaseName("qtsqlext.sqlite");
    if (!database.open()) {
        qCritical() << "failed to open database: " << database.lastError().text();
        return 1;
    }

    uint8_t bytes[] = { 0x1, 0x2, 0x3, 0x4, 0x5, 0xff, 0xfe, 0xfc, 0xcc, 0xd, 0xa };
    /*QSqlDriver* driver = database.driver();
    qDebug() << "Format values according to '" << qPrintable(sqlDriver) << "' driver format";
    qDebug() << qPrintable(QtSql::formatValue(123, driver));
    qDebug() << qPrintable(QtSql::formatValue(1.23, driver));
    qDebug() << qPrintable(QtSql::formatValue("HelloWorld!", driver));
    qDebug() << qPrintable(QtSql::formatValue(QByteArray((const char*)bytes, qCountOf(bytes)), driver));
    qDebug() << qPrintable(QtSql::formatValue(QDateTime::currentDateTime(), driver));*/

    QString sql;
    QString tableName = "events";
    QSqlRecord record;
    record.append(QSqlField("timestamp", QVariant::DateTime));
    record.append(QSqlField("payload", QVariant::ByteArray));
    record.append(QSqlField("endpoint", QVariant::String));

    QtSqlBuilder builder(database);
    if (!builder.contains(tableName)) {
        sql = builder.create(tableName, record, QSet<QString>() << "timestamp");
        qDebug() << sql;
        QSqlQuery query(database);
        if (!query.exec(sql)) {
            qCritical() << "failed to execute SQL statement: " << sql << database.lastError().text();
            return 1;
        }
    }

    QSqlRecord event = record;
    event.setValue("timestamp",QDateTime::currentDateTime());
    event.setValue("payload", QByteArray((const char*)bytes, qCountOf(bytes)));
    event.setValue("endpoint", "https://mail.ru/collector/api/v1/121001");

    sql = builder.insert(tableName, event, false);
    qDebug() << sql;
    QSqlQuery query(database);
    if (!query.exec(sql)) {
        qCritical() << query.lastError().text();
        return 1;
    }

    sql = builder.select(tableName, record);
    query.setForwardOnly(true);
    if (!query.exec(sql)) {
        qCritical() << query.lastError().text();
        return 1;
    }

    /*while(query.next()) {
        qDebug() << query.record();
    }

    sql = builder.select(tableName, record, "order by timestamp desc limit 1");
    query.setForwardOnly(true);
    if (!query.exec(sql)) {
        qCritical() << query.lastError().text();
        return 1;
    }
    while(query.next()) {
        qDebug() << query.record();
    }*/

    database.close();

    createSampleDatabase();


    QtSqlItemBrowser browser;
    browser.show();

    return app.exec();
}
