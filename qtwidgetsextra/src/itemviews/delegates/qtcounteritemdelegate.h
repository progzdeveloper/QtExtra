#ifndef QTCOUNTERITEMDELEGATE_H
#define QTCOUNTERITEMDELEGATE_H

#include <QtWidgetsExtra>
#include <QStyledItemDelegate>

class QTWIDGETSEXTRA_EXPORT QtCounterItemDelegate :
        public QStyledItemDelegate
{
    Q_OBJECT
public:

    enum CounterMode
    {
        None   = 0,
        Plus   = 1,
        Suffix = 2,
        User = Suffix + 1
    };

    explicit QtCounterItemDelegate(QObject *parent = Q_NULLPTR);
    explicit QtCounterItemDelegate(CounterMode mode, QObject* parent = Q_NULLPTR);
    virtual ~QtCounterItemDelegate();

    void setCounterMode(CounterMode mode);
    CounterMode counterMode() const;

    void setIconSize(const QSize& size);
    QSize iconSize() const;

    // QAbstractItemDelegate interface
public:
    virtual void paint(QPainter *painter,
                       const QStyleOptionViewItem &option,
                       const QModelIndex &index) const Q_DECL_OVERRIDE;

    virtual QSize sizeHint(const QStyleOptionViewItem &option,
                           const QModelIndex &index) const Q_DECL_OVERRIDE;
protected:
    virtual QString formatCounter(qint64 count) const;

private:
    CounterMode mMode;
    QSize mIconSize;
};


#endif // QTCOUNTERITEMDELEGATE_H
