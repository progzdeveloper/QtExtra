#ifndef QTSQLCONNECTIONDIALOG_H
#define QTSQLCONNECTIONDIALOG_H

#include <QDialog>
#include <QtSqlWidgets>

class QTSQLWIDGETS_EXPORT QtSqlConnectionDialog : public QDialog
{
    Q_OBJECT
public:
    QtSqlConnectionDialog(QWidget* parent = Q_NULLPTR, Qt::WindowFlags flags = 0);
    ~QtSqlConnectionDialog();

    QString connectionName() const;
    QString databaseName() const;
    QString driverName() const;
    QString userName() const;
    QString password() const;
    QString hostName() const;
    int port() const;
    QString options() const;

public Q_SLOTS:
    void setConnectionName(const QString& name);
    void setDatabaseName(const QString& databaseName);
    void setDriverName(const QString& driverName);
    void setUserName(const QString& userName);
    void setPassword(const QString& password);
    void setHostName(const QString& hostName);
    void setPort(int port);
    void setOptions(const QString& options);

    void test();

private Q_SLOTS:
    void success();
    void fail(const QString& message);
    void warning(const QString& message);
    void error(const QString& message);
private:
    QScopedPointer<class QtSqlConnectionDialogPrivate> d;
};

#endif // QTSQLCONNECTIONDIALOG_H
