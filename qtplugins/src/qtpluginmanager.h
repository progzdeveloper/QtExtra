#pragma once
#include <QObject>
#include <QtPluginsExt>

class QDir;
class QPluginLoader;

class QtPluginInterface;

template<class>
class QtGenericInterface;

struct QtPluginMetadata;


class QTPLUGINS_EXPORT QtPluginManager :
        public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(QtPluginManager)
public:
    virtual ~QtPluginManager();

    void registrate(QtPluginInterface* iface);

    template<class _CustomInterface>
    inline void registrate() {
        registrate(new _CustomInterface());
    }

    QStringList keys(const QString& iid = QString()) const;
    QStringList iids() const;
    QString category(const QString& iid) const;
    const QtPluginMetadata& metadata(const QString& key) const;

    static QtPluginManager& instance();

Q_SIGNALS:
    void loading(const QString& path);
    void loaded(const QString& path, const QString& iid);
    void failed(const QString& path, const QString& text);

public Q_SLOTS:
    void load();
    void load(const QString& path);
    void load(const QStringList& paths);
    void load(const QDir& pluginsDir);

private:
    void loadPlugin(QPluginLoader& loader);
    bool resolve(QObject* instance, const QString &filePath);

private:
    explicit QtPluginManager();
    QT_PIMPL(QtPluginManager)
};



#define QT_PLUGIN_INTERFACE(_Class) static QtPluginInterfaceRegisterer<_Class> __##_Class##_instance;

template<class _Interface>
struct QtPluginInterfaceRegisterer {
    QtPluginInterfaceRegisterer() {
        QtPluginManager::instance().registrate<_Interface>();
    }
};




