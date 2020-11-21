#ifndef QTCOLORGRID_H
#define QTCOLORGRID_H

#include <QVector>
#include <QList>
#include <QSet>

#include <QListView>
#include <QtWidgetsExtra>


class QtColorPalette;

class QtColorGrid :
        public QListView
{
    Q_OBJECT
public:
    explicit QtColorGrid(QWidget *parent = Q_NULLPTR);
    ~QtColorGrid();

    void setColors(const QVector<QColor>& colors);
    void setColors(const QList<QColor> &colors);
    void setColors(const QtColorPalette &palette);
    const QtColorPalette& colors() const;

    QColor currentColor() const;

public Q_SLOTS:
    void clear();

    // QAbstractItemView interface
protected Q_SLOTS:
    void currentChanged(const QModelIndex &current, const QModelIndex &previous) Q_DECL_OVERRIDE;

private Q_SLOTS:
    void resizeIcons(const QSize& size);

Q_SIGNALS:
    void colorsChanged();
    void colorChanged(const QColor& c);

    // QWidget interface
protected:
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
    void setModel(QAbstractItemModel *model) Q_DECL_OVERRIDE;

private:
    QT_PIMPL(QtColorGrid)
};

#endif // QTCOLORGRID_H
