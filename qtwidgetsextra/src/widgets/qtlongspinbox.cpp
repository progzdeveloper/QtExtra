#include <QLineEdit>
#include <QMessageBox>
#include <QStyleOptionSpinBox>
#include <QLocale>
#include <QValidator>
#include <QApplication>
#include "qtlongspinbox.h"

/**
 * \internal
 */
class QtLongSpinBoxPrivate  {
public:
    QString prefix;
    QString suffix;
    int radix;
    qlonglong value;
    qlonglong minimum;
    qlonglong maximum;
    qlonglong singlestep;
    QtLongSpinBox* q_ptr;

    QtLongSpinBoxPrivate(QtLongSpinBox *p);

    void updateEdit();
    void updateButtons();

    QString textFromValue(qlonglong value) const;
    qlonglong valueFromText(const QString &text) const;
};


QtLongSpinBoxPrivate::QtLongSpinBoxPrivate(QtLongSpinBox *p) :
    radix(10),
    value(0),
    minimum(0),
    maximum(99),
    singlestep(1),
    q_ptr(p)
{
}

void QtLongSpinBoxPrivate::updateEdit()
{
    QLineEdit *edit = q_ptr->lineEdit();
    QString textval = prefix + q_ptr->textFromValue(value) + suffix;
    const bool empty = edit->text().isEmpty();
    int cursor = edit->cursorPosition();
    const bool sb = edit->blockSignals(true);
    edit->setText(textval);
    cursor = qBound(prefix.size(), cursor, edit->displayText().size() - suffix.size());
    edit->setCursorPosition(empty ? prefix.size() : cursor);
    edit->blockSignals(sb);
    updateButtons();
}


void QtLongSpinBoxPrivate::updateButtons()
{
    QStyle* style = q_ptr->style();
    if (style == Q_NULLPTR)
        style = qApp->style();

    QStyleOptionSpinBox opt;
    opt.init(q_ptr);
    opt.activeSubControls = 0;
    opt.buttonSymbols = q_ptr->buttonSymbols();
    opt.subControls = QStyle::SC_SpinBoxUp | QStyle::SC_SpinBoxDown | QStyle::SC_SpinBoxFrame;
    opt.stepEnabled = q_ptr->stepEnabled();
    opt.frame = q_ptr->hasFrame();
    q_ptr->update(style->subControlRect(QStyle::CC_SpinBox, &opt, QStyle::SC_SpinBoxUp, q_ptr));
    q_ptr->update(style->subControlRect(QStyle::CC_SpinBox, &opt, QStyle::SC_SpinBoxDown, q_ptr));
}

QString QtLongSpinBoxPrivate::textFromValue(qlonglong value) const
{
    QString str;

    if (radix != 10) {
        const QLatin1String prefix = value < 0 ? QLatin1String("-") : QLatin1String();
        str = prefix + QString::number(qAbs(value), radix);
    } else {
        str = q_ptr->locale().toString(value);
        if (!q_ptr->isGroupSeparatorShown() && (qAbs(value) >= 1000 || value == LONG_LONG_MAX)) {
            str.remove(q_ptr->locale().groupSeparator());
        }
    }

    return str;
}

qlonglong QtLongSpinBoxPrivate::valueFromText(const QString &text) const
{
    QString tmp = text;
    if (!prefix.isEmpty() && tmp.startsWith(prefix)) {
        tmp.remove(0, prefix.size());
    }
    if (!suffix.isEmpty() && tmp.endsWith(suffix)) {
        tmp.chop(suffix.size());
    }
    bool isOk = false;
    return tmp.toLongLong(&isOk, radix);
}



class QtLongSpinBoxValidator : public QValidator {
public:
    QtLongSpinBoxValidator(QtLongSpinBox *sb) :
        QValidator(sb), spinbox(sb) {
    }
    void fixup ( QString & input ) const {
        spinbox->fixup(input);
    }
    State validate ( QString & input, int & pos ) const
    {
        if (!spinbox->prefix().isEmpty() && !input.startsWith(spinbox->prefix())) {
            input.prepend(spinbox->prefix());
        }
        if (!spinbox->suffix().isEmpty() && !input.endsWith(spinbox->suffix())) {
            input.append(spinbox->suffix());
        }
        return spinbox->validate(input, pos);
    }
private:
    QtLongSpinBox *spinbox;
};


