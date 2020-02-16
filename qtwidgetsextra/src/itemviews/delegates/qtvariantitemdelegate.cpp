#include <climits>
#include <float.h>

#ifndef QTWIDGETSEXTRA_NO_CUSTOMEDITORS
#include "../../widgets/qtcolorbutton.h"
#include "../../widgets/qtshortcutedit.h"
#include "../../widgets/qtlongspinbox.h"
#endif

#include "qtvariantitemdelegate.h"
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif

struct Attribute
{
    QByteArray key;
    QVariant value;

    Attribute(const char* akey, const QVariant& aval) :
        key(akey), value(aval) {}
};

typedef QHash<QVariant::Type, QByteArray> PropertyMap;
typedef QMultiHash<QVariant::Type, Attribute> AttributeMap;


class QtVariantItemEditorFactoryPrivate
{
public:
    PropertyMap propertyMap;
    AttributeMap attributeMap;
    QtVariantItemEditorFactoryPrivate()
    {
        // initialize property mapping
        propertyMap[QVariant::Bool]      = "checked";
        propertyMap[QVariant::UInt]      = "value";
        propertyMap[QVariant::ULongLong] = "value";
        propertyMap[QVariant::Int]       = "value";
        propertyMap[QVariant::LongLong]  = "value";
        propertyMap[QVariant::Double]    = "value";
        propertyMap[QVariant::Char]      = "value";
        propertyMap[QVariant::Uuid]      = "text";
        propertyMap[QVariant::Url]       = "text";
        propertyMap[QVariant::String]    = "text";
        propertyMap[QVariant::ByteArray] = "text";
        propertyMap[QVariant::Date]      = "date";
        propertyMap[QVariant::Time]      = "time";
        propertyMap[QVariant::DateTime]  = "dateTime";
        propertyMap[QVariant::Font]      = "currentFont";
    #ifndef QTWIDGETSEXTRA_NO_CUSTOMEDITORS
        propertyMap[QVariant::Color]       = "color";
        propertyMap[QVariant::KeySequence] = "shortcut";
    #endif

        // initialize attribute mapping
        attributeMap.insert(QVariant::UInt, Attribute("minimum", 0));
        attributeMap.insert(QVariant::UInt, Attribute("maximum", UINT_MAX));

        attributeMap.insert(QVariant::ULongLong, Attribute("minimum", 0));
        attributeMap.insert(QVariant::ULongLong, Attribute("maximum", ULONG_LONG_MAX));

        attributeMap.insert(QVariant::Int, Attribute("minimum", INT_MIN));
        attributeMap.insert(QVariant::Int, Attribute("maximum", INT_MAX));

        attributeMap.insert(QVariant::LongLong, Attribute("minimum", LONG_LONG_MIN));
        attributeMap.insert(QVariant::LongLong, Attribute("maximum", LONG_LONG_MAX));

        attributeMap.insert(QVariant::Double, Attribute("minimum", DBL_MIN));
        attributeMap.insert(QVariant::Double, Attribute("maximum", DBL_MAX));
        attributeMap.insert(QVariant::Double, Attribute("singleStep", 0.5));

        attributeMap.insert(QVariant::Uuid, Attribute("inputMask", "{HHHHHHHH-HHHH-HHHH-HHHH-HHHHHHHHHHHH}"));

        attributeMap.insert(QVariant::Date, Attribute("calendarPopup", true));
        attributeMap.insert(QVariant::DateTime, Attribute("calendarPopup", true));
    }

    void setupEditor(int type, QWidget* editor) const
    {
        QVariant::Type t = static_cast<QVariant::Type>(type);
        AttributeMap::const_iterator it = attributeMap.constFind(t);
        for (; it != attributeMap.end() && it.key() == t; ++it) {
            editor->setProperty(it->key, it->value);
        }
    }
};


QtVariantItemEditorFactory::QtVariantItemEditorFactory() :
    d_ptr(new QtVariantItemEditorFactoryPrivate)
{
}

QtVariantItemEditorFactory::~QtVariantItemEditorFactory()
{
}

