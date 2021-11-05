#include <QApplication>
#include <QMetaObject>
#include <QMetaProperty>
#include <QVBoxLayout>
#include <QScrollArea>

//###
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

#include <QPaintEvent>
#include <QPainter>
#include <QPixmap>

#include <QDebug>

#include "qtvariantproperty.h"
#include "qtpropertybrowser.h"
#include "qttreepropertybrowser.h"
#include "qtgroupboxpropertybrowser.h"
#include "qtbuttonpropertybrowser.h"

#include "qtpropertywidget.h"

#include "qtattributeresource.h"

#define Q_COUNTOF(x) (sizeof((x))/sizeof((x[0])))

static const char* colorMap[] = {
    "#92b2ee", // blue
    "#9bee92", // green
    "#eaee92", // yellow
    "#eed392", // orange
    "#f6a9ad", // red
    "#ea81df", // magenta
    "#c181ea", // purple
    "#81e4ea"  // cyan
};


class QtPropertyWidgetPrivate
{
    QtPropertyWidget *q_ptr;
    Q_DECLARE_PUBLIC(QtPropertyWidget)
    Q_DECLARE_TR_FUNCTIONS(QtPropertyWidgetPrivate)
public:

    QtPropertyWidgetPrivate(QtPropertyWidget* q);

    void initUi();

    int enumToInt(const QMetaEnum &metaEnum, int enumValue) const;
    int intToEnum(const QMetaEnum &metaEnum, int intValue) const;
    int flagToInt(const QMetaEnum &metaEnum, int flagValue) const;
    int intToFlag(const QMetaEnum &metaEnum, int intValue) const;
    bool isSubValue(int value, int subValue) const;
    bool isPowerOf2(int n) const;

    QString translateName(const QMetaObject* metaObject) const;
    QString translateName(const QMetaObject* metaObject, const char* propertyName) const;

    //void resolveAttributes(QtVariantProperty *property, const QMetaObject* metaObject) const;

    bool matchFilter(const QRegExp& regExp, const QString &key) const;
    void filterItems(const QList<QtBrowserItem*>& items, const QRegExp &regExp);
    void filterClasses(const QRegExp &rx);
    void filterProperties(const QRegExp &rx);

    bool writeProperty(const QMetaProperty& property, const QVariant& value);
    QVariant readProperty(const QMetaProperty& property) const;

    void clearInternals();
    void updateProperties(const QMetaObject *metaObject, bool recursive);
    void resolveClassProperties(const QMetaObject *metaClass);
    void resolveDynamicProperties(QObject* object);

    void blockRepaint();
    void unblockRepaint();

    QPixmap pixmapBuffer;
    QHash<const QMetaObject *, QMap<int, QtVariantProperty *> > classToIndexToProperty;
    QHash<const QMetaObject *, QtProperty *>  classToProperty;
    QHash<QtProperty *, const QMetaObject *>  propertyToClass;
    QHash<QtProperty *, int>                  propertyToIndex;
    QHash<QtProperty *, bool>                 propertyToExpanded;
    QList<QtProperty *>                       topLevelProperties;
    QSet<QtVariantProperty *>                 dynamicProperties;

    QString                      classFilter;
    QString                      propertyFilter;
    QVariantHash                 originalCache;

    const QMetaObject*           metaObject;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 0))
    union {
        QObject*  object;
        void*     gadget;
    };
#else
    QObject*                     object;
#endif
    QtAttributeResource*         resource;
    QScrollArea*                 scroll;
    QWidget*                     widget;
    QtAbstractPropertyBrowser*   browser;
    QtVariantPropertyManager*    manager;
    QtVariantPropertyManager*    readOnlyManager;
    QtVariantEditorFactory*      factory;
    QVBoxLayout*                 layout;

    QtPropertyWidget::ViewType     viewType;
    QtPropertyWidget::SubmitPolicy submitPolicy;

    bool isFinal : 1;
    bool isNotificable : 1;
    bool isGadget : 1;
};

