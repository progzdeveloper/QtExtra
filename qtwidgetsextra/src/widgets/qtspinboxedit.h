#ifndef QTSPINBOXEDIT_H
#define QTSPINBOXEDIT_H

#include <QSpinBox>
#include <QtWidgetsExtra>

class QTWIDGETSEXTRA_EXPORT QtSpinBoxEdit : public QSpinBox
{
    Q_OBJECT
public:
    explicit QtSpinBoxEdit(QWidget* parent = Q_NULLPTR);
    ~QtSpinBoxEdit();

public Q_SLOTS:
    virtual void showEditor();

private:
    QT_PIMPL(QtSpinBoxEdit)
};

#endif // QTSPINBOXEDIT_H
