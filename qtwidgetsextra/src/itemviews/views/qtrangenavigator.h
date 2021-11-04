#ifndef QTRANGENAVIGATOR_H
#define QTRANGENAVIGATOR_H

#include <QWidget>

#include <QtWidgetsExtra>

class QAbstractButton;

class QTWIDGETSEXTRA_EXPORT QtRangeNavigator :
        public QWidget
{
    Q_OBJECT

    Q_PROPERTY(int minimum READ minimum WRITE setMinimum)
    Q_PROPERTY(int maximum READ maximum WRITE setMaximum)
    Q_PROPERTY(int current READ current WRITE setCurrent NOTIFY indexChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)

public:
    enum ActionRole
    {
        MoveFirst = 0,
        MovePrev,
        MoveNext,
        MoveLast,
        MaxRole = MoveLast + 1
    };

    explicit QtRangeNavigator(QWidget *parent = Q_NULLPTR);
    QtRangeNavigator(int minimum, int maximum, QWidget* parent = Q_NULLPTR);
    virtual ~QtRangeNavigator();

    QString text() const;

    QPair<int, int> range() const;

    int minimum() const;
    int maximum() const;

    bool isAvailable(int i) const;
    QAbstractButton* button(int role) const;

    int current() const;

public Q_SLOTS:
    void setText(const QString& prefix);
    void setRange(int minimum, int maximum);
    void setMinimum(int value);
    void setMaximum(int value);

    virtual void setCurrent(int index);

    virtual void moveNext() {
        setCurrent(current() + 1);
    }

    virtual void movePrev() {
        setCurrent(current() - 1);
    }

    virtual void moveFirst() {
        setCurrent(minimum());
    }

    virtual void moveLast() {
        setCurrent(maximum());
    }


Q_SIGNALS:
    void indexChanged(int);
    void textChanged(const QString&);
    void rangeChanged(int, int);

private Q_SLOTS:
    void valueChanged(int index);
    void buttonClicked(int role);


private:
    QT_PIMPL(QtRangeNavigator)
};

#endif // QTRANGENAVIGATOR_H
