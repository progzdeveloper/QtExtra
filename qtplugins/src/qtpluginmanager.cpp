#include "qtpluginmanager.h"
#include "qtplugininterface.h"
#include "qtpluginmetadata.h"

#include <QSet>
#include <QMultiHash>
#include <QStringList>

#include <QDir>
#include <QDirIterator>
#include <QFileInfo>

#include <QCoreApplication>
#include <QPluginLoader>
#include <QLibrary>

#include <QDebug>


inline uint qHash(QtPluginInterface* iface) {
    return (iface != Q_NULLPTR ? 0 : qHash(iface->iid()));
}

class QtPluginManagerPrivate
{
public:
    QScopedPointer<QPluginLoader> loader;
    QMultiHash<QString, QString> classMap;
    QSet<QtPluginInterface*> interfaces;
    QHash<QString, QtPluginMetadata> metadata;

    QtPluginManagerPrivate(QtPluginManager* q);
    ~QtPluginManagerPrivate();

    bool isLoaded(const QString& path) const;
};


QtPluginManagerPrivate::QtPluginManagerPrivate(QtPluginManager *q) :
    loader(new QPluginLoader(q))
{
}

QtPluginManagerPrivate::~QtPluginManagerPrivate()
{
    qDeleteAll(interfaces);
}

bool QtPluginManagerPrivate::isLoaded(const QString &path) const
{
    for (auto it = metadata.cbegin(); it != metadata.cend(); ++it)
    {
        if (it->libPath == path && it->isLoaded)
            return true;
    }
    return false;
}


QtPluginManager::QtPluginManager()
    : QObject(Q_NULLPTR)
    , d(new QtPluginManagerPrivate(this))
{
}

QtPluginManager::~QtPluginManager()
{
}

void QtPluginManager::registrate(QtPluginInterface *iface)
{
    auto it = d->interfaces.constFind(iface);
    if (it != d->interfaces.cend())
    {
        if (*it)
            delete *it;
        d->interfaces.erase(it);
    }
    d->interfaces.insert(iface);
}

QStringList QtPluginManager::keys(const QString &iid) const
{
    return (iid.isEmpty() ? d->classMap.values() : d->classMap.values(iid));
}

QStringList QtPluginManager::iids() const
{
    QStringList iids;
    for (auto it = d->interfaces.cbegin(); it != d->interfaces.cend(); ++it)
        iids << (*it)->iid();
    return iids;
}

QString QtPluginManager::category(const QString &iid) const
{
    for (auto it = d->interfaces.cbegin(); it != d->interfaces.cend(); ++it)
    {
        if ((*it)->iid() == iid)
            return (*it)->category();
    }
    return QtPluginInterface::uncategorized();
}

const QtPluginMetadata &QtPluginManager::metadata(const QString &key) const
{
    static QtPluginMetadata invalidMetaData;
    auto it = d->metadata.constFind(key);
    return (it != d->metadata.cend() ? *it : invalidMetaData);
}

QtPluginManager &QtPluginManager::instance()
{
    static QtPluginManager manager;
    return manager;
}

void QtPluginManager::load()
{
    const QString appPath = qApp->applicationFilePath();
    const QObjectList instances = QPluginLoader::staticInstances();
    for (auto it = instances.cbegin(); it != instances.cend(); ++it)
        resolve(*it, appPath);
}

void QtPluginManager::load(const QString &path)
{
    if (path.isEmpty())
        return;

    const QFileInfo fileInfo(path);
    if (fileInfo.isFile())
    {
        if (!QLibrary::isLibrary(path))
            return;
        if (d->metadata.contains(fileInfo.absoluteFilePath()))
            return;
        d->loader->setFileName(fileInfo.absoluteFilePath());
        loadPlugin(d->loader.data());
    }
    else
    {
        QDirIterator it(fileInfo.path(), QDir::Files, QDirIterator::FollowSymlinks);
        for(; it.hasNext(); it.next())
        {
            const QString pluginPath = it.fileName();
            const QFileInfo pluginInfo(path);
            if (!QLibrary::isLibrary(pluginPath))
                continue;
            if (d->metadata.contains(pluginInfo.absoluteFilePath()))
                return;
            d->loader->setFileName(pluginInfo.absoluteFilePath());
            loadPlugin(d->loader.data());
        }
    }
}

void QtPluginManager::load(const QStringList &paths)
{
    for (auto it = paths.cbegin(); it != paths.cend(); ++it)
        load(*it);
}

void QtPluginManager::load(const QDir &pluginsDir)
{
    const QStringList paths = pluginsDir.entryList(QDir::Files|QDir::NoDotAndDotDot);
    for (auto it = paths.cbegin(); it != paths.cend(); ++it)
        load(pluginsDir.absoluteFilePath(*it));
}

void QtPluginManager::loadPlugin(QPluginLoader* loader)
{
    const QString fileName = loader->fileName();
    Q_EMIT loading(fileName);
    if (!loader->load())
    {
        QtPluginMetadata pluginInfo;
        pluginInfo.instance = Q_NULLPTR;
        pluginInfo.iid = tr("<Unknown>");
        pluginInfo.isLoaded = false;
        pluginInfo.isStatic = false;
        pluginInfo.libPath = fileName;
        pluginInfo.errorString = loader->errorString();
        d->metadata.insertMulti(pluginInfo.key, pluginInfo);
        Q_EMIT failed(fileName, loader->errorString());
    }
    else
    {
        if (!resolve(loader->instance(), fileName)) {
            Q_EMIT failed(fileName, "plugin does not support any of provided interface");
            loader->unload();
        }
    }
}

bool QtPluginManager::resolve(QObject *instance, const QString& filePath)
{
    int resolvedCount = 0;

    QtPluginInterface* interface = Q_NULLPTR;
    for (auto it = d->interfaces.cbegin(); it != d->interfaces.cend(); ++it)
    {
        interface = *it;
        if (interface->resolve(instance))
        {
            QtPluginMetadata pluginInfo;
            pluginInfo.instance = instance;
            pluginInfo.iid = interface->iid();
            pluginInfo.isLoaded = true;
            pluginInfo.isStatic = false;
            pluginInfo.libPath = filePath;
            pluginInfo.key = instance->metaObject()->className();

            d->metadata.insertMulti(pluginInfo.key, pluginInfo);
            d->classMap.insertMulti(pluginInfo.iid, pluginInfo.key);

            Q_EMIT loaded(filePath, (*it)->iid());
            ++resolvedCount;
        }
    }
    return (resolvedCount > 0);
}


