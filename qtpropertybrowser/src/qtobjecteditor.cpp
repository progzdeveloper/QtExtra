#include <QApplication>
#include <QMetaObject>
#include <QMetaProperty>
#include <QVBoxLayout>
#include <QScrollArea>

#include <chrono>
#include <QDebug>

#include "qtvariantproperty.h"
#include "qtpropertybrowser.h"
#include "qttreepropertybrowser.h"
#include "qtgroupboxpropertybrowser.h"
#include "qtbuttonpropertybrowser.h"

#include "qtobjecteditor.h"

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


class QtScopedTimer
{
    const char* scope;
    std::chrono::time_point<std::chrono::high_resolution_clock> started;

public:
    inline QtScopedTimer(const char* scopeName) :
        scope(scopeName) {
        started = std::chrono::high_resolution_clock::now();
    }
    inline ~QtScopedTimer() {
        std::chrono::duration<double, std::micro> diff = std::chrono::high_resolution_clock::now() - started;
        qDebug() << scope << ":" << diff.count() << "\xC2\xB5s";
    }
};

class QtObjectEditorPrivate
{
    QtObjectEditor *q_ptr;
    Q_DECLARE_PUBLIC(QtObjectEditor)
    Q_DECLARE_TR_FUNCTIONS(QtObjectEditorPrivate)
public:

    QtObjectEditorPrivate(QtObjectEditor* q);

    void initUi();

    int enumToInt(const QMetaEnum &metaEnum, int enumValue) const;
    int intToEnum(const QMetaEnum &metaEnum, int intValue) const;
    int flagToInt(const QMetaEnum &metaEnum, int flagValue) const;
    int intToFlag(const QMetaEnum &metaEnum, int intValue) const;
    bool isSubValue(int value, int subValue) const;
    bool isPowerOf2(int n) const;

    QString translateClass(const QMetaObject* metaObject) const;
    QString translateProperty(const QMetaObject* metaObject, const char* propertyName)const;
    bool matchFilter(const QRegExp& regExp, const char* key) const;

    void updateClassProperties(const QMetaObject *metaObject, bool recursive);
    void addClassProperties(const QMetaObject *metaObject, const QRegExp& propertyRx = QRegExp(), const QRegExp &classRx = QRegExp());
    void addDynamicProperties(QObject* object, const QRegExp& propertyRx = QRegExp());

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

    QObject*                     object;
    QScrollArea*                 scroll;
    QWidget*                     widget;
    QtAbstractPropertyBrowser*   browser;
    QtVariantPropertyManager*    manager;
    QtVariantPropertyManager*    readOnlyManager;
    QtVariantEditorFactory*      factory;
    QVBoxLayout*                 layout;

    QtObjectEditor::ViewType     viewType;
    QtObjectEditor::SubmitPolicy submitPolicy;

    bool isFinal;
    bool isNotificable;
};

QtObjectEditorPrivate::QtObjectEditorPrivate(QtObjectEditor *q) :
    q_ptr(q),
    object(0),
    scroll(0),
    widget(0),
    browser(0),
    manager(new QtVariantPropertyManager(q)),
    readOnlyManager(new QtVariantPropertyManager(q)),
    factory(new QtVariantEditorFactory(q)),
    viewType(QtObjectEditor::TreeView),
    submitPolicy(QtObjectEditor::AutoSubmit),
    isFinal(false),
    isNotificable(false)
{
}

void QtObjectEditorPrivate::initUi()
{
    QObject::connect(manager, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
                     q_ptr, SLOT(slotValueChanged(QtProperty *, const QVariant &)));

    layout = new QVBoxLayout(q_ptr);
    layout->setMargin(0);
    q_ptr->setViewType(QtObjectEditor::TreeView);
}

