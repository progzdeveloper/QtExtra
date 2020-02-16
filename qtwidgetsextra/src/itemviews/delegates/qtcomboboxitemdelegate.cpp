#include "qtcomboboxitemdelegate.h"
#include <QComboBox>
#include <QCompleter>
#include <QValidator>
#include <QModelIndex>
#include <QStringList>

class QtComboBoxItemDelegatePrivate
{
public:
    QtComboBoxItemDelegate *q_ptr;
    QAbstractItemModel *model;
    QCompleter *completer;
    const QValidator *validator;
    int modelColumn;
    QModelIndex rootIndex;
    QStringList items;
};



QtComboBoxItemDelegate::QtComboBoxItemDelegate(QObject *parent) 
    : QItemDelegate(parent), d_ptr(new QtComboBoxItemDelegatePrivate)
{
    d_ptr->q_ptr = this;
    d_ptr->modelColumn = 0;
    d_ptr->model = 0;
    d_ptr->completer = 0;
    d_ptr->validator = 0;
}

QtComboBoxItemDelegate::QtComboBoxItemDelegate(const QStringList &items, QObject *parent)
    : QItemDelegate(parent), d_ptr(new QtComboBoxItemDelegatePrivate)
{
    d_ptr->q_ptr = this;
    d_ptr->modelColumn = 0;
    d_ptr->model = 0;
    d_ptr->completer = 0;
    d_ptr->validator = 0;
    addItems(items);
}

QtComboBoxItemDelegate::~QtComboBoxItemDelegate()
{
    delete d_ptr;
}

void QtComboBoxItemDelegate::addItems(const QStringList& items)
{
    Q_D(QtComboBoxItemDelegate);
    d->items << items;
}

void QtComboBoxItemDelegate::setItems(const QStringList& items)
{
    clear();
    addItems(items);
}

void QtComboBoxItemDelegate::clear()
{
    Q_D(QtComboBoxItemDelegate);
    d->items.clear();
}


void QtComboBoxItemDelegate::setModel(QAbstractItemModel *model)
{
    Q_D(QtComboBoxItemDelegate);
    d->model = model;
}

QAbstractItemModel * QtComboBoxItemDelegate::model() const
{
    Q_D(const QtComboBoxItemDelegate);
    return d->model;
}

void QtComboBoxItemDelegate::setModelColumn(int visibleColumn)
{
    Q_D(QtComboBoxItemDelegate);
    d->modelColumn = visibleColumn;
}

int QtComboBoxItemDelegate::modelColumn() const
{
    Q_D(const QtComboBoxItemDelegate);
    return d->modelColumn;
}

void QtComboBoxItemDelegate::setRootModelIndex(const QModelIndex& index)
{
    Q_D(QtComboBoxItemDelegate);
    d->rootIndex = index;
}

QModelIndex QtComboBoxItemDelegate::rootModelIndex () const
{
    Q_D(const QtComboBoxItemDelegate);
    return d->rootIndex;
}

void QtComboBoxItemDelegate::setValidator(const QValidator* validator)
{
    Q_D(QtComboBoxItemDelegate);
    d->validator = validator;
}

const QValidator *QtComboBoxItemDelegate::validator () const
{
    Q_D(const QtComboBoxItemDelegate);
    return d->validator;
}

void QtComboBoxItemDelegate::setCompleter(QCompleter *completer)
{
    Q_D(QtComboBoxItemDelegate);
    d->completer = completer;
}


QCompleter *QtComboBoxItemDelegate::completer() const
{
    Q_D(const QtComboBoxItemDelegate);
    return d->completer;
}

QWidget *QtComboBoxItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/*option*/,
                                              const QModelIndex &/*index*/) const
{
    Q_D(const QtComboBoxItemDelegate);
    QComboBox *editor = new QComboBox(parent);
    if (d->model) {
        editor->setModel(d->model);
        editor->setModelColumn(d->modelColumn);
    }
    else {
        editor->addItems(d->items);
    }
    editor->setCompleter(d->completer);
    editor->setValidator(d->validator);
    return editor;
}

void QtComboBoxItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString value = index.data(Qt::EditRole).toString();

    QComboBox *comboBox = qobject_cast<QComboBox*>(editor);
    if (comboBox)
        comboBox->setCurrentIndex(comboBox->findText(value));
}

void QtComboBoxItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                          const QModelIndex &index) const
{
    QComboBox *comboBox = qobject_cast<QComboBox*>(editor);
    if (comboBox) {
        QString value = comboBox->currentText();
        model->setData(index, value, Qt::EditRole);
    }
}

void QtComboBoxItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, 
                                                  const QModelIndex &/*index*/) const
{
    editor->setGeometry(option.rect);
}



