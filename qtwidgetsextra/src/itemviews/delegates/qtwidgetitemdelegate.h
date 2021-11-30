#ifndef QTWIDGETITEMDELEGATE_H
#define QTWIDGETITEMDELEGATE_H

#include <QStyledItemDelegate>

#include <QtWidgetsExtra>

class QtItemWidget;

//
// class QtWidgetItemDelegate provides the ability
// to embed custom widgets as elements of QAbstractItemView
//
class QTWIDGETSEXTRA_EXPORT QtWidgetItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    enum RenderHint
    {
        RenderDirect, // render directly into view viewport
        RenderCached  // cached rendering
    };

    explicit QtWidgetItemDelegate(QObject* parent = Q_NULLPTR);
    ~QtWidgetItemDelegate();

    void setAutoFillBackground(bool on);
    bool isAutoFillBackground() const;

    void setStaticContent(bool on);
    bool isStaticContent() const;

    void setCacheEnabled(bool enabled);
    bool isCacheEnabled() const;

    void setCacheLimit(int cacheSize);
    int cacheLimit() const;

    // QAbstractItemDelegate interface
public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) Q_DECL_OVERRIDE;

public Q_SLOTS:
    void invalidate();
    void invalidateIndex(const QModelIndex& index);
    void invalidateRange(const QModelIndex& topLeft, const QModelIndex& bottomRight);

Q_SIGNALS:
    void requestRepaint();

    // QObject interface
protected:
    bool eventFilter(QObject *object, QEvent *event) Q_DECL_OVERRIDE;

protected:
    virtual bool eventHandler(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
    virtual QtItemWidget* createItemWidget() const = 0;
    virtual void updateWidgetData(const QModelIndex& index, const QStyleOptionViewItem &option) const;
    virtual RenderHint renderHint(const QStyleOptionViewItem &option, const QModelIndex &) const;

    QtItemWidget* widget() const;

protected:
    void createWidgetOnDemand() const;

private:
    QT_PIMPL(QtWidgetItemDelegate)
};

struct WidgetState;
class QTWIDGETSEXTRA_EXPORT QtItemWidget : public QWidget
{
    Q_OBJECT

public:
    friend class QtWidgetItemDelegate;

    explicit QtItemWidget(QWidget* parent);
    virtual ~QtItemWidget();

    virtual void setData(const QModelIndex&, const QStyleOptionViewItem&);

protected:
    virtual bool viewportEvent(QEvent *e, QWidget*);

    bool isMouseOver(QWidget* w, QMouseEvent* e) const;

private:
    bool handleEvent(QEvent* e, QWidget* w, WidgetState& wstate);
};


#endif // QTWIDGETITEMDELEGATE_H
