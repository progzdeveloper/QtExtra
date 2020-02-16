#include "qtsqlconnectiondialog.h"
#include <QtConcurrent/QtConcurrentRun>
#include <QFuture>
#include <QFutureWatcher>

#include <QCoreApplication>
#include <QMessageBox>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QBoxLayout>

#include <QtSqlConnectionEdit>

class QtSqlConnectionDialogPrivate
{
    Q_DECLARE_TR_FUNCTIONS(QtSqlConnectionDialogPrivate)
public:
    QtSqlConnectionDialog* q_ptr;
    QtSqlConnectionEdit* edit;
    QPushButton* acceptButton;
    QPushButton* rejectButton;
    QPushButton* testButton;
    QProgressBar* progressBar;
    QLabel* progressLabel;
    QFutureWatcher<void>* watcher;

    QtSqlConnectionDialogPrivate(QtSqlConnectionDialog* q);
    void initUi();
    void lock();
    void unlock();
};

QtSqlConnectionDialogPrivate::QtSqlConnectionDialogPrivate(QtSqlConnectionDialog* q) :
    q_ptr(q)
{
}

void QtSqlConnectionDialogPrivate::initUi()
{
    watcher = new QFutureWatcher<void>(q_ptr);

    progressBar = new QProgressBar(q_ptr);
    progressBar->setRange(0, 0);
    progressBar->hide();

    progressLabel = new QLabel(q_ptr);
    progressLabel->hide();

    QObject::connect(watcher, SIGNAL(started()), progressBar, SLOT(show()));
    QObject::connect(watcher, SIGNAL(started()), progressLabel, SLOT(show()));

    QObject::connect(watcher, SIGNAL(finished()), progressBar, SLOT(hide()));
    QObject::connect(watcher, SIGNAL(finished()), progressLabel, SLOT(hide()));

    edit = new QtSqlConnectionEdit(q_ptr);
    QObject::connect(edit, SIGNAL(testSucceeded()), q_ptr, SLOT(success()));
    QObject::connect(edit, SIGNAL(testFailed(QString)), q_ptr, SLOT(fail(QString)));

    QObject::connect(edit, SIGNAL(reportWarning(QString)), q_ptr, SLOT(warning(QString)));
    QObject::connect(edit, SIGNAL(reportError(QString)), q_ptr, SLOT(error(QString)));


    acceptButton = new QPushButton(tr("Apply"), q_ptr);
    QObject::connect(acceptButton, SIGNAL(clicked()), q_ptr, SLOT(accept()));

    rejectButton = new QPushButton(tr("Cancel"), q_ptr);
    QObject::connect(rejectButton, SIGNAL(clicked()), q_ptr, SLOT(reject()));

    testButton = new QPushButton(tr("Test..."), q_ptr);
    QObject::connect(testButton, SIGNAL(clicked()), q_ptr, SLOT(test()));

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(testButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(acceptButton);
    buttonLayout->addWidget(rejectButton);

    QVBoxLayout* mainLayout = new QVBoxLayout(q_ptr);
    mainLayout->addWidget(progressLabel);
    mainLayout->addWidget(progressBar);
    mainLayout->addWidget(edit);
    mainLayout->addLayout(buttonLayout);
}

void QtSqlConnectionDialogPrivate::lock()
{
    edit->setEnabled(false);
    acceptButton->setEnabled(false);
    rejectButton->setEnabled(false);
}

void QtSqlConnectionDialogPrivate::unlock()
{
    edit->setEnabled(true);
    acceptButton->setEnabled(true);
    rejectButton->setEnabled(true);
}


QtSqlConnectionDialog::QtSqlConnectionDialog(QWidget *parent, Qt::WindowFlags flags) :
    QDialog(parent, flags), d_ptr(new QtSqlConnectionDialogPrivate(this))
{
    Q_D(QtSqlConnectionDialog);
    d->initUi();
    adjustSize();
    setFixedHeight(height());
}

QtSqlConnectionDialog::~QtSqlConnectionDialog()
{
}

void QtSqlConnectionDialog::setConnectionName(const QString &name)
{
    Q_D(QtSqlConnectionDialog);
    d->edit->setConnectionName(name);
}

QString QtSqlConnectionDialog::connectionName() const
{
    Q_D(const QtSqlConnectionDialog);
    return d->edit->connectionName();
}

void QtSqlConnectionDialog::setDatabaseName(const QString &databaseName)
{
    Q_D(QtSqlConnectionDialog);
    d->edit->setDatabaseName(databaseName);
}

QString QtSqlConnectionDialog::databaseName() const
{
    Q_D(const QtSqlConnectionDialog);
    return d->edit->databaseName();
}

void QtSqlConnectionDialog::setDriverName(const QString &driverName)
{
    Q_D(QtSqlConnectionDialog);
    d->edit->setDriverName(driverName);
}

QString QtSqlConnectionDialog::driverName() const
{
    Q_D(const QtSqlConnectionDialog);
    return d->edit->driverName();
}

void QtSqlConnectionDialog::setUserName(const QString &userName)
{
    Q_D(QtSqlConnectionDialog);
    d->edit->setUserName(userName);
}

QString QtSqlConnectionDialog::userName() const
{
    Q_D(const QtSqlConnectionDialog);
    return d->edit->userName();
}

void QtSqlConnectionDialog::setPassword(const QString &password)
{
    Q_D(QtSqlConnectionDialog);
    d->edit->setPassword(password);
}

QString QtSqlConnectionDialog::password() const
{
    Q_D(const QtSqlConnectionDialog);
    return d->edit->password();
}

void QtSqlConnectionDialog::setHostName(const QString& hostName)
{
    Q_D(QtSqlConnectionDialog);
    d->edit->setHostName(hostName);
}

QString QtSqlConnectionDialog::hostName() const
{
    Q_D(const QtSqlConnectionDialog);
    return d->edit->hostName();
}

void QtSqlConnectionDialog::setPort(int port)
{
    Q_D(QtSqlConnectionDialog);
    d->edit->setPort(port);
}

int QtSqlConnectionDialog::port() const
{
    Q_D(const QtSqlConnectionDialog);
    return d->edit->port();
}

void QtSqlConnectionDialog::setOptions(const QString& options)
{
    Q_D(QtSqlConnectionDialog);
    d->edit->setOptions(options);
}

QString QtSqlConnectionDialog::options() const
{
    Q_D(const QtSqlConnectionDialog);
    return d->edit->options();
}

void QtSqlConnectionDialog::test()
{
    Q_D(QtSqlConnectionDialog);
    d->progressLabel->setText(tr("Connecting with '%1'...").arg(d->edit->connectionName()));
    d->watcher->setFuture(QtConcurrent::run(d->edit, &QtSqlConnectionEdit::test));
    d->lock();
    adjustSize();
}

void QtSqlConnectionDialog::success()
{
    Q_D(QtSqlConnectionDialog);
    QMessageBox messageBox(this);
    messageBox.setWindowTitle(tr("Connection Test"));
    messageBox.setIcon(QMessageBox::Information);
    messageBox.setText(tr("Connection test is successfull.                  "));
    messageBox.exec();

    d->unlock();
}

void QtSqlConnectionDialog::fail(const QString &message)
{
    Q_D(QtSqlConnectionDialog);
    QMessageBox messageBox(this);
    messageBox.setWindowTitle(tr("Connection Test"));
    messageBox.setIcon(QMessageBox::Critical);
    messageBox.setText(tr("Connection test is failed.                         "));
    messageBox.setDetailedText(message);
    messageBox.exec();

    d->unlock();
}

void QtSqlConnectionDialog::warning(const QString &message)
{
    Q_D(QtSqlConnectionDialog);
    QMessageBox::warning(this, tr("Warning"), message);

    d->unlock();
}

void QtSqlConnectionDialog::error(const QString &message)
{
    Q_D(QtSqlConnectionDialog);
    QMessageBox::critical(this, tr("Error"), message);

    d->unlock();
}