QtPropertyWidgetPrivate::QtPropertyWidgetPrivate(QtPropertyWidget *q) :
    q_ptr(q),
    metaObject(0),
    object(0),
    resource(0),
    scroll(0),
    widget(0),
    browser(0),
    manager(new QtVariantPropertyManager(q)),
    readOnlyManager(new QtVariantPropertyManager(q)),
    factory(new QtVariantEditorFactory(q)),
    viewType(QtPropertyWidget::TreeView),
    submitPolicy(QtPropertyWidget::AutoSubmit),
    isFinal(false),
    isNotificable(false)
{
}

void QtPropertyWidgetPrivate::initUi()
{
    QObject::connect(manager, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
                     q_ptr, SLOT(slotValueChanged(QtProperty *, const QVariant &)));

    layout = new QVBoxLayout(q_ptr);
    layout->setMargin(0);
    q_ptr->setViewType(QtPropertyWidget::TreeView);
}

int QtPropertyWidgetPrivate::enumToInt(const QMetaEnum &metaEnum, int enumValue) const
{
    QMap<int, int> valueMap; // dont show multiple enum values which have the same values
    int pos = 0;
    for (int i = 0; i < metaEnum.keyCount(); i++) {
        int value = metaEnum.value(i);
        if (!valueMap.contains(value)) {
            if (value == enumValue)
                return pos;
            valueMap[value] = pos++;
        }
    }
    return -1;
}

int QtPropertyWidgetPrivate::intToEnum(const QMetaEnum &metaEnum, int intValue) const
{
    QSet<int> valueMap; // dont show multiple enum values which have the same values
    QVector<int> values;
    values.reserve(metaEnum.keyCount());
    for (int i = 0, n = metaEnum.keyCount(); i < n; i++) {
        const int value = metaEnum.value(i);
        if (!valueMap.contains(value)) {
            valueMap.insert(value);
            values.append(value);
        }
    }
    if (intValue >= values.count())
        return -1;
    return values.at(intValue);
}

bool QtPropertyWidgetPrivate::isSubValue(int value, int subValue) const
{
    if (value == subValue)
        return true;
    int i = 0;
    while (subValue) {
        if (!(value & (1 << i))) {
            if (subValue & 1)
                return false;
        }
        i++;
        subValue = subValue >> 1;
    }
    return true;
}

bool QtPropertyWidgetPrivate::isPowerOf2(int n) const
{
    return (n > 0 && ((n & (n-1)) == 0));
}

int QtPropertyWidgetPrivate::flagToInt(const QMetaEnum &metaEnum, int flagValue) const
{
    if (!flagValue)
        return 0;

    int intValue = 0;
    QMap<int, int> valueMap; // dont show multiple enum values which have the same values
    int pos = 0;
    for (int i = 0, n = metaEnum.keyCount(); i < n; i++) {
        const int value = metaEnum.value(i);
        if (!valueMap.contains(value) && isPowerOf2(value)) {
            if (isSubValue(flagValue, value))
                intValue |= (1 << pos);
            valueMap[value] = pos++;
        }
    }
    return intValue;
}

int QtPropertyWidgetPrivate::intToFlag(const QMetaEnum &metaEnum, int intValue) const
{
    QSet<int> valueMap; // dont show multiple enum values which have the same values
    QVector<int> values;
    valueMap.reserve(metaEnum.keyCount());
    for (int i = 0, n = metaEnum.keyCount(); i < n; i++) {
        int value = metaEnum.value(i);
        if (!valueMap.contains(value) && isPowerOf2(value)) {
            valueMap.insert(value);
            values.append(value);
        }
    }
    int flagValue = 0;
    int temp = intValue;
    int i = 0;
    while (temp) {
        if (i >= values.count())
            return -1;
        if (temp & 1)
            flagValue |= values.at(i);
        i++;
        temp = temp >> 1;
    }
    return flagValue;
}

QString QtPropertyWidgetPrivate::translateName(const QMetaObject *metaObject) const
{
    const char* className = metaObject->className();
    const int index = metaObject->indexOfClassInfo(className);
    if (index != -1)
        return tr( metaObject->classInfo(index).value() );
    return className;
}

QString QtPropertyWidgetPrivate::translateName(const QMetaObject *metaObject, const char *key) const
{
    const int index = metaObject->indexOfClassInfo(key);
    if (index != -1)
        return tr( metaObject->classInfo(index).value() );
    return key;
}


bool QtPropertyWidgetPrivate::matchFilter(const QRegExp &regExp, const QString& key) const
{
    return (regExp.isValid() && key.contains(regExp));
}

