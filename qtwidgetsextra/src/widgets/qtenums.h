#pragma once
#include <QString>
#include <QIcon>
#include <QComboBox>

#include <QtWidgetsExtra>

class QMetaEnum;
class QObject;
class QWidget;
class QAbstractButton;
class QButtonGroup;
class QActionGroup;

#if 0
struct QEnumElement
{
    int value;
    QString text;
    QIcon icon;
};

template<class _Enum>
struct QEnumTraits;


template<class _Enum>
QActionGroup* createActions(_Enum current, QObject* parent = 0)
{
    typedef QEnumTraits<_Enum> TraitsType;
    static const size_t N = TraitsType::Count;
    QEnumElement elements[N];
    if (TraitsType::reflect(elements) != elements + N)
        return Q_NULLPTR;

    QActionGroup* actionGroup = new QActionGroup(parent);
    actionGroup->setObjectName(TraitsType::name());
    for (auto it = elements, e = elements + N; it != e; ++it) {
        QAction* action = actionGroup->addAction(it->icon, it->text);
        action->setObjectName(it->text);
        action->setData(it->value);
        action->setCheckable(true);
        action->setChecked(it->value == static_cast<int>(current));
    }
    return actionGroup;
}

template<class _Enum>
void createItems(_Enum current, QComboBox* comboBox)
{
    typedef QEnumTraits<_Enum> TraitsType;
    static const size_t N = TraitsType::Count;
    QEnumElement elements[N];
    if (TraitsType::reflect(elements) != elements + N)
        return;

    for (auto it = elements, e = elements + N; it != e; ++it) {
        comboBox->addItem(it->icon, it->text, it->value);
    }
    comboBox->setCurrentIndex(comboBox->findData((int)current));
}
#endif

QTWIDGETSEXTRA_EXPORT QActionGroup* createActions(int current, const QMetaEnum& e, QObject* parent = 0);

QTWIDGETSEXTRA_EXPORT void createItems(int current, const QMetaEnum& e, QComboBox* comboBox);



class QTWIDGETSEXTRA_EXPORT QtActionGroupBuilder
{
    Q_DISABLE_COPY(QtActionGroupBuilder)
public:
    static QActionGroup* create(int current, const QMetaEnum& e, QObject* parent = 0);
#if 0
    template<class _Enum>
    static inline QActionGroup* create(_Enum current, QObject* parent = 0) {
        typedef QEnumTraits<_Enum> TraitsType;
        static const size_t N = TraitsType::Count;
        QEnumElement elements[N];
        if (TraitsType::reflect(elements) != elements + N)
            return Q_NULLPTR;
        return createGroup(static_cast<int>(current), elements, N, parent);
    }

private:
    static QActionGroup* createGroup(int current, const QEnumElement* e, int n, QObject * parent);
#endif
private:
    QtActionGroupBuilder() {}
};



class QTWIDGETSEXTRA_EXPORT QtButtonGroupBuilder
{
    Q_DISABLE_COPY(QtButtonGroupBuilder)
    QtButtonGroupBuilder();

public:
    ~QtButtonGroupBuilder();

    template<class _Button>
    static QButtonGroup* create(int current, const QMetaEnum& e, QWidget* parent = 0);
#if 0
    template<class _Enum, class _Button>
    static QButtonGroup* create(_Enum current, QWidget* parent = 0);
#endif
protected:
    QButtonGroup* createGroup(int current, const char* className, const QMetaEnum& e, QWidget* parent) const;
#if 0
    QButtonGroup* createGroup(int current, const char* className, const QEnumElement* e, int n, QWidget* parent) const;
#endif
    virtual QAbstractButton* createButton(const char* className, QWidget* parent) const;

private:
    static QtButtonGroupBuilder& instance();

    QT_PIMPL(QtButtonGroupBuilder)
};


template<class _Button>
inline QButtonGroup * QtButtonGroupBuilder::create(int current, const QMetaEnum & e, QWidget* parent)
{
    static const char* className = _Button::staticMetaObject.className();
    return QtButtonGroupBuilder::instance().createGroup(current, className, e, parent);
}

#if 0
template<class _Enum, class _Button>
inline QButtonGroup * QButtonGroupBuilder::create(_Enum current, QWidget* parent)
{
    typedef QEnumTraits<_Enum> TraitsType;
    static const size_t N = TraitsType::Count;
    static const char* className = _Button::staticMetaObject.className();

    QEnumElement elements[N];
    if (TraitsType::reflect(elements) != elements + N)
        return Q_NULLPTR;
    return QButtonGroupBuilder::instance().createGroup((int)current, className, elements, N, parent);
}
#endif
