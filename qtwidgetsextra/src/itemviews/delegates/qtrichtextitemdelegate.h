#ifndef QTRICHTEXTDELEGATE_H
#define QTRICHTEXTDELEGATE_H

#include <QtWidgetsExtra>
#include <QStyledItemDelegate>

class QHeaderView;

class QTWIDGETSEXTRA_EXPORT QtRichTextItemDelegate :
        public QStyledItemDelegate
{
    Q_OBJECT
    Q_PROPERTY(Qt::TextFormat textFormat READ textFormat WRITE setTextFormat NOTIFY textFormatChanged)
public:
    explicit QtRichTextItemDelegate(QObject* parent = Q_NULLPTR);
    virtual ~QtRichTextItemDelegate();

    void setTextFormat(Qt::TextFormat format);
    Qt::TextFormat textFormat() const;

    // QAbstractItemDelegate interface
public:
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;


    // QAbstractItemDelegate interface
public:
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const Q_DECL_OVERRIDE;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const Q_DECL_OVERRIDE;

Q_SIGNALS:
    void textFormatChanged(Qt::TextFormat);

private:
    QScopedPointer<class QtRichTextItemDelegatePrivate> d_ptr;
    Q_DECLARE_PRIVATE(QtRichTextItemDelegate)
};

#endif // QTRICHTEXTDELEGATE_H
