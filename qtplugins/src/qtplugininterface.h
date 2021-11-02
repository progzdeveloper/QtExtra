#pragma once
#include <QString>
#include <QtPluginsExt>

class QObject;

class QTPLUGINS_EXPORT QtPluginInterface
{
public:
    virtual ~QtPluginInterface();
    virtual bool resolve(QObject* instance) const = 0;
    virtual const QString& iid() const = 0;
    virtual const QString& category() const = 0;

    static const QString& uncategorized();
};

template<class _Interface>
class QtGenericInterface :
        public QtPluginInterface
{
public:
    QtGenericInterface(const QString& category = QtPluginInterface::uncategorized()) :
        _category(category) {
    }

    virtual ~QtGenericInterface() {}

    bool resolve(QObject*) const Q_DECL_OVERRIDE {
        return false;
    }

    const QString& iid() const Q_DECL_OVERRIDE {
        static const QString staticIID(qobject_interface_iid<_Interface*>());
        return staticIID;
    }

    const QString& category() const Q_DECL_OVERRIDE {
        return _category;
    }

private:
    QString _category;
};
