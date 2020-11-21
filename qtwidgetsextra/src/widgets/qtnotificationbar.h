#pragma once
#include <QFrame>
#include <QIcon>

#include <QtWidgetsExtra>

class QTWIDGETSEXTRA_EXPORT QtNotificationBar :
        public QFrame
{
    Q_OBJECT
    Q_DISABLE_COPY(QtNotificationBar)
public:
    explicit QtNotificationBar(QWidget *parent = Q_NULLPTR);
    ~QtNotificationBar();

    bool isAnimated() const;

public Q_SLOTS:
    void setAnimated(bool on = true);
    void showMessage(const QString& text, const QIcon& icon = QIcon(), bool closeable = true);
    void clearMessage();

private:
    void animate();

private:
    QT_PIMPL(QtNotificationBar)
};

