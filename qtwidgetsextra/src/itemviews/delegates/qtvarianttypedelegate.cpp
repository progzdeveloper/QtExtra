#include "qtvarianttypedelegate.h"
#include <QComboBox>

template<int _TypeId>
inline void registerType(QComboBox *editor) {
    editor->addItem(QVariant::typeToName(_TypeId), _TypeId);
}



QtVariantTypeDelegate::QtVariantTypeDelegate(QObject *parent /*= 0*/) :
    QStyledItemDelegate(parent),
    mRole(Qt::EditRole)
{
}

QtVariantTypeDelegate::~QtVariantTypeDelegate()
{
}

QWidget *QtVariantTypeDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                             const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)
    QComboBox *editor = new QComboBox(parent);
    registerType<QVariant::LongLong>(editor);
    registerType<QVariant::ULongLong>(editor);
    registerType<QVariant::Int>(editor);
    registerType<QVariant::UInt>(editor);
    registerType<QVariant::Double>(editor);
    registerType<QVariant::Bool>(editor);
    registerType<QVariant::Uuid>(editor);
    registerType<QVariant::Url>(editor);
    registerType<QVariant::String>(editor);
    registerType<QVariant::ByteArray>(editor);
    registerType<QVariant::Date>(editor);
    registerType<QVariant::Time>(editor);
    registerType<QVariant::DateTime>(editor);
    registerType<QVariant::Font>(editor);
    registerType<QVariant::KeySequence>(editor);
    registerType<QVariant::Color>(editor);
    return editor;
}

void QtVariantTypeDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    int value = index.data(mRole).toInt();

    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    if (comboBox)
        comboBox->setCurrentIndex(comboBox->findData(value));
}

void QtVariantTypeDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                         const QModelIndex &index) const
{
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    if (comboBox) {
        model->setData(index, comboBox->itemData(comboBox->currentIndex()), mRole);
    }
}

void QtVariantTypeDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, 
                                                 const QModelIndex &index) const
{
    Q_UNUSED(index)
    editor->setGeometry(option.rect);
}


