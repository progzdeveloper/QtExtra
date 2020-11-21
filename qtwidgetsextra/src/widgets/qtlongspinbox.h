#ifndef QTLONGSPINBOX_H
#define QTLONGSPINBOX_H

#include <QAbstractSpinBox>
#include <QtWidgetsExtra>

class QTWIDGETSEXTRA_EXPORT QtLongSpinBox :
        public QAbstractSpinBox
{
    Q_OBJECT
    Q_PROPERTY(qlonglong minimum READ minimum WRITE setMinimum)
    Q_PROPERTY(qlonglong maximum READ maximum WRITE setMaximum)
    Q_PROPERTY(qlonglong value READ value WRITE setValue NOTIFY valueChanged USER true)
    Q_PROPERTY(QString prefix READ prefix WRITE setPrefix)
    Q_PROPERTY(QString suffix READ suffix WRITE setSuffix)
    Q_PROPERTY(qlonglong singleStep READ singleStep WRITE setSingleStep)
    Q_PROPERTY(int displayIntegerBase READ displayIntegerBase WRITE setDisplayIntegerBase)
    Q_DISABLE_COPY(QtLongSpinBox)

public:
    explicit QtLongSpinBox(QWidget* parent = Q_NULLPTR);
    QtLongSpinBox(qlonglong minimum, qlonglong maximum, QWidget* parent = Q_NULLPTR);
    ~QtLongSpinBox();

    void stepBy ( int steps ) Q_DECL_OVERRIDE;

    qlonglong value() const;

    qlonglong minimum() const;
    void setMinimum(qlonglong v);

    qlonglong maximum() const;
    void setMaximum(qlonglong v);

    const QString &prefix() const;
    void setPrefix(const QString &);

    const QString &suffix() const;
    void setSuffix(const QString &);

    qlonglong singleStep() const;
    void setSingleStep(qlonglong s);

    void setRange(qlonglong min, qlonglong max);

    void setDisplayIntegerBase(int base);
    int displayIntegerBase() const;

    QValidator::State validate ( QString & input, int & pos ) const Q_DECL_OVERRIDE;
    void fixup ( QString & input ) const Q_DECL_OVERRIDE;

public Q_SLOTS:
    void setValue(qlonglong v);

Q_SIGNALS:
    void valueChanged ( qlonglong i );
    void valueChanged ( const QString & text );

protected:
    virtual QString textFromValue(qlonglong value) const;
    virtual qlonglong valueFromText(const QString &text) const;
    StepEnabled stepEnabled() const Q_DECL_OVERRIDE;

private Q_SLOTS:
    void slotEditorTextChanged(const QString &t);

private:
    QT_PIMPL(QtLongSpinBox)
};

#endif // QTLONGSPINBOX_H
