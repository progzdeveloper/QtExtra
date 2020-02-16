#include <algorithm>
#include <QApplication>
#include <QIcon>
#include <QSignalMapper>
#include <QToolButton>
#include <QSpinBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QDebug>

#include "qtrangenavigator.h"



class QtRangeNavigatorPrivate
{
    Q_DECLARE_TR_FUNCTIONS(QtRangeNavigatorPrivate)
public:
    static const char *buttonToolTips[QtRangeNavigator::MaxRole];
    //static const QIcon buttonIcons[QtRangeNavigator::MaxRole];

    QToolButton* buttons[QtRangeNavigator::MaxRole];
    QSignalMapper* signalMapper;
    QSpinBox* spinBox;
    QLabel* prefixLabel;
    QLabel* suffixLabel;
    QtRangeNavigator* q_ptr;


    QtRangeNavigatorPrivate(QtRangeNavigator* q);

    void initUi();
    void update();
};



QtRangeNavigatorPrivate::QtRangeNavigatorPrivate(QtRangeNavigator *q) :
    q_ptr(q)
{
}

void QtRangeNavigatorPrivate::initUi()
{
    static const char *buttonToolTips[QtRangeNavigator::MaxRole] = {
        QT_TR_NOOP("Go to first"), // MoveFirst
        QT_TR_NOOP("Go to previous"), // MovePrev
        QT_TR_NOOP("Go to next"), // MoveNext
        QT_TR_NOOP("Go to last"), // MoveLast
    };

    static QIcon buttonIcons[QtRangeNavigator::MaxRole] = {
        QIcon::fromTheme("go-first", QIcon(":/images/first")),
        QIcon::fromTheme("go-previous", QIcon(":/images/prev")),
        QIcon::fromTheme("go-next", QIcon(":/images/next")),
        QIcon::fromTheme("go-last", QIcon(":/images/last"))
    };

    signalMapper = new QSignalMapper(q_ptr);
    spinBox = new QSpinBox(q_ptr);
    spinBox->setRange(0, 0);
    spinBox->setToolTip(tr("Go to specified position"));
    spinBox->setStatusTip(tr("Move cusor to the specified position"));

    QObject::connect(spinBox, SIGNAL(valueChanged(int)),
                     q_ptr, SLOT(valueChanged(int)));

    memset(buttons, 0, sizeof(buttons));
    for (int role = QtRangeNavigator::MoveFirst; role < QtRangeNavigator::MaxRole; role++)
    {
        QToolButton* button = new QToolButton(q_ptr);
        button->setIcon(buttonIcons[role]);
        button->setToolTip(tr(buttonToolTips[role]));
        button->setAutoRaise(true);

        signalMapper->setMapping(button, role);
        QObject::connect(button, SIGNAL(clicked()), signalMapper, SLOT(map()));
        buttons[role] = button;
    }
    QObject::connect(signalMapper, SIGNAL(mapped(int)), q_ptr, SLOT(buttonClicked(int)));

    prefixLabel = new QLabel(q_ptr);
    suffixLabel = new QLabel(" / 0", q_ptr);

    QHBoxLayout* layout = new QHBoxLayout(q_ptr);
    layout->addWidget(buttons[QtRangeNavigator::MoveFirst]);
    layout->addWidget(buttons[QtRangeNavigator::MovePrev]);
    layout->addWidget(prefixLabel);
    layout->addWidget(spinBox);
    layout->addWidget(suffixLabel);
    layout->addWidget(buttons[QtRangeNavigator::MoveNext]);
    layout->addWidget(buttons[QtRangeNavigator::MoveLast]);

    layout->setContentsMargins(0, 0, 0, 0);
}

void QtRangeNavigatorPrivate::update()
{
    suffixLabel->setText(tr(" / %1").arg(spinBox->maximum()));

    bool noRange = (spinBox->minimum() == 0 && spinBox->maximum() == 0);
    q_ptr->setEnabled(!noRange);
    if (noRange)
        return;

    bool isFirst = (spinBox->value() == spinBox->minimum());
    buttons[QtRangeNavigator::MoveFirst]->setEnabled(!isFirst);
    buttons[QtRangeNavigator::MovePrev]->setEnabled(!isFirst);

    bool isLast = (spinBox->value() == spinBox->maximum());
    buttons[QtRangeNavigator::MoveLast]->setEnabled(!isLast);
    buttons[QtRangeNavigator::MoveNext]->setEnabled(!isLast);
}




