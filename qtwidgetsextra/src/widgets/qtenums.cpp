#include <QMetaEnum>
#include <QHash>
#include <QAction>
#include <QActionGroup>
#include <QButtonGroup>
#include <QPushButton>
#include <QRadioButton>
#include <QToolButton>
#include <QCheckBox>
#include <QComboBox>

#include "qtenums.h"


#if 0
QActionGroup * createActions(int current, const QMetaEnum & e, QObject * parent)
{
    if (!e.isValid())
        return Q_NULLPTR;

    QActionGroup* actionGroup = new QActionGroup(parent);
    actionGroup->setObjectName(e.name());
    actionGroup->setExclusive(!e.isFlag());
    for (int i = 0; i < e.keyCount(); i++) {
        QAction* action = actionGroup->addAction(e.key(i));
        action->setObjectName(e.key(i));
        action->setData(e.value(i));
        action->setCheckable(true);
        action->setChecked(e.value(i) == current);
    }
    return actionGroup;
}
#endif

void createItems(int current, const QMetaEnum & e, QComboBox * comboBox)
{
    if (!e.isValid() || comboBox == Q_NULLPTR)
        return;

    for (int i = 0; i < e.keyCount(); i++)
        comboBox->addItem(e.key(i), e.value(i));
    comboBox->setCurrentIndex(comboBox->findData(current));
}



QActionGroup * QtActionGroupBuilder::create(int current, const QMetaEnum & e, QObject * parent)
{
    if (!e.isValid())
        return Q_NULLPTR;

    QActionGroup* actionGroup = new QActionGroup(parent);
    actionGroup->setObjectName(e.name());
    actionGroup->setExclusive(!e.isFlag());
    for (int i = 0; i < e.keyCount(); i++) {
        QAction* action = actionGroup->addAction(e.key(i));
        action->setObjectName(e.key(i));
        action->setData(e.value(i));
        action->setCheckable(true);
        action->setChecked(e.value(i) == current);
    }
    return actionGroup;
}
#if 0
QActionGroup * QtActionGroupBuilder::createGroup(int current, const QEnumElement * elems, int n, QObject * parent)
{
    QActionGroup* actionGroup = new QActionGroup(parent);
    for (auto it = elems, e = elems + n; it != e; ++it) {
        QAction* action = actionGroup->addAction(it->icon, it->text);
        action->setObjectName(it->text);
        action->setData(it->value);
        action->setCheckable(true);
        action->setChecked(it->value == current);
    }
    return actionGroup;
}
#endif



class QtButtonGroupBuilderPrivate
{
public:
    typedef QAbstractButton* (*buttonCreator)(QWidget* parent);
    QHash<QString, buttonCreator> buttonFactory;

    QtButtonGroupBuilderPrivate()
    {
        registrate<QPushButton>();
        registrate<QToolButton>();
        registrate<QRadioButton>();
        registrate<QCheckBox>();
    }

    template<class _Button>
    void registrate() {
        buttonFactory.insert(className<_Button>(), &createButton<_Button>);
    }

    template<class _Button>
    static QAbstractButton* createButton(QWidget* parent) {
        return new _Button(parent);
    }

    template<class _Object>
    static inline const char* className() {
        return _Object::staticMetaObject.className();
    }
};

QtButtonGroupBuilder::QtButtonGroupBuilder() :
    d_ptr(new QtButtonGroupBuilderPrivate)
{
}

QtButtonGroupBuilder::~QtButtonGroupBuilder()
{
}

QButtonGroup * QtButtonGroupBuilder::createGroup(int current, const char* className, const QMetaEnum& e, QWidget* parent) const
{
    if (!e.isValid())
        return Q_NULLPTR;

    QButtonGroup* buttonGroup = new QButtonGroup(parent);
    buttonGroup->setObjectName(e.name());
    buttonGroup->setExclusive(!e.isFlag());
    for (int i = 0; i < e.keyCount(); i++) {
        QAbstractButton* button = createButton(className, parent);
        button->setObjectName(e.key(i));
        button->setText(e.key(i));
        button->setCheckable(true);
        button->setChecked(e.value(i) == current);
        buttonGroup->addButton(button, e.value(i));
    }
    return buttonGroup;
}

#if 0
QButtonGroup * QtButtonGroupBuilder::createGroup(int current, const char * className, const QEnumElement * elems, int n, QWidget* parent) const
{
    QButtonGroup* buttonGroup = new QButtonGroup(parent);
    for (auto it = elems, e = elems + n; it != e; ++it) {
        QAbstractButton* button = createButton(className, parent);
        button->setObjectName(it->text);
        button->setText(it->text);
        button->setIcon(it->icon);
        button->setCheckable(true);
        button->setChecked(it->value == current);
        buttonGroup->addButton(button, it->value);
    }
    return buttonGroup;
}
#endif

QAbstractButton * QtButtonGroupBuilder::createButton(const char * className, QWidget* parent) const
{
    Q_D(const QtButtonGroupBuilder);
    auto it = d->buttonFactory.find(className);
    return (it != d->buttonFactory.end() ? (*it)(parent) : Q_NULLPTR);
}

QtButtonGroupBuilder& QtButtonGroupBuilder::instance()
{
    static QtButtonGroupBuilder globalInstance;
    return globalInstance;
}
