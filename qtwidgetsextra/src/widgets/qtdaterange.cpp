#include "qtdaterange.h"

QtDateRange::QtDateRange(QObject *parent) :
    QObject(parent), mSpan(Undefined), mStep(-1)
{
    reset();
}

QtDateRange::QtDateRange(const QDateTime &lower, const QDateTime &upper, QObject *parent) :
    QObject(parent), mLower(lower), mUpper(upper), mSpan(Undefined), mStep(-1)
{
}

QtDateRange::~QtDateRange()
{
}

void QtDateRange::setStep(int step)
{
    if (mStep != step) {
        mStep = step;
        mLower = ((mStep == 0) ? mUpper : adjusted(mUpper, mStep));
        Q_EMIT rangeChanged(mLower, mUpper);
    }
}

int QtDateRange::step() const
{
    return mStep;
}

void QtDateRange::reset()
{
    mSpan = Undefined;
    mLower = mUpper = QDateTime::currentDateTime();
    Q_EMIT rangeChanged(mLower, mUpper);
}

void QtDateRange::setLower(const QDateTime &dt)
{
    if (!dt.isValid())
        return;

    if (mLower == dt)
        return;

    mLower = dt;
    if (mSpan != Undefined) {
        mUpper = adjusted(dt, +qAbs(mStep));
    }

    Q_EMIT rangeChanged(mLower, mUpper);
}

const QDateTime &QtDateRange::lower() const
{
    return mLower;
}

void QtDateRange::setUpper(const QDateTime &dt)
{
    if (!dt.isValid())
        return;

    if (mUpper == dt)
        return;

    mUpper = dt;
    if (mSpan != Undefined) {
        mLower = adjusted(dt, -qAbs(mStep));
    }

    Q_EMIT rangeChanged(mLower, mUpper);
}

const QDateTime &QtDateRange::upper() const
{
    return mUpper;
}

bool QtDateRange::isSingle() const
{
    return (mLower == mUpper);
}

void QtDateRange::setSpan(int span)
{
    mSpan = (SpanType)span;
    if (span != Undefined) {
        mLower = adjusted(mUpper, mStep);
        Q_EMIT rangeChanged(mLower, mUpper);
    }
    Q_EMIT spanChanged(span);
}

int QtDateRange::span() const
{
    return (int)mSpan;
}

QDateTime QtDateRange::adjusted(const QDateTime &dt, int amount) const
{
    switch (mSpan) {
    case Hour:
        return dt.addSecs(((qint64)amount) * qint64(3600));
        break;
    case Day:
        return dt.addDays(amount).addSecs(amount > 0 ? -1 : 1);
        break;
    case Week:
        return dt.addDays(((qint64)amount) * qint64(7)).addSecs(amount > 0 ? -1 : 1);
        break;
    case Month:
        return dt.addMonths(amount).addSecs(amount > 0 ? -1 : 1);
        break;
    case Quarter:
        return dt.addMonths(4 * amount).addSecs(amount > 0 ? -1 : 1);
        break;
    case Year:
        return dt.addYears(amount).addSecs(amount > 0 ? -1 : 1);
    default:
        break;
    }
    return dt;
}

QString QtDateRange::intervalText() const
{
    static const qint64 SecsPerDay = 86400;

    QString text;
    if (mLower == mUpper) {
        text += tr("none");
    } else {
        qint64 seconds = mLower.secsTo(mUpper);
        qint64 days = seconds / SecsPerDay;
        if (days > 0) {
            if (days >= 365) {
                int years = days / 365;
                text += QString::number(years);
                text += tr(" years ");
                days = days % 365;
            }
            text += QString::number(days);
            text += tr(" days ");
        }

        QTime t(0, 0, 0);
        t = t.addSecs(seconds % SecsPerDay);

        text += QString::number(t.hour());
        text += tr(" hours ");

        text += QString::number(t.minute());
        text += tr(" minutes ");

        text += QString::number(t.second());
        text += tr(" seconds");
    }
    return text;
}


