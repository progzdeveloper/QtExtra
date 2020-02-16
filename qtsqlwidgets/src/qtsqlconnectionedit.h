#ifndef QTSQLCONNECTIONEDIT_H
#define QTSQLCONNECTIONEDIT_H

#include <QWidget>
#include <QtSqlWidgets>

class QSqlDatabase;

class QTSQLWIDGETS_EXPORT QtSqlConnectionEdit : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(QtSqlConnectionEdit)
public:

    enum Field {
        ConnectionName = 0,
        DriverName,
        DatabaseName,
        UserName,
        Password,
        HostName,
        Port,
        ConnectionOptions
    };

    explicit QtSqlConnectionEdit(QWidget *parent = Q_NULLPTR, Qt::WindowFlags flags = 0);
    ~QtSqlConnectionEdit();

    void setDatabase(const QSqlDatabase& db);

    bool isReadOnly() const;

    QString connectionName() const;
    QString databaseName() const;
    QString driverName() const;
    QString userName() const;
    QString password() const;
    QString hostName() const;
    int port() const;
    QString options() const;

    QVariant value(Field field);

    bool verifyInput();

public Q_SLOTS:
    void setReadOnly(bool on = true);

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
    void enableEchoMode(bool on);
    void editOptions();
    void driverChanged(const QString&);

Q_SIGNALS:
    void testFailed(const QString& message);
    void testSucceeded();
    void reportError(const QString& message);
    void reportWarning(const QString& message);

private:
    QT_PIMPL(QtSqlConnectionEdit)
};


#endif // QTSQLCONNECTIONEDIT_H
