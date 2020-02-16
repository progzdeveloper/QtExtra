#ifndef QTSHEETDELEGATE_H
#define QTSHEETDELEGATE_H

#include <QtWidgetsExtra>
#include <QStyledItemDelegate>

class QTreeView;

class QTWIDGETSEXTRA_EXPORT QtSheetItemDelegate :
        public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit QtSheetItemDelegate(QTreeView *view, QObject *parent);
    virtual ~QtSheetItemDelegate();

    // QAbstractItemDelegate interface
public:
    virtual void paint(QPainter *painter,
                       const QStyleOptionViewItem &opt,
                       const QModelIndex &index) const Q_DECL_OVERRIDE;

    virtual QSize sizeHint(const QStyleOptionViewItem &opt,
                           const QModelIndex &index) const Q_DECL_OVERRIDE;


private:
    QTreeView* mView;
};

#endif // QTSHEETDELEGATE_H