/*!
 *  \class  QtLongSpinBox
 *  \brief  The QtLongSpinBox class provides a spinbox taking long long values.
 */
/*!
 * \fn      void QtLongSpinBox::valueChanged( qlonglong i )
 * \brief   This signal is emitted whenever value of the spin box is set to \a i
 */
/*!
 * \fn      void QtLongSpinBox::valueChanged( const QString & text )
 * \brief   This signal is emitted whenever the textual representation of the value changes to \a text.
 */

/*!
 * Constructs a long spin box widget with a given \a parent.
 *
 */
QtLongSpinBox::QtLongSpinBox(QWidget *parent) :
    QAbstractSpinBox(parent),
    d_ptr(new QtLongSpinBoxPrivate(this))
{
    Q_D(QtLongSpinBox);
    d->updateEdit();

    lineEdit()->setValidator(new QtLongSpinBoxValidator(this));
    connect(lineEdit(), SIGNAL(textChanged(const QString &)),
            this, SLOT(slotEditorTextChanged(const QString &)));
}

QtLongSpinBox::QtLongSpinBox(qlonglong minimum, qlonglong maximum, QWidget *parent) :
    QAbstractSpinBox(parent),
    d_ptr(new QtLongSpinBoxPrivate(this))
{
    Q_D(QtLongSpinBox);
    d->updateEdit();

    lineEdit()->setValidator(new QtLongSpinBoxValidator(this));
    connect(lineEdit(), SIGNAL(textChanged(const QString &)),
            this, SLOT(slotEditorTextChanged(const QString &)));

    this->blockSignals(true);
    setRange(minimum, maximum);
    this->blockSignals(false);
}

QtLongSpinBox::~QtLongSpinBox()
{
}

/*!
 * \property QtLongSpinBox::value
 * \brief Property holds the spinbox value.
 */
qlonglong QtLongSpinBox::value() const {
    Q_D(const QtLongSpinBox);
    return d->value;
}

/*!
 * \property QtLongSpinBox::minimum
 * \brief Property holds the minimum value allowed for the spinbox.
 */
qlonglong QtLongSpinBox::minimum() const {
    Q_D(const QtLongSpinBox);
    return d->minimum;
}

/*!
 * \property QtLongSpinBox::maximum
 * \brief Property holds the maximum value allowed for the spinbox.
 */
qlonglong QtLongSpinBox::maximum() const {
    Q_D(const QtLongSpinBox);
    return d->maximum;
}

/*!
 * \property QtLongSpinBox::prefix
 * \brief Property holds the spinbox's prefix.
 */
const QString & QtLongSpinBox::prefix() const {
    Q_D(const QtLongSpinBox);
    return d->prefix;
}

/*!
 * \property QtLongSpinBox::suffix
 * \brief Property holds the spinbox's suffix.
 */
const QString & QtLongSpinBox::suffix() const {
    Q_D(const QtLongSpinBox);
    return d->suffix;
}

/*!
 *  \brief  Returns textual representation of the \a value.
 */
QString QtLongSpinBox::textFromValue(qlonglong value) const {
    Q_D(const QtLongSpinBox);
    return d->textFromValue(value);
}

/*!
 *  \brief  Returns numerical representation of the \a text.
 */
qlonglong QtLongSpinBox::valueFromText(const QString & text) const {
    Q_D(const QtLongSpinBox);
    return d->valueFromText(text);
}

/*!
 *
 */
void QtLongSpinBox::setMaximum(qlonglong v)
{
    if (maximum()==v)
        return;
    Q_D(QtLongSpinBox);
    setRange(qMin(d->minimum, v), v);
}

/**
 *
 * @param v
 */
void QtLongSpinBox::setMinimum(qlonglong v)
{
    if (minimum()==v)
        return;
    Q_D(QtLongSpinBox);
    setRange(v, qMax(d->maximum, v));
}

/*!
 * \brief Sets the spinbox value to \a v
 */
void QtLongSpinBox::setValue(qlonglong v)
{
    v = qBound(minimum(), v, maximum());
    if (value() == v)
        return;
    Q_D(QtLongSpinBox);
    d->value = v;
    QString textval = d->prefix + textFromValue(v) + d->suffix;
    d->updateEdit();
    emit valueChanged(v);
    emit valueChanged(textval);
}

