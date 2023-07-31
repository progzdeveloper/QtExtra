#pragma once
#include <QAbstractTableModel>
#include <QtPluginsExt>

struct QtPluginMetadata;
class QtPluginPropertyModelPrivate;

class QTPLUGINS_EXPORT QtPluginPropertyModel :
        public QAbstractTableModel
{
    Q_OBJECT

public:
    enum ColumnType
    {
        TitleColumn,
        ValueColumn
    };

    explicit QtPluginPropertyModel(QObject *parent = Q_NULLPTR);
    ~QtPluginPropertyModel();

    void setMetadata(const QtPluginMetadata& metadata);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    int columnCount(const QModelIndex& = QModelIndex()) const { return ValueColumn+1; }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

public Q_SLOTS:
    void clear();

private:
    QScopedPointer<class QtPluginPropertyModelPrivate> d;
};
