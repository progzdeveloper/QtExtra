#ifndef QTRADIUSESMODEL_H
#define QTRADIUSESMODEL_H

#include <QAbstractTableModel>

using RadiusesVector = QVarLengthArray<double, 16>;

class QtRadiusesModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit QtRadiusesModel(QObject *parent = nullptr);

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

    void reset(int count);

    const RadiusesVector& radiuses() const;

private:
    RadiusesVector radiusesArray;
};

#endif // QTRADIUSESMODEL_H