void QtPropertyWidgetPrivate::filterItems(const QList<QtBrowserItem*>& items, const QRegExp &regExp)
{
    for (auto it = items.cbegin(); it != items.cend(); ++it) {
        QtBrowserItem* item = *it;
        browser->setItemVisible(item, matchFilter(regExp, item->property()->propertyName()));
    }
}

void QtPropertyWidgetPrivate::filterClasses(const QRegExp &rx)
{
    if (!object)
        return;

    filterItems(browser->topLevelItems(), rx);
}

void QtPropertyWidgetPrivate::filterProperties(const QRegExp &rx)
{
    if (!object)
        return;

    const QList<QtBrowserItem*> classItems = browser->topLevelItems();
    for (auto it = classItems.cbegin(); it != classItems.cend(); ++it) {
        filterItems((*it)->children(), rx);
    }
}

bool QtPropertyWidgetPrivate::writeProperty(const QMetaProperty &property, const QVariant &value)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 0))
    if (isGadget)
        return property.writeOnGadget(gadget, value);
    else
        return property.write(object, value);
#else
    return property.write(object, value);
#endif
}

QVariant QtPropertyWidgetPrivate::readProperty(const QMetaProperty &property) const
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 0))
    if (isGadget)
        return property.readOnGadget(gadget);
    else
        return property.read(object);
#else
    return property.read(object);
#endif
}

void QtPropertyWidgetPrivate::clearInternals()
{
    classFilter.clear();
    propertyFilter.clear();

    browser->clear();
    manager->clear();
    readOnlyManager->clear();

    classToIndexToProperty.clear();
    classToProperty.clear();
    propertyToClass.clear();
    propertyToIndex.clear();
    propertyToExpanded.clear();

    // ###
    qDeleteAll(dynamicProperties);
    dynamicProperties.clear();
    topLevelProperties.clear();
}



void QtPropertyWidgetPrivate::updateProperties(const QMetaObject *metaObject, bool recursive)
{
    if (!metaObject)
        return;

    if (recursive)
        updateProperties(metaObject->superClass(), recursive);

    const QtProperty *classProperty = classToProperty.value(metaObject);
    if (!classProperty)
        return;

    for (int idx = metaObject->propertyOffset(), n = metaObject->propertyCount(); idx < n; idx++) {
        const QMetaProperty metaProperty = metaObject->property(idx);
        if (metaProperty.isReadable()) {
            if (classToIndexToProperty.contains(metaObject) && classToIndexToProperty[metaObject].contains(idx)) {
                QtVariantProperty *subProperty = classToIndexToProperty[metaObject][idx];
                if (metaProperty.isEnumType()) {
                    if (metaProperty.isFlagType())
                        subProperty->setValue(flagToInt(metaProperty.enumerator(), readProperty(metaProperty).toInt()));
                    else
                        subProperty->setValue(enumToInt(metaProperty.enumerator(), readProperty(metaProperty).toInt()));
                } else {
                    subProperty->setValue(readProperty(metaProperty));
                }
                subProperty->setKey(metaProperty.name());
                if (resource)
                    resource->setup(metaObject, subProperty);
            }
        }
    }
}