QWidget *QtVariantItemEditorFactory::createEditor(int userType, QWidget * parent) const
{
    //Q_D(const QtVariantItemEditorFactory);
    QWidget *w = 0;
    switch(userType)
    {
    case QVariant::Bool:
        w = new QCheckBox(parent);
        break;

    case QVariant::UInt:
        w = new QSpinBox(parent);
        w->setProperty("minimum", 0);
        w->setProperty("maximum", INT_MAX);
        break;

#ifndef QTWIDGETSEXTRA_NO_CUSTOMEDITORS
    case QVariant::ULongLong:
        w = new QtLongSpinBox(parent);
        w->setProperty("minimum", 0);
        w->setProperty("maximum", LONG_LONG_MAX);
        break;
#else
    case QVariant::ULongLong:
        w = new QSpinBox(parent);
        w->setProperty("minimum", 0);
        w->setProperty("maximum", UINT_MAX);
        break;
#endif

    case QVariant::Int:
        w = new QSpinBox(parent);
        w->setProperty("minimum", INT_MIN);
        w->setProperty("maximum", INT_MAX);
        break;

#ifndef QTWIDGETSEXTRA_NO_CUSTOMEDITORS
    case QVariant::LongLong:
        w = new QtLongSpinBox(parent);
        w->setProperty("minimum", LONG_LONG_MIN);
        w->setProperty("maximum", LONG_LONG_MAX);
        break;
#else
    case QVariant::LongLong:
        w = new QSpinBox(parent);
        w->setProperty("minimum", INT_MIN);
        w->setProperty("maximum", INT_MAX);
        break;
#endif

    case QVariant::Double:
        w = new QDoubleSpinBox(parent);
        w->setProperty("minimum", DBL_MIN);
        w->setProperty("maximum", DBL_MAX);
        w->setProperty("singleStep", 0.5);
        break;

    case QVariant::Uuid:
        w = new QLineEdit(parent);
        w->setProperty("inputMask", "{HHHHHHHH-HHHH-HHHH-HHHH-HHHHHHHHHHHH}");
        break;

    case QVariant::Url:
    case QVariant::String:
    case QVariant::ByteArray:
        w = new QLineEdit(parent);
        break;

    case QVariant::Date:
        w = new QDateEdit(parent);
        w->setProperty("calendarPopup", true);
        w->setProperty("date", QDate::currentDate());
        break;

    case QVariant::Time:
        w = new QTimeEdit(parent);
        w->setProperty("time", QTime::currentTime());
        break;

    case QVariant::DateTime:
        w = new QDateTimeEdit(parent);
        w->setProperty("calendarPopup", true);
        w->setProperty("dateTime", QDateTime::currentDateTime());
        break;

    case QVariant::Font:
        w = new QFontComboBox(parent);
        break;

#ifndef QTWIDGETSEXTRA_NO_CUSTOMEDITORS
    case QVariant::Color:
        w = new QtColorButton(parent);
        break;

    case QVariant::KeySequence:
        w = new QtShortcutEdit(parent);
        break;
#endif
    }

    if (w == Q_NULLPTR)
        w = QItemEditorFactory::createEditor(userType, parent);

    //d->setupEditor(userType, w);

    return w;
}

QByteArray QtVariantItemEditorFactory::valuePropertyName(int userType) const
{
    Q_D(const QtVariantItemEditorFactory);
    PropertyMap::const_iterator it = d->propertyMap.constFind(static_cast<QVariant::Type>(userType));
    return (it == d->propertyMap.end() ? QItemEditorFactory::valuePropertyName(userType) : *it);
}

QVariant QtVariantItemEditorFactory::attribute(int userType, const char *key) const
{
    Q_D(const QtVariantItemEditorFactory);
    QVariant::Type type = static_cast<QVariant::Type>(userType);
    AttributeMap::const_iterator it = d->attributeMap.constFind(type);
    for (; it != d->attributeMap.end() && it.key() == type; ++it) {
        if (it->key == key)
            return it->value;
    }
    return QVariant();
}

void QtVariantItemEditorFactory::setAttribute(int userType, const char *key, const QVariant &value)
{
    Q_D(QtVariantItemEditorFactory);
    QVariant::Type type = static_cast<QVariant::Type>(userType);
    AttributeMap::iterator it = d->attributeMap.find(type);
    for (; it != d->attributeMap.end() && it.key() == type; ++it) {
        if (it->key == key) {
            it->value = value; // overwrite
            return;
        }
    }
    // insert
    d->attributeMap.insert(type, Attribute(key, value));
}





class QtVariantItemDelegatePrivate
{
public:
    int itemRole;
};


QtVariantItemDelegate::QtVariantItemDelegate(QObject *parent /*= 0*/) :
    QStyledItemDelegate(parent),
    d_ptr(new QtVariantItemDelegatePrivate)
{
    d_ptr->itemRole = Qt::EditRole;
    setItemEditorFactory(new QtVariantItemEditorFactory);
}

QtVariantItemDelegate::~QtVariantItemDelegate()
{
}

void QtVariantItemDelegate::setItemRole(int role)
{
    Q_D(QtVariantItemDelegate);
    d->itemRole = role;
}

int QtVariantItemDelegate::itemRole() const
{
    Q_D(const QtVariantItemDelegate);
    return d->itemRole;
}

QWidget *QtVariantItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem&,
                                             const QModelIndex &index) const
{
    Q_D(const QtVariantItemDelegate);
    Q_ASSERT(itemEditorFactory() != 0);
    return itemEditorFactory()->createEditor(index.data(d->itemRole).type(), parent);
}

void QtVariantItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    Q_D(const QtVariantItemDelegate);
    Q_ASSERT(itemEditorFactory() != 0);
    QVariant value = index.data(d->itemRole);
    QByteArray propertyName = itemEditorFactory()->valuePropertyName(value.type());
    editor->setProperty(propertyName.constData(), value);
}

void QtVariantItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                         const QModelIndex &index) const
{
    Q_D(const QtVariantItemDelegate);
    Q_ASSERT(itemEditorFactory() != 0);
    int typeId = index.data(d->itemRole).type();
    QByteArray propertyName = itemEditorFactory()->valuePropertyName(typeId);
    QVariant value = editor->property(propertyName.constData());
    model->setData(index, value, d->itemRole);
}

void QtVariantItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, 
                                                 const QModelIndex &index) const
{
    Q_UNUSED(index)
    editor->setGeometry(option.rect);
}

QSize QtVariantItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight(size.height() + 4);
    return size;
}
