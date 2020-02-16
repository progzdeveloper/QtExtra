#ifndef QOPTIONMODEL_H
#define QOPTIONMODEL_H

#include <QAbstractTableModel>

class QOptionModel :
    public QAbstractTableModel
{
    Q_OBJECT

public:
    enum ColumnType
    {
        ColumnExpr = 0,
        ColumnVal,
        ColumnImp,
        ColumnReq,
        ColumnText,
        COLUMN_COUNT = (ColumnText + 1)
    };


    explicit QOptionModel(QObject *parent = nullptr);

    bool insertRows(int row, int count, const QModelIndex &parent) override;
    bool removeRows(int row, int count, const QModelIndex &parent) override;
    bool moveRows(const QModelIndex &sourceParent,
                                      int sourceRow, int count,
                                      const QModelIndex &destinationParent, int destinationChild) override;

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent) const;

    // ###
    QString generateCppCode(const QString& className = "command_line") const;

public Q_SLOTS:
    void clear();

private:
    class QOptionModelPrivate* d_ptr;
    Q_DECLARE_PRIVATE(QOptionModel)
};

#endif // QOPTIONMODEL_H