void QtPropertyWidgetPrivate::resolveClassProperties(const QMetaObject *metaClass)
{
    static int groupId = QtVariantPropertyManager::groupTypeId();
    static int flagId = QtVariantPropertyManager::flagTypeId();
    static int enumId = QtVariantPropertyManager::enumTypeId();

    if (!metaClass)
        return;

    if (!isFinal)
        resolveClassProperties(metaClass->superClass());

    QtVariantProperty *classProperty = static_cast<QtVariantProperty*>(classToProperty.value(metaClass));
    if (!classProperty)
    {
        classProperty = manager->addProperty(groupId, translateName(metaClass));
        classProperty->setKey(metaClass->className());
        if (resource)
            resource->setup(metaClass, classProperty);

        classToProperty[metaClass] = classProperty;
        propertyToClass[classProperty] = metaClass;

        QStringList flagNames, enumNames;
        flagNames.reserve(16);
        enumNames.reserve(16);

        QSet<int> valueSet;
        valueSet.reserve(16);

        QString propertyName;
        propertyName.reserve(64);

        QMetaProperty metaProperty;
        for (int idx = metaClass->propertyOffset(), n = metaClass->propertyCount(); idx < n; idx++)
        {
            metaProperty = metaClass->property(idx);

            int type = metaProperty.userType();
            QtVariantProperty *subProperty = 0;

            propertyName = translateName(metaClass, metaProperty.name());

            if (!metaProperty.isReadable()) {
                subProperty = readOnlyManager->addProperty(QVariant::String, propertyName);
                subProperty->setValue(QLatin1String("< Non Readable >"));
            }
            else if (metaProperty.isEnumType())
            {
                if (metaProperty.isFlagType()) {
                    subProperty = manager->addProperty(flagId, propertyName);
                    const QMetaEnum metaEnum = metaProperty.enumerator();
                    for (int i = 0, k = metaEnum.keyCount(); i < k; i++)
                    {
                        int value = metaEnum.value(i);
                        if (!valueSet.contains(value) && isPowerOf2(value)) { // dont show multiple enum values which have the same values
                            valueSet.insert(value);
                            flagNames.append(QLatin1String(metaEnum.key(i)));
                        }
                        subProperty->setAttribute(QLatin1String("flagNames"), flagNames);
                        subProperty->setValue(flagToInt(metaEnum, metaProperty.read(object).toInt()));
                    }
                    valueSet.clear();
                    flagNames.clear();
                } else {
                    subProperty = manager->addProperty(enumId, propertyName);
                    const QMetaEnum metaEnum = metaProperty.enumerator();
                    for (int i = 0, k = metaEnum.keyCount(); i < k; i++)
                    {
                        int value = metaEnum.value(i);
                        if (!valueSet.contains(value)) { // dont show multiple enum values which have the same values
                            valueSet.insert(value);
                            enumNames.append(QLatin1String(metaEnum.key(i)));
                        }
                    }
                    subProperty->setAttribute(QLatin1String("enumNames"), enumNames);
                    subProperty->setValue(enumToInt(metaEnum, metaProperty.read(object).toInt()));
                    valueSet.clear();
                    enumNames.clear();
                }
            }
            else if (manager->isPropertyTypeSupported(type))
            {
                if (!metaProperty.isWritable())
                    subProperty = readOnlyManager->addProperty(type, propertyName + QLatin1String(" (Non Writable)"));
                if (!metaProperty.isDesignable())
                    subProperty = readOnlyManager->addProperty(type, propertyName + QLatin1String(" (Non Designable)"));
                else
                    subProperty = manager->addProperty(type, propertyName);
                subProperty->setValue(readProperty(metaProperty));

            }
            else
            {
                subProperty = readOnlyManager->addProperty(QVariant::String, propertyName);
                subProperty->setValue(QLatin1String("< Unknown Type >"));
                subProperty->setEnabled(false);
            }
            subProperty->setKey(metaProperty.name());
            if (resource)
                resource->setup(metaClass, subProperty);

            classProperty->addSubProperty(subProperty);
            propertyToIndex[subProperty] = idx;
            classToIndexToProperty[metaClass][idx] = subProperty;
        }
    } else {
        updateProperties(metaClass, false);
    }

    topLevelProperties.append(classProperty);

    QtBrowserItem* browserItem = browser->addProperty(classProperty);
    QtTreePropertyBrowser* treeBrowser = qobject_cast<QtTreePropertyBrowser*>(browser);
    if (treeBrowser) {
        treeBrowser->setBackgroundColor(browserItem, colorMap[(classToProperty.size() - 1) % Q_COUNTOF(colorMap)]);
    }
}

void QtPropertyWidgetPrivate::resolveDynamicProperties(QObject *object)
{
    QtProperty *classProperty = classToProperty.value(metaObject);
    QtVariantProperty *subProperty = 0;

    const QList<QByteArray> propertyList = object->dynamicPropertyNames();
    int idx = metaObject->propertyCount();
    for (auto it = propertyList.cbegin(); it != propertyList.cend(); ++it, ++idx)
    {
        if (it->startsWith("_q_"))
            continue;

        QString propertyName = translateName(metaObject, *it);
        QVariant propertyValue = object->property(*it);
        subProperty = manager->addProperty(propertyValue.type(), propertyName);
        subProperty->setValue(propertyValue);
        if (resource)
            resource->setup(metaObject, subProperty);

        classProperty->addSubProperty(subProperty);
        propertyToIndex[subProperty] = idx;
        //classToIndexToProperty[metaObject][idx] = subProperty;
        dynamicProperties.insert(subProperty);
    }
    topLevelProperties.append(classProperty);
}