int QtObjectEditorPrivate::enumToInt(const QMetaEnum &metaEnum, int enumValue) const
{
    //QtScopedTimer timer(Q_FUNC_INFO);
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

int QtObjectEditorPrivate::intToEnum(const QMetaEnum &metaEnum, int intValue) const
{
    //QtScopedTimer timer(Q_FUNC_INFO);
    QSet<int> valueMap; // dont show multiple enum values which have the same values
    QVector<int> values;
    values.reserve(metaEnum.keyCount());
    for (int i = 0; i < metaEnum.keyCount(); i++) {
        int value = metaEnum.value(i);
        if (!valueMap.contains(value)) {
            valueMap.insert(value);
            values.append(value);
        }
    }
    if (intValue >= values.count())
        return -1;
    return values.at(intValue);
}

bool QtObjectEditorPrivate::isSubValue(int value, int subValue) const
{
    //QtScopedTimer timer(Q_FUNC_INFO);
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

bool QtObjectEditorPrivate::isPowerOf2(int n) const
{
    return (n > 0 && ((n & (n-1)) == 0));
}

QString QtObjectEditorPrivate::translateClass(const QMetaObject *metaObject) const
{
    int index = metaObject->indexOfClassInfo("className");
    if (index != -1)
        return tr( metaObject->classInfo(index).value() );
    return metaObject->className();
}

int QtObjectEditorPrivate::flagToInt(const QMetaEnum &metaEnum, int flagValue) const
{
    //QtScopedTimer timer(Q_FUNC_INFO);
    if (!flagValue)
        return 0;
    int intValue = 0;
    QMap<int, int> valueMap; // dont show multiple enum values which have the same values
    int pos = 0;
    for (int i = 0; i < metaEnum.keyCount(); i++) {
        int value = metaEnum.value(i);
        if (!valueMap.contains(value) && isPowerOf2(value)) {
            if (isSubValue(flagValue, value))
                intValue |= (1 << pos);
            valueMap[value] = pos++;
        }
    }
    return intValue;
}

int QtObjectEditorPrivate::intToFlag(const QMetaEnum &metaEnum, int intValue) const
{
    //QtScopedTimer timer(Q_FUNC_INFO);
    QSet<int> valueMap; // dont show multiple enum values which have the same values
    QVector<int> values;
    valueMap.reserve(metaEnum.keyCount());
    for (int i = 0; i < metaEnum.keyCount(); i++) {
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

QString QtObjectEditorPrivate::translateProperty(const QMetaObject *metaObject, const char *propertyName) const
{
    int index = metaObject->indexOfClassInfo(propertyName);
    if (index != -1)
        return QObject::tr( metaObject->classInfo(index).value() );
    return propertyName;
}

bool QtObjectEditorPrivate::matchFilter(const QRegExp &regExp, const char *key) const
{
    return (regExp.isValid() && (QString(key).contains(regExp)));
}

void QtObjectEditorPrivate::updateClassProperties(const QMetaObject *metaObject, bool recursive)
{
    if (!metaObject)
        return;

    if (recursive)
        updateClassProperties(metaObject->superClass(), recursive);

    QtProperty *classProperty = classToProperty.value(metaObject);
    if (!classProperty)
        return;

    for (int idx = metaObject->propertyOffset(); idx < metaObject->propertyCount(); idx++) {
        QMetaProperty metaProperty = metaObject->property(idx);
        if (metaProperty.isReadable()) {
            if (classToIndexToProperty.contains(metaObject) && classToIndexToProperty[metaObject].contains(idx)) {
                QtVariantProperty *subProperty = classToIndexToProperty[metaObject][idx];
                if (metaProperty.isEnumType()) {
                    if (metaProperty.isFlagType())
                        subProperty->setValue(flagToInt(metaProperty.enumerator(), metaProperty.read(object).toInt()));
                    else
                        subProperty->setValue(enumToInt(metaProperty.enumerator(), metaProperty.read(object).toInt()));
                } else {
                    subProperty->setValue(metaProperty.read(object));
                }
            }
        }
    }
}

void QtObjectEditorPrivate::addClassProperties(const QMetaObject *metaObject, const QRegExp& propertyRx, const QRegExp& classRx)
{
    if (!metaObject)
        return;

    if (!isFinal)
        addClassProperties(metaObject->superClass(), propertyRx, classRx);

    QtProperty *classProperty = classToProperty.value(metaObject);
    if (!classProperty)
    {
        if (!matchFilter(classRx, metaObject->className()))
            return;

        classProperty = manager->addProperty(QtVariantPropertyManager::groupTypeId(), translateClass(metaObject));
        classToProperty[metaObject] = classProperty;
        propertyToClass[classProperty] = metaObject;

        QSet<int> valueSet;
        valueSet.reserve(16);

        QString propertyName;
        propertyName.reserve(64);

        for (int idx = metaObject->propertyOffset(); idx < metaObject->propertyCount(); idx++)
        {
            QMetaProperty metaProperty = metaObject->property(idx);

            int type = metaProperty.userType();
            QtVariantProperty *subProperty = 0;

            if (!matchFilter(propertyRx, metaProperty.name()))
                continue;

            propertyName = translateProperty(metaObject, metaProperty.name());

            if (!metaProperty.isReadable()) {
                subProperty = readOnlyManager->addProperty(QVariant::String, propertyName);
                subProperty->setValue(QLatin1String("< Non Readable >"));
            } else if (metaProperty.isEnumType()) {
                if (metaProperty.isFlagType()) {
                    subProperty = manager->addProperty(QtVariantPropertyManager::flagTypeId(), propertyName);
                    QMetaEnum metaEnum = metaProperty.enumerator();
                    QStringList flagNames;
                    for (int i = 0; i < metaEnum.keyCount(); i++) {
                        int value = metaEnum.value(i);
                        if (!valueSet.contains(value) && isPowerOf2(value)) { // dont show multiple enum values which have the same values
                            valueSet.insert(value);
                            flagNames.append(QLatin1String(metaEnum.key(i)));
                        }
                        subProperty->setAttribute(QLatin1String("flagNames"), flagNames);
                        subProperty->setValue(flagToInt(metaEnum, metaProperty.read(object).toInt()));
                    }
                    valueSet.clear();
                } else {
                    subProperty = manager->addProperty(QtVariantPropertyManager::enumTypeId(), propertyName);
                    QMetaEnum metaEnum = metaProperty.enumerator();
                    QStringList enumNames;
                    for (int i = 0; i < metaEnum.keyCount(); i++) {
                        int value = metaEnum.value(i);
                        if (!valueSet.contains(value)) { // dont show multiple enum values which have the same values
                            valueSet.insert(value);
                            enumNames.append(QLatin1String(metaEnum.key(i)));
                        }
                    }
                    valueSet.clear();
                    subProperty->setAttribute(QLatin1String("enumNames"), enumNames);
                    subProperty->setValue(enumToInt(metaEnum, metaProperty.read(object).toInt()));
                }
            } else if (manager->isPropertyTypeSupported(type)) {
                if (!metaProperty.isWritable())
                    subProperty = readOnlyManager->addProperty(type, propertyName + QLatin1String(" (Non Writable)"));
                if (!metaProperty.isDesignable())
                    subProperty = readOnlyManager->addProperty(type, propertyName + QLatin1String(" (Non Designable)"));
                else
                    subProperty = manager->addProperty(type, propertyName);
                subProperty->setValue(metaProperty.read(object));
            } else {
                subProperty = readOnlyManager->addProperty(QVariant::String, propertyName);
                subProperty->setValue(QLatin1String("< Unknown Type >"));
                subProperty->setEnabled(false);
            }
            classProperty->addSubProperty(subProperty);
            propertyToIndex[subProperty] = idx;
            classToIndexToProperty[metaObject][idx] = subProperty;
        }
    } else {
        updateClassProperties(metaObject, false);
    }

    topLevelProperties.append(classProperty);

    QtBrowserItem* browserItem = browser->addProperty(classProperty);
    QtTreePropertyBrowser* treeBrowser = qobject_cast<QtTreePropertyBrowser*>(browser);
    if (treeBrowser) {
        treeBrowser->setBackgroundColor(browserItem, colorMap[(classToProperty.size() - 1) % Q_COUNTOF(colorMap)]);
    }
}

void QtObjectEditorPrivate::addDynamicProperties(QObject *object, const QRegExp& propertyRx)
{
    const QMetaObject* metaObject = object->metaObject();
    QtProperty *classProperty = classToProperty.value(metaObject);
    QList<QByteArray> propertyList = object->dynamicPropertyNames();
    QtVariantProperty *subProperty = 0;
    int idx = metaObject->propertyCount();
    for (auto it = propertyList.begin(); it != propertyList.end(); ++it, ++idx)
    {
        if (it->startsWith("_q_"))
            continue;

        QString propertyName = *it;
        if (propertyRx.isValid() && (!propertyName.contains(propertyRx)))
            continue;

        int propId = metaObject->indexOfClassInfo(*it);
        if (propId != -1)
            propertyName = tr( metaObject->classInfo(propId).value() );

        QVariant value = object->property(*it);
        subProperty = manager->addProperty(value.type(), propertyName);
        subProperty->setValue(value);
        classProperty->addSubProperty(subProperty);
        dynamicProperties.insert(subProperty);
    }
}



QtObjectEditor::QtObjectEditor(QWidget *parent)
    : QWidget(parent), d_ptr(new QtObjectEditorPrivate(this))
{
    Q_D(QtObjectEditor);
    d->initUi();
}

QtObjectEditor::~QtObjectEditor()
{
    delete d_ptr;
}

void QtObjectEditor::setViewType(ViewType type)
{
    Q_D(QtObjectEditor);

    if (d->viewType == type && d->browser != 0) {
        return;
    }

    if (d->object) {
        //qDeleteAll(d->topLevelProperties);
        d->dynamicProperties.clear();
        d->topLevelProperties.clear();
        d->browser->clear();
        d->manager->clear();
        d->readOnlyManager->clear();
        d->classToIndexToProperty.clear();
        d->classToProperty.clear();
        d->propertyToClass.clear();
        d->propertyToIndex.clear();
        d->propertyToExpanded.clear();
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
    d->browser->setFactoryForManager(d->manager, d->factory);
    if (d->object) {
        d->addClassProperties(d->object->metaObject(), QRegExp(d->propertyFilter, Qt::CaseInsensitive, QRegExp::FixedString));
        d->addDynamicProperties(d->object, QRegExp(d->propertyFilter, Qt::CaseInsensitive, QRegExp::FixedString));
    }
    d->browser->update();
}

QtObjectEditor::ViewType QtObjectEditor::viewType() const
{
    Q_D(const QtObjectEditor);
    return d->viewType;
}

void QtObjectEditor::setSubmitPolicy(QtObjectEditor::SubmitPolicy policy)
{
    Q_D(QtObjectEditor);
    d->submitPolicy = policy;
}

QtObjectEditor::SubmitPolicy QtObjectEditor::submitPolicy() const
{
    Q_D(const QtObjectEditor);
    return d->submitPolicy;
}

void QtObjectEditor::setFinal(bool on)
{
    Q_D(QtObjectEditor);
    d->isFinal = on;

    d->classFilter.clear();
    d->propertyFilter.clear();

    d->browser->clear();
    d->manager->clear();
    d->readOnlyManager->clear();
    d->classToIndexToProperty.clear();
    d->classToProperty.clear();
    d->propertyToClass.clear();
    d->propertyToIndex.clear();
    d->propertyToExpanded.clear();

    d->dynamicProperties.clear();
    d->topLevelProperties.clear();

    if (d->object) {
        d->addClassProperties(d->object->metaObject(), QRegExp(d->propertyFilter), QRegExp(d->classFilter));
        d->addDynamicProperties(d->object, QRegExp(d->propertyFilter));
    }
}

bool QtObjectEditor::isFinal() const
{
    Q_D(const QtObjectEditor);
    return d->isFinal;
}



void QtObjectEditor::setObject(QObject *object)
{
    Q_D(QtObjectEditor);
    if (d->object == object)
        return;

    if (d->object != 0) {
        disconnect(d->object, SIGNAL(destroyed(QObject*)),
                   this, SLOT(objectDestroyed(QObject*)));
    }

    d->originalCache.clear();
    d->propertyFilter.clear();

    d->browser->clear();
    d->manager->clear();
    d->readOnlyManager->clear();
    d->classToIndexToProperty.clear();
    d->classToProperty.clear();
    d->propertyToClass.clear();
    d->propertyToIndex.clear();
    d->propertyToExpanded.clear();

    d->dynamicProperties.clear();
    d->topLevelProperties.clear();

    d->object = object;
    if (!d->object) {
        return;
    }

    connect(object, SIGNAL(destroyed(QObject*)),
        SLOT(objectDestroyed(QObject*)));

    d->addClassProperties(d->object->metaObject(), QRegExp(d->propertyFilter), QRegExp(d->classFilter));
    d->addDynamicProperties(d->object, QRegExp(d->propertyFilter));
}

QObject *QtObjectEditor::object() const
{
    Q_D(const QtObjectEditor);
    return d->object;
}


void QtObjectEditor::setPropertyFilter(const QString& pattern)
{
    Q_D(QtObjectEditor);
    d->propertyFilter = pattern;
    emit propertyFilterChanged(pattern);
    filterProperties(QRegExp(d->propertyFilter, Qt::CaseInsensitive, QRegExp::FixedString));
}

QString QtObjectEditor::propertyFilter() const {
    Q_D(const QtObjectEditor);
    return d->propertyFilter;
}

void QtObjectEditor::filterProperties(const QRegExp &rx)
{
    Q_D(QtObjectEditor);
    if (!d->object)
        return;

    QSignalBlocker blocker(d->manager);

    d->browser->clear();
    d->manager->clear();
    d->readOnlyManager->clear();
    d->classToIndexToProperty.clear();
    d->classToProperty.clear();
    d->propertyToClass.clear();
    d->propertyToIndex.clear();
    d->propertyToExpanded.clear();

    d->topLevelProperties.clear();

    d->addClassProperties(d->object->metaObject(), rx, QRegExp(d->classFilter));
    d->addDynamicProperties(d->object, QRegExp(d->propertyFilter));
    repaint();
}


void QtObjectEditor::setClassFilter(const QString &pattern)
{
    Q_D(QtObjectEditor);
    d->classFilter = pattern;
    emit propertyFilterChanged(pattern);
    filterClasses(QRegExp(d->classFilter, Qt::CaseSensitive, QRegExp::FixedString));
}

QString QtObjectEditor::classFilter() const
{
    Q_D(const QtObjectEditor);
    return d->classFilter;
}

void QtObjectEditor::enableNotifications(bool on)
{
    Q_D(QtObjectEditor);
    d->isNotificable = on;
}

bool QtObjectEditor::notificationsEnabled() const
{
    Q_D(const QtObjectEditor);
    return d->isNotificable;
}

void QtObjectEditor::filterClasses(const QRegExp &rx)
{
    Q_D(QtObjectEditor);
    if (!d->object)
        return;

    d->browser->clear();
    d->manager->clear();
    d->readOnlyManager->clear();

    d->classToIndexToProperty.clear();
    d->classToProperty.clear();
    d->propertyToClass.clear();
    d->propertyToIndex.clear();
    d->propertyToExpanded.clear();

    d->topLevelProperties.clear();

    d->addClassProperties(d->object->metaObject(), QRegExp(d->propertyFilter), rx);
    d->addDynamicProperties(d->object, QRegExp(d->propertyFilter));
}

void QtObjectEditor::slotValueChanged(QtProperty *property, const QVariant &value)
{
    Q_D(QtObjectEditor);
    if (!d->object)
        return;

    if (d->isNotificable) {
        emit propertyChanged(property->propertyName(), value);
    }

    if (d->dynamicProperties.contains(static_cast<QtVariantProperty*>(property))) {
        d->object->setProperty(property->propertyName().toLatin1(), static_cast<QtVariantProperty*>(property)->value());
        return;
    }

    if (!d->propertyToIndex.contains(property)) {
        //d->addClassProperties(d->object->metaObject(), QRegExp(d->propertyFilter), QRegExp(d->classFilter));
        return;
    }
    int idx = d->propertyToIndex.value(property);

    const QMetaObject *metaObject = d->object->metaObject();
    QMetaProperty metaProperty = metaObject->property(idx);

    if (d->submitPolicy == ManualSubmit) { // if manual submit
        // store original value if it isn't already stored
        QVariantHash::const_iterator it = d->originalCache.constFind(metaProperty.name());
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

    d->updateClassProperties(metaObject, true);
}

void QtObjectEditor::submit()
{
    Q_D(QtObjectEditor);
    // does nothing because changes is already applied to object
    // simply clear cached values
    d->originalCache.clear();
}

void QtObjectEditor::revert()
{
    Q_D(QtObjectEditor);
    if (!d->object)
        return;

    if (d->submitPolicy != ManualSubmit) {
        return;
    }
    // revert the changes to original values
    const QMetaObject *metaObject = d->object->metaObject();
    QVariantHash::const_iterator it = d->originalCache.begin();
    for (; it != d->originalCache.end(); ++it)
    {
        QString name = it.key();
        QVariant value = it.value();

        QMetaProperty metaProperty = metaObject->property(metaObject->indexOfProperty(name.toLatin1()));
        if (metaProperty.isEnumType()) {
            if (metaProperty.isFlagType())
                metaProperty.write(d->object, d->intToFlag(metaProperty.enumerator(), value.toInt()));
            else
                metaProperty.write(d->object, d->intToEnum(metaProperty.enumerator(), value.toInt()));
        } else {
            metaProperty.write(d->object, value);
        }
    }
    d->updateClassProperties(metaObject, true);
}

void QtObjectEditor::refresh()
{
    Q_D(QtObjectEditor);
    if (d->object) {
        d->updateClassProperties(d->object->metaObject(), true);
    }
}

void QtObjectEditor::objectDestroyed(QObject *object)
{
    Q_D(QtObjectEditor);
    if (object == d->object) {
        setObject(Q_NULLPTR);
    }
}
