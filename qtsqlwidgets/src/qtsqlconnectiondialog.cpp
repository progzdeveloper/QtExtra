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
    QtSqlConnectionDialog* q;
    QtSqlConnectionEdit* edit;
    QPushButton* acceptButton;
    QPushButton* rejectButton;
    QPushButton* testButton;
    QProgressBar* progressBar;
    QLabel* progressLabel;
    QFutureWatcher<void>* watcher;

    QtSqlConnectionDialogPrivate(QtSqlConnectionDialog* dialog);
    void initUi();
    void lock();
    void unlock();
};

QtSqlConnectionDialogPrivate::QtSqlConnectionDialogPrivate(QtSqlConnectionDialog* dialog) :
    q(dialog)
{
}

void QtSqlConnectionDialogPrivate::initUi()
{
    watcher = new QFutureWatcher<void>(q);

    progressBar = new QProgressBar(q);
    progressBar->setRange(0, 0);
    progressBar->hide();

    progressLabel = new QLabel(q);
    progressLabel->hide();

    QObject::connect(watcher, SIGNAL(started()), progressBar, SLOT(show()));
    QObject::connect(watcher, SIGNAL(started()), progressLabel, SLOT(show()));

    QObject::connect(watcher, SIGNAL(finished()), progressBar, SLOT(hide()));
    QObject::connect(watcher, SIGNAL(finished()), progressLabel, SLOT(hide()));

    edit = new QtSqlConnectionEdit(q);
    QObject::connect(edit, SIGNAL(testSucceeded()), q, SLOT(success()));
    QObject::connect(edit, SIGNAL(testFailed(QString)), q, SLOT(fail(QString)));

    QObject::connect(edit, SIGNAL(reportWarning(QString)), q, SLOT(warning(QString)));
    QObject::connect(edit, SIGNAL(reportError(QString)), q, SLOT(error(QString)));


    acceptButton = new QPushButton(tr("Apply"), q);
    QObject::connect(acceptButton, SIGNAL(clicked()), q, SLOT(accept()));

    rejectButton = new QPushButton(tr("Cancel"), q);
    QObject::connect(rejectButton, SIGNAL(clicked()), q, SLOT(reject()));

    testButton = new QPushButton(tr("Test..."), q);
    QObject::connect(testButton, SIGNAL(clicked()), q, SLOT(test()));

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(testButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(acceptButton);
    buttonLayout->addWidget(rejectButton);

    QVBoxLayout* mainLayout = new QVBoxLayout(q);
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
    QDialog(parent, flags), d(new QtSqlConnectionDialogPrivate(this))
{
    d->initUi();
    adjustSize();
    setFixedHeight(height());
}

QtSqlConnectionDialog::~QtSqlConnectionDialog()
{
}

void QtSqlConnectionDialog::setConnectionName(const QString &name)
{
    d->edit->setConnectionName(name);
}

QString QtSqlConnectionDialog::connectionName() const
{
    return d->edit->connectionName();
}

void QtSqlConnectionDialog::setDatabaseName(const QString &databaseName)
{
    d->edit->setDatabaseName(databaseName);
}

QString QtSqlConnectionDialog::databaseName() const
{
    return d->edit->databaseName();
}

void QtSqlConnectionDialog::setDriverName(const QString &driverName)
{
    d->edit->setDriverName(driverName);
}

QString QtSqlConnectionDialog::driverName() const
{
    return d->edit->driverName();
}

void QtSqlConnectionDialog::setUserName(const QString &userName)
{
    d->edit->setUserName(userName);
}

QString QtSqlConnectionDialog::userName() const
{
    return d->edit->userName();
}

void QtSqlConnectionDialog::setPassword(const QString &password)
{
    d->edit->setPassword(password);
}

QString QtSqlConnectionDialog::password() const
{
    return d->edit->password();
}

void QtSqlConnectionDialog::setHostName(const QString& hostName)
{
    d->edit->setHostName(hostName);
}

QString QtSqlConnectionDialog::hostName() const
{
    return d->edit->hostName();
}

void QtSqlConnectionDialog::setPort(int port)
{
    d->edit->setPort(port);
}

int QtSqlConnectionDialog::port() const
{
    return d->edit->port();
}

void QtSqlConnectionDialog::setOptions(const QString& options)
{
    d->edit->setOptions(options);
}

QString QtSqlConnectionDialog::options() const
{
    return d->edit->options();
}

void QtSqlConnectionDialog::test()
{
    d->progressLabel->setText(tr("Connecting with '%1'...").arg(d->edit->connectionName()));
    d->watcher->setFuture(QtConcurrent::run(d->edit, &QtSqlConnectionEdit::test));
    d->lock();
    adjustSize();
}

void QtSqlConnectionDialog::success()
{
    QMessageBox messageBox(this);
    messageBox.setWindowTitle(tr("Connection Test"));
    messageBox.setIcon(QMessageBox::Information);
    messageBox.setText(tr("Connection test is successfull.                  "));
    messageBox.exec();
    d->unlock();
}

void QtSqlConnectionDialog::fail(const QString &message)
{
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
    QMessageBox::warning(this, tr("Warning"), message);
    d->unlock();
}

void QtSqlConnectionDialog::error(const QString &message)
{
    QMessageBox::critical(this, tr("Error"), message);
    d->unlock();
}


