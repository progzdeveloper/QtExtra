#ifndef QTTEXTSPINBOX_H
#define QTTEXTSPINBOX_H

#include <QSpinBox>
#include <QtWidgetsExtra>

class QTWIDGETSEXTRA_EXPORT QtTextSpinBox :
        public QSpinBox
{
    Q_OBJECT
    Q_PROPERTY(QStringList list READ list WRITE setList)

public:
    explicit QtTextSpinBox(QWidget* parent = Q_NULLPTR);
    QtTextSpinBox(const QStringList &list, QWidget *parent = Q_NULLPTR);
    ~QtTextSpinBox();

    QValidator::State validate(QString& input, int& pos ) const Q_DECL_OVERRIDE;

public Q_SLOTS:
    virtual void setList(const QStringList &s);
    const QStringList &list() const;

Q_SIGNALS:
    void listChanged(const QStringList &);

protected:
    virtual QString textFromValue(int value) const;
    virtual int valueFromText(const QString & text) const;

private:
    QT_PIMPL(QtTextSpinBox)
};

#endif // QTTEXTSPINBOX_H
