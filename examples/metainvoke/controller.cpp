#include <QDebug>
#include <QtMethodInvoker>
#include "controller.h"

BasicController::BasicController(QObject *parent) :
    QObject(parent),
    invoker(new QtMethodInvoker)
{
}

BasicController::~BasicController()
{
    delete invoker;
}

void BasicController::invoke(const QString &command)
{
    if (!invoker->invoke(this, command, Q_ARG(QString, command))) {
        qCritical() << "failed to find command: " << command;
    }
}

void BasicController::initialize()
{
    invoker->setMetaObject(this->metaObject());
}

DomainController::DomainController(QObject *parent) :
    BasicController(parent)
{
    initialize();
}

DomainController::~DomainController()
{
}

void DomainController::update(const QString &value)
{
    qDebug() << Q_FUNC_INFO << value;
}

void DomainController::start(const QString &value)
{
    qDebug() << Q_FUNC_INFO << value;
}

void DomainController::stop(const QString &value)
{
    qDebug() << Q_FUNC_INFO << value;
}
