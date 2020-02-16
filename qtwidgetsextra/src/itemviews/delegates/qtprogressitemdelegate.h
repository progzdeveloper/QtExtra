#ifndef QTPROGRESSITEMDELEGATE_H
#define QTPROGRESSITEMDELEGATE_H
#include <QtWidgetsExtra>
#include <QItemDelegate>

class QRect;

class QTWIDGETSEXTRA_EXPORT QtProgressItemDelegate :
        public QItemDelegate
{
    Q_OBJECT
public:
    explicit QtProgressItemDelegate(QObject * parent = 0);
    ~QtProgressItemDelegate();

    void setPrecision(int prec = 0);
    int precision() const;

    // QAbstractItemDelegate interface
public:
    virtual void paint(QPainter * painter,
               const QStyleOptionViewItem & option,
               const QModelIndex & index) const Q_DECL_OVERRIDE;

    virtual QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const Q_DECL_OVERRIDE;

private:
    QRect rect(const QStyleOptionViewItem &option,
               const QModelIndex &index, int role) const;

private:
    int mPrec;
};

#endif