void QtPropertyWidgetPrivate::blockRepaint()
{
    if (!q_ptr->isVisible())
        return;

    pixmapBuffer = q_ptr->grab();
    browser->setUpdatesEnabled(false);
    if (scroll)
        scroll->setUpdatesEnabled(false);
}

void QtPropertyWidgetPrivate::unblockRepaint()
{
    browser->setUpdatesEnabled(true);
    if (scroll)
        scroll->setUpdatesEnabled(true);
    pixmapBuffer = QPixmap();
}


struct RepaintLocker
{
    QtPropertyWidgetPrivate* d;

    RepaintLocker(QtPropertyWidgetPrivate* p) : d(p)
    {
        d->blockRepaint();
    }

    void unlock() { d->unblockRepaint(); }

    ~RepaintLocker()
    {
        if (!d->pixmapBuffer.isNull())
            d->unblockRepaint();
    }
};



QtPropertyWidget::QtPropertyWidget(QWidget *parent)
    : QWidget(parent), d_ptr(new QtPropertyWidgetPrivate(this))
{
    Q_D(QtPropertyWidget);
    d->initUi();
}

QtPropertyWidget::~QtPropertyWidget()
{
}

void QtPropertyWidget::setResource(QtAttributeResource *resource)
{
    Q_D(QtPropertyWidget);
    d->resource = resource;
}

QtAttributeResource* QtPropertyWidget::resource() const
{
    Q_D(const QtPropertyWidget);
    return d->resource;
}

void QtPropertyWidget::setViewType(ViewType type)
{
    Q_D(QtPropertyWidget);

    if (d->viewType == type && d->browser != 0) {
        return;
    }

    if (d->object) {
        d->clearInternals();
    }

    d->viewType = type;
    d->layout->removeWidget(d->widget);
    if (d->browser) {
        delete d->browser;
        d->browser = 0;
    }

    if (d->scroll) {
        delete d->scroll;
        d->scroll = 0;
    }
    switch(d->viewType)
    {
        case TreeView:
        {
            d->browser = new QtTreePropertyBrowser(this);
            static_cast<QtTreePropertyBrowser*>(d->browser)->setAnimated(true);
            static_cast<QtTreePropertyBrowser*>(d->browser)->setRootIsDecorated(true);
            static_cast<QtTreePropertyBrowser*>(d->browser)->setAlternatingRowColors(true);
            d->layout->addWidget(d->browser);
            d->widget = d->browser;
            break;
        }
        case GroupView:
        {
            d->browser = new QtGroupBoxPropertyBrowser(this);
            d->scroll = new QScrollArea(this);
            d->scroll->setWidgetResizable(true);
            d->scroll->setWidget(d->browser);
            d->layout->addWidget(d->scroll);
            d->widget = d->scroll;
            break;
        }
        case ButtonView:
        {
            d->browser = new QtButtonPropertyBrowser(this);
            d->scroll = new QScrollArea(this);
            d->scroll->setWidgetResizable(true);
            d->scroll->setWidget(d->browser);
            d->layout->addWidget(d->scroll);
            d->widget = d->scroll;
            break;
        }
    }

    RepaintLocker locker(d);
    d->browser->setFactoryForManager(d->manager, d->factory);
    if (d->object) {
        d->resolveClassProperties(d->metaObject);
        if (!d->isGadget)
            d->resolveDynamicProperties(d->object);
    }
    locker.unlock();

    d->browser->update();
}

QtPropertyWidget::ViewType QtPropertyWidget::viewType() const
{
    Q_D(const QtPropertyWidget);
    return d->viewType;
}

void QtPropertyWidget::setSubmitPolicy(QtPropertyWidget::SubmitPolicy policy)
{
    Q_D(QtPropertyWidget);
    d->submitPolicy = policy;
}

