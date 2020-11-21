#ifndef QTDATERANGEEDIT_H
#define QTDATERANGEEDIT_H

#include <QWidget>
#include <QtWidgetsExtra>

class QtDateRange;
class QTWIDGETSEXTRA_EXPORT QtDateSpanEdit : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int span READ span WRITE setSpan NOTIFY spanChanged)
    Q_PROPERTY(int amount READ amount WRITE setAmount NOTIFY amountChanged)
public:
    explicit QtDateSpanEdit(QtDateRange* range, QWidget* parent = Q_NULLPTR);
    ~QtDateSpanEdit();

    int span() const;
    int amount() const;

public Q_SLOTS:
    void setSpan(int span);
    void setAmount(int value);

private Q_SLOTS:
    void indexChanged(int index);

Q_SIGNALS:
    void amountChanged(int);
    void spanChanged(int);
private:
    QT_PIMPL(QtDateSpanEdit)
};



class QTWIDGETSEXTRA_EXPORT QtDateRangeEdit : public QWidget
{
    Q_OBJECT

public:
    explicit QtDateRangeEdit(QWidget *parent = Q_NULLPTR);
    ~QtDateRangeEdit();

    QDateTime lowerDateTime() const;
    QDateTime upperDateTime() const;

    QDate lowerDate() const;
    QDate upperDate() const;

private Q_SLOTS:
    void setupRange(const QDateTime& lower, const QDateTime& upper);
    void setupSpan(int span);
    void setCurrentDate();
    void setCurrentTime();
    void setCurrentDateTime();
    void resetTime();

Q_SIGNALS:
    void rangeChanged(const QDateTime& lower, const QDateTime& upper);

private:
    QT_PIMPL(QtDateRangeEdit)
};

#endif // QTDATERANGEEDIT_H
