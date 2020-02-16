#include <QtWidgets>

#include "qtdaterangeedit.h"
#include "qtdaterange.h"


class QtDateSpanEditPrivate
{
    Q_DECLARE_TR_FUNCTIONS(QtDateSpanEditPrivate)
public:
    QtDateSpanEdit* q_ptr;
    QSpinBox* amountEdit;
    QComboBox* spanEdit;

    QtDateSpanEditPrivate(QtDateSpanEdit* q);
    void initUi(QtDateRange* range);
    void setupRange(int span);
};


QtDateSpanEditPrivate::QtDateSpanEditPrivate(QtDateSpanEdit *q) :
    q_ptr(q)
{
}

void QtDateSpanEditPrivate::initUi(QtDateRange* range)
{
    amountEdit = new QSpinBox(q_ptr);
    amountEdit->setValue(range->step());
    QObject::connect(amountEdit, SIGNAL(valueChanged(int)),
                     range, SLOT(setStep(int)));

    spanEdit = new QComboBox(q_ptr);
    spanEdit->addItem(tr("Hours"),     QtDateRange::Hour);
    spanEdit->addItem(tr("Days"),      QtDateRange::Day);
    spanEdit->addItem(tr("Weeks"),     QtDateRange::Week);
    spanEdit->addItem(tr("Months"),    QtDateRange::Month);
    spanEdit->addItem(tr("Quarters"),  QtDateRange::Quarter);
    spanEdit->addItem(tr("Years"),     QtDateRange::Year);
    spanEdit->addItem(tr("Undefined"), QtDateRange::Undefined);
    QObject::connect(spanEdit, SIGNAL(currentIndexChanged(int)),
                     q_ptr, SLOT(indexChanged(int)));
    QObject::connect(q_ptr, SIGNAL(spanChanged(int)),
                     range, SLOT(setSpan(int)));

    QHBoxLayout* layout = new QHBoxLayout(q_ptr);
    layout->addWidget(amountEdit, 0);
    layout->addWidget(spanEdit, 1);
    layout->setContentsMargins(0, 0, 0, 0);
}

void QtDateSpanEditPrivate::setupRange(int span)
{
    int currentYear = QDate::currentDate().year();
    int days = QDate(1, 1, 1).daysTo(QDate::currentDate());
    switch (span) {
    case QtDateRange::Hour:
        amountEdit->setRange(-24 * days, -1);
        break;
    case QtDateRange::Day:
        amountEdit->setRange(-days, -1);
        break;
    case QtDateRange::Week:
        amountEdit->setRange(-(days / 7), -1);
        break;
    case QtDateRange::Month:
        amountEdit->setRange(-12 * currentYear, -1);
        break;
    case QtDateRange::Quarter:
        amountEdit->setRange(-3 * currentYear, -1);
        break;
    case QtDateRange::Year:
        amountEdit->setRange(-(currentYear - 1), -1);
        break;
    default:
        break;
    }
    qDebug() << "amountEdit [" << amountEdit->minimum() << ':' << amountEdit->maximum() << ']';
}

QtDateSpanEdit::QtDateSpanEdit(QtDateRange *range, QWidget *parent) :
    QWidget(parent), d_ptr(new QtDateSpanEditPrivate(this))
{
    Q_D(QtDateSpanEdit);
    d->initUi(range);
}

QtDateSpanEdit::~QtDateSpanEdit()
{
}

int QtDateSpanEdit::span() const
{
    Q_D(const QtDateSpanEdit);
    return d->spanEdit->currentData().toInt();
}

int QtDateSpanEdit::amount() const
{
    Q_D(const QtDateSpanEdit);
    return d->amountEdit->value();
}

void QtDateSpanEdit::setSpan(int span)
{
    Q_D(QtDateSpanEdit);
    d->spanEdit->setCurrentIndex(d->spanEdit->findData(span));
}

void QtDateSpanEdit::setAmount(int value)
{
    Q_D(QtDateSpanEdit);
    d->amountEdit->setValue(value);
}

void QtDateSpanEdit::indexChanged(int index)
{
    Q_D(QtDateSpanEdit);
    int span = d->spanEdit->itemData(index).toInt();
    d->amountEdit->setVisible(span != QtDateRange::Undefined);
    d->setupRange(span);
    Q_EMIT spanChanged(span);
}





