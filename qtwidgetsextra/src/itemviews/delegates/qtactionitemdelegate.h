#ifndef QTACTIONITEMDELEGATE_H
#define QTACTIONITEMDELEGATE_H

#include <QtWidgetsExtra>
#include <QStyledItemDelegate>
#include <QActionGroup>

class QAction;

class QTWIDGETSEXTRA_EXPORT QtItemEditor : public QWidget
{
    Q_OBJECT
public:
    QtItemEditor(QWidget* w, QWidget* parent = 0) :
        QWidget(parent),
        editor(w)
    {
        setFocusPolicy(Qt::StrongFocus);
        setAutoFillBackground(true);
    }
    QWidget* widget() const { return editor; }
private:
    Q_DISABLE_COPY(QtItemEditor)
    QWidget* editor;
};




class QtActionItemDelegatePrivate;
class QTWIDGETSEXTRA_EXPORT QtActionItemDelegate :
        public QStyledItemDelegate
{
    Q_OBJECT
    Q_PROPERTY(Qt::ToolButtonStyle toolButtonStyle READ toolButtonStyle WRITE setToolButtonStyle)
    Q_PROPERTY(bool autoRaise READ isAutoRaise WRITE setAutoRaise)
    Q_PROPERTY(QSize iconSize READ iconSize WRITE setIconSize)
public:
    enum Side { LeftSide, RightSide };
    QtActionItemDelegate(QObject* parent = 0);
    ~QtActionItemDelegate();

    void setToolButtonStyle(Qt::ToolButtonStyle style);
    Qt::ToolButtonStyle toolButtonStyle() const;

    void setAutoRaise(bool on = true);
    bool isAutoRaise() const;

    void setIconSize(const QSize& size);
    QSize iconSize() const;

    void addActions(const QList<QAction*>& actions, Side side = RightSide);
    void addAction(QAction* action, Side side = RightSide);
    void removeAction(QAction* action);
    QList<QAction*> actions() const;

    QWidget *createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;

    void setEditorData(QWidget * editor, const QModelIndex & index) const;
    void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const;
    void updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index) const;
private slots:
    void updateButton(QWidget* object);
    void editorDestroyed(QObject * editor);
private:
    Q_DISABLE_COPY(QtActionItemDelegate)
    Q_DECLARE_PRIVATE(QtActionItemDelegate)
    QtActionItemDelegatePrivate* d_ptr;
};

#endif