QtRangeNavigator::QtRangeNavigator(QWidget *parent) :
    QWidget(parent), d_ptr(new QtRangeNavigatorPrivate(this))
{
    Q_D(QtRangeNavigator);
    d->initUi();
    d->spinBox->setRange(0, 0);
}

QtRangeNavigator::QtRangeNavigator(int minimum, int maximum, QWidget *parent) :
    QWidget(parent), d_ptr(new QtRangeNavigatorPrivate(this))
{
    Q_D(QtRangeNavigator);
    d->initUi();
    d->spinBox->setRange(minimum, maximum);
}

QtRangeNavigator::~QtRangeNavigator()
{
}

void QtRangeNavigator::setText(const QString &prefix)
{
    Q_D(QtRangeNavigator);
    if (prefix != d->prefixLabel->text()) {
        d->prefixLabel->setText(prefix);
        Q_EMIT textChanged(prefix);
    }
}

QString QtRangeNavigator::text() const
{
    Q_D(const QtRangeNavigator);
    return d->prefixLabel->text();
}

void QtRangeNavigator::setRange(int minimum, int maximum)
{
    Q_D(QtRangeNavigator);
    if (minimum > maximum) {
        std::swap(minimum, maximum);
    }

    if (d->spinBox->minimum() == (minimum + 1) &&
        d->spinBox->maximum() == maximum) {
        return;
    }

    int value = d->spinBox->value();
    d->spinBox->blockSignals(true);
    d->spinBox->setRange(minimum + 1, maximum);
    d->spinBox->setValue(qBound(d->spinBox->minimum(), value, d->spinBox->maximum()));
    d->spinBox->blockSignals(false);

    d->update();
    Q_EMIT rangeChanged(minimum, maximum);
}

QPair<int, int> QtRangeNavigator::range() const
{
    Q_D(const QtRangeNavigator);
    return qMakePair(d->spinBox->minimum() - 1, d->spinBox->maximum());
}

void QtRangeNavigator::setMinimum(int value)
{
    Q_D(QtRangeNavigator);
    d->spinBox->setMinimum(value + 1);
    d->update();
    Q_EMIT rangeChanged(value, d->spinBox->maximum());
}

int QtRangeNavigator::minimum() const
{
    Q_D(const QtRangeNavigator);
    return (d->spinBox->minimum() - 1);
}

void QtRangeNavigator::setMaximum(int value)
{
    Q_D(QtRangeNavigator);
    d->spinBox->setMaximum(value);
    d->update();
    Q_EMIT rangeChanged(d->spinBox->minimum() - 1, value);
}

int QtRangeNavigator::maximum() const
{
    Q_D(const QtRangeNavigator);
    return d->spinBox->maximum();
}

bool QtRangeNavigator::isAvailable(int i) const
{
    Q_D(const QtRangeNavigator);
    return ((i >= (d->spinBox->minimum() - 1)) &&
            (i < d->spinBox->maximum()));
}

QAbstractButton *QtRangeNavigator::button(int role) const
{
    Q_D(const QtRangeNavigator);
    if (role >= 0 && role < MaxRole)
        return d->buttons[role];
    return Q_NULLPTR;
}

void QtRangeNavigator::setCurrent(int index)
{
    Q_D(QtRangeNavigator);
    index = qBound(d->spinBox->minimum(), index, d->spinBox->maximum());
    if (index != d->spinBox->value()) {
        d->spinBox->blockSignals(true);
        d->spinBox->setValue(index);
        d->spinBox->blockSignals(false);
        d->update();
        Q_EMIT indexChanged(index - 1);
    }
}

void QtRangeNavigator::valueChanged(int index)
{
    Q_D(QtRangeNavigator);
    d->update();
    Q_EMIT indexChanged(index - 1);
}

void QtRangeNavigator::buttonClicked(int role)
{
    switch(role) {
    case MoveFirst:
        moveFirst();
        break;
    case MovePrev:
        movePrev();
        break;
    case MoveNext:
        moveNext();
        break;
    case MoveLast:
        moveLast();
        break;
    default:
        break;
    }
}

int QtRangeNavigator::current() const
{
    Q_D(const QtRangeNavigator);
    return d->spinBox->value();
}

