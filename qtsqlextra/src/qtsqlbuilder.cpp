#include "qtsqlbuilder.h"
#include "qtsqlutils.h"
#include <QtSql>
#include <QString>


QtSqlBuilder::QtSqlBuilder(const QSqlDatabase &db) : database(db)
{
}

QtSqlBuilder::~QtSqlBuilder()
{
}

QString QtSqlBuilder::create(const QString &tableName,
                             const QSqlRecord &columns,
                             const QSet<QString> &indices) const
{
    QSqlDriver* driver = database.driver();
    if (!driver || tableName.isEmpty() || columns.isEmpty())
        return QString();

    QSqlField field;
    QString sql;
    sql.reserve(4096);

    sql += "CREATE TABLE IF NOT EXISTS ";
    sql += driver->escapeIdentifier(tableName, QSqlDriver::TableName);
    sql += '(';
    for (int i = 0, n = columns.count(); i < n; i++)
    {
        field = columns.field(i);
        sql += driver->escapeIdentifier(field.name(), QSqlDriver::FieldName) ;
        sql += ' ';
        sql += QtSql::typeName(field.type());
        if (field.length() > 0) {
            sql += '(';
            sql += QString::number(field.length());
            sql += ')';
        }
        if (indices.contains(field.name()))
            sql += " PRIMARY KEY";

        if (field.requiredStatus() == QSqlField::Required)
            sql += " NOT NULL";

        if (i < (n - 1))
            sql += ',';
    }
    sql += ')';
    return sql;
}

QString QtSqlBuilder::drop(const QString &tableName) const
{
    QSqlDriver* driver = database.driver();
    if (!driver)
        return QString();
    return ("DROP TABLE " + driver->escapeIdentifier(tableName, QSqlDriver::TableName));
}

bool QtSqlBuilder::contains(const QString &tableName,
                            QSql::TableType flag) const
{
    return database.tables(flag).contains(tableName);
}

QString QtSqlBuilder::insert(const QString &tableName,
                             const QSqlRecord &record,
                             bool prepared) const
{
    QSqlDriver* driver = database.driver();
    if (!driver)
        return QString();
    return driver->sqlStatement(QSqlDriver::InsertStatement, tableName, record, prepared);
}

QString QtSqlBuilder::update(const QString &tableName,
                             const QSqlRecord &record,
                             bool prepared,
                             const QString &predicate) const
{
    QSqlDriver* driver = database.driver();
    if (!driver)
        return QString();
    QString sql = driver->sqlStatement(QSqlDriver::UpdateStatement, tableName, record, prepared);
    if (!predicate.isEmpty()) {
        sql += ' ';
        sql += predicate;
    }
    return sql;
}

QString QtSqlBuilder::select(const QString &tableName,
                             const QSqlRecord &columns,
                             const QString &predicate) const
{
    QSqlDriver* driver = database.driver();
    if (!driver)
        return QString();
    QString sql = driver->sqlStatement(QSqlDriver::SelectStatement, tableName, columns, false);
    if (!predicate.isEmpty()) {
        sql += ' ';
        sql += predicate;
    }
    return sql;
}


QString QtSqlBuilder::select(const QString &tableName,
                             const QSqlRecord &columns,
                             const QSqlRecord &predicate) const
{
    QSqlDriver* driver = database.driver();
    if (!driver)
        return QString();
    QString sql;
    sql.reserve(4096);
    sql += driver->sqlStatement(QSqlDriver::SelectStatement, tableName, columns, false);
    sql += ' ';
    sql += driver->sqlStatement(QSqlDriver::WhereStatement, tableName, predicate, false);
    return sql;
}

QString QtSqlBuilder::remove(const QString &tableName,
                             const QSqlRecord &predicate) const
{
    QSqlDriver* driver = database.driver();
    if (!driver)
        return QString();
    return driver->sqlStatement(QSqlDriver::DeleteStatement, tableName, predicate, false);
}

QString QtSqlBuilder::remove(const QString &tableName,
                             const QString &predicate) const
{
    QSqlDriver* driver = database.driver();
    if (!driver)
        return QString();

    QString sql;
    sql.reserve(4096);
    sql += "DELETE * FROM ";
    sql += driver->escapeIdentifier(tableName, QSqlDriver::TableName);
    sql += ' ';
    sql += predicate;
    return sql;
}

QString QtSqlBuilder::prepare(const QString &sql,
                              const QVariantMap &values,
                              Qt::CaseSensitivity cs) const
{
    QString prepared = sql;
    QSqlDriver* driver = database.driver();
    for (auto it = values.begin(); it != values.end(); ++it) {
        prepared.replace(it.key(), QtSql::formatValue(it.value(), driver), cs);
    }
    return prepared;
}