class QtDateRangeEditPrivate
{
    Q_DECLARE_TR_FUNCTIONS(QtDateSpanEditPrivate)
public:
    QtDateRangeEdit* q_ptr;
    QtDateRange* range;
    QtDateSpanEdit* spanEdit;
    QDateTimeEdit* lowerEdit;
    QDateTimeEdit* upperEdit;
    QLabel* intervalLabel;

    QtDateRangeEditPrivate(QtDateRangeEdit* q);
    void initUi();
    void createActions(QLineEdit* edit, quintptr id);
    QDateTimeEdit* fromSender(QObject *sender);
};

QtDateRangeEditPrivate::QtDateRangeEditPrivate(QtDateRangeEdit *q) :
    q_ptr(q)
{
}

void QtDateRangeEditPrivate::initUi()
{
    QString dateTimeFormat = "yyyy-MM-dd hh:mm:ss";
    range = new QtDateRange(q_ptr);

    QObject::connect(range, SIGNAL(rangeChanged(QDateTime,QDateTime)),
                     q_ptr, SLOT(setupRange(QDateTime,QDateTime)));
    QObject::connect(range, SIGNAL(rangeChanged(QDateTime,QDateTime)),
                     q_ptr, SIGNAL(rangeChanged(QDateTime,QDateTime)));
    QObject::connect(range, SIGNAL(spanChanged(int)),
                     q_ptr, SLOT(setupSpan(int)));

    lowerEdit = new QDateTimeEdit(q_ptr);
    lowerEdit->setDisplayFormat(dateTimeFormat);
    lowerEdit->setCalendarPopup(true);
    lowerEdit->setDateTime(range->lower());
    createActions(lowerEdit->findChild<QLineEdit*>(), reinterpret_cast<quintptr>(lowerEdit));
    QObject::connect(lowerEdit, SIGNAL(dateTimeChanged(QDateTime)),
                     range, SLOT(setLower(QDateTime)));


    upperEdit = new QDateTimeEdit(q_ptr);
    upperEdit->setDisplayFormat(dateTimeFormat);
    upperEdit->setCalendarPopup(true);
    upperEdit->setDateTime(range->upper());
    createActions(upperEdit->findChild<QLineEdit*>(), reinterpret_cast<quintptr>(upperEdit));
    QObject::connect(upperEdit, SIGNAL(dateTimeChanged(QDateTime)),
                     range, SLOT(setUpper(QDateTime)));

    intervalLabel = new QLabel(q_ptr);
    intervalLabel->setText(range->intervalText());
    intervalLabel->setWordWrap(true);

    spanEdit = new QtDateSpanEdit(range, q_ptr);
    spanEdit->setSpan(QtDateRange::Day);

    range->setLower(QDateTime(QDate::currentDate().addDays(-1), QTime(0, 0, 0)));
    range->setSpan(QtDateRange::Day);

    QFormLayout* layout = new QFormLayout(q_ptr);
    layout->addRow(tr("Period:"), spanEdit);
    layout->addRow(tr("Lower:"), lowerEdit);
    layout->addRow(tr("Upper:"), upperEdit);
    layout->addRow(tr("Interval:"), intervalLabel);
}

void QtDateRangeEditPrivate::createActions(QLineEdit *edit, quintptr id)
{
    QScopedPointer<QMenu> menu( edit->createStandardContextMenu() );

    QList<QAction*> actions;

    QAction* currentDateAct = new QAction(tr("Current Date..."), edit);
    currentDateAct->setData(id);
    QObject::connect(currentDateAct, SIGNAL(triggered()), q_ptr, SLOT(setCurrentDate()));

    QAction* currentTimeAct = new QAction(tr("Current Time..."), edit);
    currentTimeAct->setData(id);
    QObject::connect(currentTimeAct, SIGNAL(triggered()), q_ptr, SLOT(setCurrentTime()));

    QAction* currentDateTimeAct = new QAction(tr("Current Date/Time..."), edit);
    currentDateTimeAct->setData(id);
    QObject::connect(currentDateTimeAct, SIGNAL(triggered()), q_ptr, SLOT(setCurrentDateTime()));

    QAction* resetTimeAct = new QAction(tr("Reset Time..."), edit);
    resetTimeAct->setData(id);
    QObject::connect(resetTimeAct, SIGNAL(triggered()), q_ptr, SLOT(resetTime()));

    QAction* separatorAct = new QAction(edit);
    separatorAct->setSeparator(true);
    actions << currentDateAct << currentTimeAct << currentDateTimeAct << resetTimeAct << separatorAct;
    actions << menu->actions();
    for (auto it = actions.begin(); it != actions.end(); ++it) {
        (*it)->setParent(edit);
    }

    edit->setContextMenuPolicy(Qt::ActionsContextMenu);
    edit->addActions(actions);
}

