#ifndef QTDATERANGE_H
#define QTDATERANGE_H

#include <QObject>
#include <QDateTime>

#include <QtWidgetsExtra>

class QDateTime;

class QTWIDGETSEXTRA_EXPORT QtDateRange : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QDateTime lower READ lower WRITE setLower)
    Q_PROPERTY(QDateTime upper READ upper WRITE setUpper)
    Q_PROPERTY(int step READ step WRITE setStep NOTIFY stepChanged)
    Q_PROPERTY(int span READ span WRITE setSpan NOTIFY spanChanged)

public:
    enum SpanType {
        Undefined = 0,
        Hour,
        Day,
        Week,
        Month,
        Quarter,
        Year
    };
    Q_ENUM(SpanType)

    explicit QtDateRange(QObject *parent = Q_NULLPTR);
    QtDateRange(const QDateTime& lower, const QDateTime& upper, QObject* parent = Q_NULLPTR);
    ~QtDateRange();

    const QDateTime& lower() const;
    const QDateTime& upper() const;

    bool isSingle() const;

    int step() const;
    int span() const;

    QDateTime adjusted(const QDateTime& dt, int amount) const;

    QString intervalText() const;

public Q_SLOTS:
    void setStep(int step);
    void setLower(const QDateTime& dt);
    void setUpper(const QDateTime& dt);
    void setSpan(int span);
    void reset();

Q_SIGNALS:
    void rangeChanged(const QDateTime& lower, const QDateTime& upper);
    void stepChanged(int);
    void spanChanged(int);

private:
    QDateTime mLower, mUpper;
    SpanType mSpan;
    int mStep;
};

#endif // QTDATERANGE_H
