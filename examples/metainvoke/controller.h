#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>

class QtMethodInvoker;
class BasicController : public QObject
{
    Q_OBJECT
public:
    explicit BasicController(QObject *parent = Q_NULLPTR);
    virtual ~BasicController();

    void invoke(const QString& command);
protected:
    void initialize();

private:
    QtMethodInvoker* invoker;
};


class DomainController : public BasicController
{
    Q_OBJECT
public:
    DomainController(QObject *parent = Q_NULLPTR);
    virtual ~DomainController();

    Q_CLASSINFO("update", "update")
    Q_INVOKABLE void update(const QString& value);

    Q_CLASSINFO("start", "start")
    Q_INVOKABLE void start(const QString& value);

    Q_CLASSINFO("stop", "stop")
    Q_INVOKABLE void stop(const QString& value);
};

#endif // CONTROLLER_H