QDateTimeEdit *QtDateRangeEditPrivate::fromSender(QObject *sender)
{
    QAction* action = qobject_cast<QAction*>(sender);
    if (action == Q_NULLPTR)
        return Q_NULLPTR;

    return reinterpret_cast<QDateTimeEdit*>(action->data().toULongLong());
}






QtDateRangeEdit::QtDateRangeEdit(QWidget *parent)
    : QWidget(parent), d_ptr(new QtDateRangeEditPrivate(this))
{
    Q_D(QtDateRangeEdit);
    d->initUi();
}

QtDateRangeEdit::~QtDateRangeEdit()
{
}

QDateTime QtDateRangeEdit::lowerDateTime() const
{
    Q_D(const QtDateRangeEdit);
    return d->lowerEdit->dateTime();
}

QDateTime QtDateRangeEdit::upperDateTime() const
{
    Q_D(const QtDateRangeEdit);
    return d->upperEdit->dateTime();
}

QDate QtDateRangeEdit::lowerDate() const
{
    Q_D(const QtDateRangeEdit);
    return d->lowerEdit->date();
}

QDate QtDateRangeEdit::upperDate() const
{
    Q_D(const QtDateRangeEdit);
    return d->upperEdit->date();
}

void QtDateRangeEdit::setupRange(const QDateTime &lower, const QDateTime &upper)
{
    Q_D(QtDateRangeEdit);
    if (d->range->span() == QtDateRange::Undefined) {
        d->upperEdit->setMinimumDateTime(lower);
        d->lowerEdit->setMaximumDateTime(upper);
    }else {
        d->lowerEdit->clearMaximumDateTime();
        d->upperEdit->clearMinimumDateTime();
    }

    d->lowerEdit->blockSignals(true);
    d->lowerEdit->setDateTime(lower);
    d->lowerEdit->blockSignals(false);

    d->upperEdit->blockSignals(true);
    d->upperEdit->setDateTime(upper);
    d->upperEdit->blockSignals(false);

    d->intervalLabel->setText(d->range->intervalText());
}

void QtDateRangeEdit::setupSpan(int span)
{
    Q_D(QtDateRangeEdit);
    if (span == QtDateRange::Undefined) {
        d->lowerEdit->setMaximumDateTime(d->range->upper());
        d->upperEdit->setMinimumDateTime(d->range->lower());
    } else {
        d->lowerEdit->clearMaximumDateTime();
        d->upperEdit->clearMinimumDateTime();
    }
}

void QtDateRangeEdit::setCurrentDate()
{
    Q_D(QtDateRangeEdit);
    QDateTimeEdit* edit = d->fromSender(sender());
    if (edit != Q_NULLPTR) {
        edit->setDate(QDate::currentDate());
        edit->setTime(QTime(0, 0, 0));
    }
}

void QtDateRangeEdit::setCurrentTime()
{
    Q_D(QtDateRangeEdit);
    QDateTimeEdit* edit = d->fromSender(sender());
    if (edit != Q_NULLPTR) {
        edit->setTime(QTime::currentTime());
    }
}

void QtDateRangeEdit::setCurrentDateTime()
{
    Q_D(QtDateRangeEdit);
    QDateTimeEdit* edit = d->fromSender(sender());
    if (edit != Q_NULLPTR) {
        edit->setDateTime(QDateTime::currentDateTime());
    }
}

void QtDateRangeEdit::resetTime()
{
    Q_D(QtDateRangeEdit);
    QDateTimeEdit* edit = d->fromSender(sender());
    if (edit != Q_NULLPTR) {
        edit->setTime(QTime(0, 0, 0));
    }
}