QtPropertyWidget::SubmitPolicy QtPropertyWidget::submitPolicy() const
{
    Q_D(const QtPropertyWidget);
    return d->submitPolicy;
}

void QtPropertyWidget::setFinal(bool on)
{
    Q_D(QtPropertyWidget);

    RepaintLocker locker(d);
    d->isFinal = on;

    d->clearInternals();

    if (d->object) {
        d->resolveClassProperties(d->metaObject);
        if (!d->isGadget)
            d->resolveDynamicProperties(d->object);
    }
}

bool QtPropertyWidget::isFinal() const
{
    Q_D(const QtPropertyWidget);
    return d->isFinal;
}


#if QT_VERSION < QT_VERSION_CHECK(5, 2, 0)
void QtPropertyWidget::setObject(QObject *object)
{
    setupObject(object);
}
#endif

QObject *QtPropertyWidget::object() const
{
    Q_D(const QtPropertyWidget);
    return d->object;
}

#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
void *QtPropertyWidget::gadget() const
{
    Q_D(const QtPropertyWidget);
    return d->gadget;
}

bool QtPropertyWidget::isGadget() const
{
    Q_D(const QtPropertyWidget);
    return ((d->gadget != Q_NULLPTR) ? d->isGadget : false);
}

bool QtPropertyWidget::isObject() const
{
    Q_D(const QtPropertyWidget);
    return ((d->object != Q_NULLPTR) ? (!d->isGadget) : false);
}
#endif

void QtPropertyWidget::setPropertyFilter(const QString& pattern)
{
    Q_D(QtPropertyWidget);
    RepaintLocker locker(d);
    d->propertyFilter = pattern;
    Q_EMIT propertyFilterChanged(pattern);
    d->filterProperties(QRegExp(d->propertyFilter, Qt::CaseInsensitive, QRegExp::RegExp2));
}

QString QtPropertyWidget::propertyFilter() const {
    Q_D(const QtPropertyWidget);
    return d->propertyFilter;
}

void QtPropertyWidget::setClassFilter(const QString &pattern)
{
    Q_D(QtPropertyWidget);
    RepaintLocker locker(d);
    d->classFilter = pattern;
    Q_EMIT propertyFilterChanged(pattern);
    d->filterClasses(QRegExp(d->classFilter, Qt::CaseSensitive, QRegExp::RegExp2));
}

QString QtPropertyWidget::classFilter() const
{
    Q_D(const QtPropertyWidget);
    return d->classFilter;
}

void QtPropertyWidget::enableNotifications(bool on)
{
    Q_D(QtPropertyWidget);
    d->isNotificable = on;
}

bool QtPropertyWidget::notificationsEnabled() const
{
    Q_D(const QtPropertyWidget);
    return d->isNotificable;
}


void QtPropertyWidget::slotValueChanged(QtProperty *property, const QVariant &value)
{
    Q_D(QtPropertyWidget);
    if (!d->object)
        return;

    QtVariantProperty* variantProperty = static_cast<QtVariantProperty*>(property);
    if (d->dynamicProperties.contains(variantProperty))
    {
        d->object->setProperty(property->propertyName().toLatin1(), variantProperty->value());
        if (d->isNotificable)
            Q_EMIT propertyChanged(property->propertyName(), value);

        return;
    }

    if (!d->propertyToIndex.contains(property))
        return;

    QMetaProperty metaProperty = d->metaObject->property(d->propertyToIndex.value(property));
    if (d->submitPolicy == ManualSubmit) // if manual submit
    {
        // store original value if it isn't already stored
        auto it = d->originalCache.constFind(metaProperty.name());
        if (it == d->originalCache.constEnd())
            d->originalCache[metaProperty.name()] = metaProperty.read(d->object);
    }

    if (metaProperty.isEnumType()) {
        if (metaProperty.isFlagType())
            metaProperty.write(d->object, d->intToFlag(metaProperty.enumerator(), value.toInt()));
        else
            metaProperty.write(d->object, d->intToEnum(metaProperty.enumerator(), value.toInt()));
    } else {
        metaProperty.write(d->object, value);
    }

    if (d->isNotificable)
        Q_EMIT propertyChanged(property->propertyName(), value);
}

