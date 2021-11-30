#ifndef WIDGET_H
#define WIDGET_H

#include <unordered_set>

#include <QtWidgets>

#include <QtWidgetItemDelegate>


class QtFileListModel;

class FileItemDelegate;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = Q_NULLPTR);
    ~Widget();

private Q_SLOTS:
    void selectFiles();
    void onModelReset();
    void removeItem(const QModelIndex& index);
    void showContextMenu(const QModelIndex& index);
    void showContextMenuAround(const QModelIndex &index, const QRect& rectHint);
    void showContextMenuAt(const QModelIndex& index, const QPoint& globalPos);

protected:
    bool eventFilter(QObject* object, QEvent* event);

private:
    QMenu* createMenu(const QModelIndex&) const;

private:
    FileItemDelegate* delegate;
    QLabel* label;
    QtFileListModel* model;
    QTableView* tableView;
    QListView* listView;
};


class PreviewLabel;
class FileItemWidget : public QtItemWidget
{
    Q_OBJECT
public:
    explicit FileItemWidget(bool isStatic, QWidget* parent = Q_NULLPTR);

    void setData(const QModelIndex& index, const QStyleOptionViewItem &option) Q_DECL_OVERRIDE;

    void updateProgress();

Q_SIGNALS:
    void removeItem();

    void requestContextMenu();
    void requestContextMenuAround(const QRect&);
    void requestContextMenuAt(const QPoint&);

protected:
    bool viewportEvent(QEvent* e, QWidget* w) Q_DECL_OVERRIDE;

private:
    QString elidedText(const QString& text) const;

private:
    QPalette selectedPalette;
    PreviewLabel* previewLabel;
    QLabel* pathLabel;
    QLabel* sizeLabel;
    QPushButton* sampleButton;
    QToolButton* moreButton;
    QToolButton* removeButton;
    bool staticContents;
};

//
// Example of delegate for displaying static-only content
// (no dynamic/clickable elements)
//
class ItemWidgetDelegate : public QtWidgetItemDelegate
{
    Q_OBJECT
public:
    explicit ItemWidgetDelegate(QObject* parent = Q_NULLPTR);

protected:
    QtItemWidget *createItemWidget() const Q_DECL_OVERRIDE;
};


class FileItemDelegate : public QtWidgetItemDelegate
{
    Q_OBJECT
public:
    explicit FileItemDelegate(QObject* parent = Q_NULLPTR);

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;

public Q_SLOTS:
    void onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);

private Q_SLOTS:
    void itemRemoveRequest();

    void itemMenuRequest();
    void itemMenuRequestAt(const QPoint& globalPos);
    void itemMenuRequestAround(const QRect& rectHint);

    void updateProgress();

Q_SIGNALS:
    void removeIndex(const QModelIndex&);

    void showItemMenu(const QModelIndex&);
    void showItemMenuAt(const QModelIndex&, const QPoint&);
    void showItemMenuAround(const QModelIndex&, const QRect&);

    // QtWidgetItemDelegate interface
protected:
    RenderHint renderHint(const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    QtItemWidget *createItemWidget() const Q_DECL_OVERRIDE;

private:
    QTimer* timer;
    mutable FileItemWidget* itemWidget;
    mutable std::unordered_set<quint64> lookup;
};


#endif // WIDGET_H
