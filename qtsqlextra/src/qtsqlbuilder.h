#ifndef QTSQLBUILDER_H
#define QTSQLBUILDER_H

#include <QSet>
#include <QString>
#include <QVariant>

#include <QSql>
#include <QSqlDatabase>

#include <QtSqlExtra>

class QSqlDriver;
class QSqlRecord;

class QTSQLEXTRA_EXPORT QtSqlBuilder
{
public:
    QtSqlBuilder(const QSqlDatabase& db);
    ~QtSqlBuilder();

    QString create(const QString& tableName,
                   const QSqlRecord& columns,
                   const QSet<QString>& indices) const;

    QString drop(const QString& tableName) const;

    bool contains(const QString& tableName,
                  QSql::TableType flag = QSql::AllTables) const;

    QString insert(const QString& tableName,
                   const QSqlRecord& record,
                   bool prepared) const;

    QString update(const QString& tableName,
                   const QSqlRecord& record,
                   bool prepared,
                   const QString& predicate = QString()) const;

    QString select(const QString& tableName,
                   const QSqlRecord& columns,
                   const QString& predicate = QString()) const;

    QString select(const QString& tableName,
                   const QSqlRecord& columns,
                   const QSqlRecord& predicate) const;

    QString remove(const QString& tableName,
                   const QSqlRecord& predicate) const;

    QString remove(const QString& tableName,
                   const QString& predicate) const;

    QString prepare(const QString& sql,
                    const QVariantMap& values,
                    Qt::CaseSensitivity cs = Qt::CaseSensitive) const;

private:
    QSqlDatabase database;
};


#endif // QTSQLBUILDER_H
