#pragma once
#include <QtPluginsExt>

class QObject;

struct QtPluginMetadata
{
    QObject *instance;
    QString key;
    QString libPath;
    QString iid;
    QString errorString;
    bool isLoaded;
    bool isStatic;
};

