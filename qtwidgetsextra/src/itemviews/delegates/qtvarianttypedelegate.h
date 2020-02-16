#ifndef QTVARIANTTYPEDELEGATE_H
#define QTVARIANTTYPEDELEGATE_H

#include <QStyledItemDelegate>
#include <QVariant>

#include <QtWidgetsExtra>

class QWidget;
class QModelIndex;
class QStyleOptionViewItem;
class QAbstractItemModel;


class QTWIDGETSEXTRA_EXPORT QtVariantTypeDelegate :
        public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit QtVariantTypeDelegate(QObject *parent = 0);
    ~QtVariantTypeDelegate();

    inline void setItemRole(int role) { mRole = role; }
    inline int itemRole() const { return mRole; }

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const;

private:
    Q_DISABLE_COPY(QtVariantTypeDelegate)
    int mRole;
};

#endif
