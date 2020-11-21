#ifndef QTBUTTONLOCKER_H
#define QTBUTTONLOCKER_H

#include <QObject>
#include <QIcon>

#include <QtWidgetsExtra>

class QAbstractButton;

class QTWIDGETSEXTRA_EXPORT QtButtonLocker : public QObject
{
    Q_OBJECT
public:
    explicit QtButtonLocker(QObject *parent = Q_NULLPTR);
    ~QtButtonLocker();

    void setIcon(const QIcon& icon);
    QIcon icon() const;

    // QObject interface
public:
    bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;

protected:
    virtual void lock(QAbstractButton* button);
    virtual void unlock(QAbstractButton* button);

private:
    QT_PIMPL(QtButtonLocker)
};

#endif // QTBUTTONLOCKER_H
