#ifndef QTSWITCHBUTTON_H
#define QTSWITCHBUTTON_H

#include <QAbstractButton>
#include <QtWidgetsExtra>

class QTWIDGETSEXTRA_EXPORT QtSwitchButton : public QAbstractButton
{
public:
    QtSwitchButton(QWidget* parent);
    ~QtSwitchButton();

    // QWidget interface
protected:
    virtual void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

    // QAbstractButton interface
protected:
    virtual bool hitButton(const QPoint &pos) const Q_DECL_OVERRIDE;

private:
    QT_PIMPL(QtSwitchButton)
};

#endif // QTSWITCHBUTTON_H
