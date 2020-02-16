#ifndef QTCOMBOBOXITEMDELEGATE_H
#define QTCOMBOBOXITEMDELEGATE_H

#include <QtWidgetsExtra>
#include <QItemDelegate>

class QWidget;
class QCompleter;
class QValidator;
class QModelIndex;
class QStyleOptionViewItem;
class QAbstractItemModel;
class QStringList;

class QtComboBoxItemDelegatePrivate;
class QTWIDGETSEXTRA_EXPORT QtComboBoxItemDelegate :
        public QItemDelegate
{
    Q_OBJECT
public:
    explicit QtComboBoxItemDelegate(QObject *parent = 0);
    explicit QtComboBoxItemDelegate(const QStringList& items, QObject *parent = 0);
    ~QtComboBoxItemDelegate();

    void addItems(const QStringList& items);
    void setItems(const QStringList& items);
    void clear();

    void setModel(QAbstractItemModel *model);
    QAbstractItemModel *model() const;

    void setModelColumn(int visibleColumn);
    int modelColumn() const;

    void setRootModelIndex(const QModelIndex& index);
    QModelIndex rootModelIndex () const;

    void setValidator(const QValidator *validator);
    const QValidator *validator () const;

    QCompleter *completer() const;
    void setCompleter(QCompleter *completer);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const;

private:
    QtComboBoxItemDelegatePrivate *d_ptr;
    Q_DECLARE_PRIVATE(QtComboBoxItemDelegate)
};

#endif

