#ifndef QTSLIDEBUTTON_H
#define QTSLIDEBUTTON_H

#include <QtWidgetsExtra>
#include <QAbstractButton>

class QtSlideButtonPrivate;
class QTWIDGETSEXTRA_EXPORT QtSlideButton :
        public QAbstractButton
{
    Q_OBJECT
public:
    QtSlideButton(QWidget *parent = 0);
    ~QtSlideButton();

    QSize sizeHint() const;

protected:
    bool hitButton(const QPoint& p) const;
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);

private:
    QT_PIMPL(QtSlideButton)
};

#endif // QTSLIDEBUTTON_H