void QtPropertyWidget::submit()
{
    Q_D(QtPropertyWidget);
    // does nothing because changes is already applied to object
    // simply clear cached values
    d->originalCache.clear();
}

void QtPropertyWidget::revert()
{
    Q_D(QtPropertyWidget);

    if (!d->object)
        return;

    if (d->submitPolicy != ManualSubmit)
        return;

    RepaintLocker locker(d);
    // revert the changes to original values
    auto it = d->originalCache.cbegin();
    for (; it != d->originalCache.cend(); ++it)
    {
        const QString name = it.key();
        const QVariant value = it.value();

        const QMetaProperty metaProperty = d->metaObject->property(d->metaObject->indexOfProperty(name.toLatin1()));
        if (metaProperty.isEnumType()) {
            if (metaProperty.isFlagType())
                d->writeProperty(metaProperty, d->intToFlag(metaProperty.enumerator(), value.toInt()));
            else
                d->writeProperty(metaProperty, d->intToEnum(metaProperty.enumerator(), value.toInt()));
        } else {
            d->writeProperty(metaProperty, value);
        }
    }
    d->updateProperties(d->metaObject, true);
}

void QtPropertyWidget::refresh()
{
    Q_D(QtPropertyWidget);

    if (d->object)
    {
        RepaintLocker locker(d);
        d->updateProperties(d->metaObject, true);
    }
}

void QtPropertyWidget::objectDestroyed(QObject *object)
{
    Q_D(QtPropertyWidget);
    if (object == d->object)
    {
        RepaintLocker locker(d);
        setupObject(Q_NULLPTR);
    }
}

void QtPropertyWidget::paintEvent(QPaintEvent *event)
{
    Q_D(QtPropertyWidget);
    if (d->pixmapBuffer.isNull())
        return QWidget::paintEvent(event);

    QPainter painter(this);
    painter.drawPixmap(event->rect(), d->pixmapBuffer, event->rect());
}

bool QtPropertyWidget::isQObject(const QMetaObject *metaObject)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
    return metaObject->inherits(&QObject::staticMetaObject);
#else
    const QMetaObject* super = metaObject->superClass();
    while(super != Q_NULLPTR) {
        if (!qstrcmp(super->className(), QObject::staticMetaObject.className()))
            return true;
        super = super->superClass();
    }
    return false;
#endif
}

void QtPropertyWidget::setupObject(QObject *object)
{
    Q_D(QtPropertyWidget);

    if (d->object == object)
        return;

    RepaintLocker locker(d);
    if (d->object != Q_NULLPTR && !d->isGadget) {
        disconnect(d->object, SIGNAL(destroyed(QObject*)),
                   this, SLOT(objectDestroyed(QObject*)));
    }

    d->originalCache.clear();
    d->clearInternals();

    d->object = object;
    if (d->object == Q_NULLPTR) {
        return;
    }

    d->metaObject = object->metaObject();
    d->isGadget = false;

    connect(object, SIGNAL(destroyed(QObject*)), SLOT(objectDestroyed(QObject*)));

    d->resolveClassProperties(d->metaObject);
    d->resolveDynamicProperties(d->object);

    d->filterClasses(QRegExp(d->classFilter, Qt::CaseSensitive, QRegExp::FixedString));
    d->filterProperties(QRegExp(d->propertyFilter, Qt::CaseInsensitive, QRegExp::FixedString));

}

void QtPropertyWidget::setupGadget(void *gadget, const QMetaObject *metaObject)
{
    Q_D(QtPropertyWidget);
    if (d->gadget == gadget)
        return;

    RepaintLocker locker(d);
    if (d->object != Q_NULLPTR && !d->isGadget) {
        disconnect(d->object, SIGNAL(destroyed(QObject*)),
                   this, SLOT(objectDestroyed(QObject*)));
    }

    d->originalCache.clear();
    d->clearInternals();

    d->gadget = gadget;
    if (d->gadget == Q_NULLPTR) {
        return;
    }

    d->metaObject = metaObject;
    d->isGadget = true;

    d->resolveClassProperties(d->metaObject);
    d->filterClasses(QRegExp(d->classFilter, Qt::CaseSensitive, QRegExp::FixedString));
    d->filterProperties(QRegExp(d->propertyFilter, Qt::CaseInsensitive, QRegExp::FixedString));
}