/**
 * @brief Sets the spinbox prefix
 * @param p
 */
void QtLongSpinBox::setPrefix(const QString &p)
{
    Q_D(QtLongSpinBox);
    d->prefix = p;
    d->updateEdit();
}

/**
 * @brief Sets the spinbox suffix
 * @param s
 */
void QtLongSpinBox::setSuffix(const QString &s)
{
    Q_D(QtLongSpinBox);
    d->suffix = s;
    d->updateEdit();
}

/*!
 * \property QtLongSpinBox::singleStep
 * \brief This property holds the step for the spinbox
 */
qlonglong QtLongSpinBox::singleStep() const {
    Q_D(const QtLongSpinBox);
    return d->singlestep;
}

/**
 * @brief   Sets the step size for the spinbox
 */
void QtLongSpinBox::setSingleStep(qlonglong s)
{
    if (s==0)
        return;
    Q_D(QtLongSpinBox);
    d->singlestep = s;
}

/*!
 * \brief   Sets the range of values allowed for the spinbox to \a min and \a max
 */
void QtLongSpinBox::setRange(qlonglong min, qlonglong max)
{
    Q_D(QtLongSpinBox);
    d->minimum = min;
    d->maximum = max;
    if (value()<d->minimum) {
        setValue(d->minimum);
    } else if (value()>d->maximum) {
        setValue(d->maximum);
    }
}

void QtLongSpinBox::setDisplayIntegerBase(int base)
{
    Q_D(QtLongSpinBox);
    // Falls back to base 10 on invalid bases (like QString)
    if (Q_UNLIKELY(base < 2 || base > 36)) {
        qWarning("QSpinBox::setDisplayIntegerBase: Invalid base (%d)", base);
        d->radix = 10;
    }

    if (base != d->radix) {
        d->radix = base;
        d->updateEdit();
    }
}

int QtLongSpinBox::displayIntegerBase() const
{
    Q_D(const QtLongSpinBox);
    return d->radix;
}


/*!
 * \brief   Changes the spinbox value by a number of \a steps
 */
void QtLongSpinBox::stepBy(int steps)
{
    if (steps == 0)
        return;

    qlonglong nvalue = value()+steps*singleStep();
    if (wrapping()) {
        qlonglong d = nvalue - maximum();
        qlonglong nd = nvalue - minimum();
        if (nvalue > value() && d > 0) {
            nvalue = minimum() + d - 1;
        } else if (nvalue < value() && nd < 0) {
            nvalue = maximum() + nd + 1;
        }
    } else {
        nvalue = qBound(minimum(), nvalue, maximum());
    }
    setValue(nvalue);
    selectAll();
}


/*!
 * \brief   Validates the spinbox \a input with cursor at position \a pos
 */
QValidator::State QtLongSpinBox::validate(QString & input, int & /*pos*/) const
{
    Q_D(const QtLongSpinBox);
    QString tinput = input;
    if (!prefix().isEmpty() && tinput.startsWith(prefix()))
        tinput.remove(0, prefix().size());
    if (!suffix().isEmpty() && tinput.endsWith(suffix()))
        tinput.chop(suffix().size());
    bool ok;
    qlonglong v = tinput.toLongLong(&ok, d->radix);
    if (!ok)
        return QValidator::Invalid;
    if (v<minimum() || v>maximum())
        return QValidator::Invalid;
    return QValidator::Acceptable;
}

/*!
 *  \brief  Attempts to correct the \a input.
 */
void QtLongSpinBox::fixup(QString & input) const {
    input.remove(locale().groupSeparator());
}


/*!
 * \brief Returns set of enabled steps.
 */
QAbstractSpinBox::StepEnabled QtLongSpinBox::stepEnabled() const
{
    if (wrapping())
        return StepUpEnabled|StepDownEnabled;
    StepEnabled se = 0;
    if (value()<maximum()) se |= StepUpEnabled;
    if (value()>minimum()) se |= StepDownEnabled;
    return se;
}

/*!
 * \internal
 */
void QtLongSpinBox::slotEditorTextChanged(const QString & t)
{
    QString tmp = t;
    int pos = lineEdit()->cursorPosition();
    QValidator::State state = validate(tmp, pos);
    if (state==QValidator::Acceptable) {
        setValue(valueFromText(tmp));
    }
}
