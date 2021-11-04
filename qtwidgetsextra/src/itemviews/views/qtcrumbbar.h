#ifndef QTCRUMBVIEW_H
#define QTCRUMBVIEW_H

#include <QModelIndex>
#include <QFrame>

#include <QtWidgetsExtra>

class QCompleter;
class QAbstractItemModel;

class QTWIDGETSEXTRA_EXPORT QtCrumbBar :
        public QFrame
{
    Q_OBJECT
public:
    explicit QtCrumbBar(QWidget* parent = Q_NULLPTR);
    virtual ~QtCrumbBar();

    virtual void setModel(QAbstractItemModel* model);
    QAbstractItemModel* model() const;

    QModelIndexList indexList() const;

    void setCompleter(QCompleter* completer);
    QCompleter* completer() const;

    void setRootIcon(const QIcon& icon);
    QIcon rootIcon() const;

public Q_SLOTS:
    void reset();
    void back();
    void setCurrentIndex(const QModelIndex& index);

private Q_SLOTS:
    void crumbClicked();
    void closeEditor();

Q_SIGNALS:
    void indexChanged(const QModelIndex& index);

    // QObject interface
protected:
    virtual bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;

private:
    QT_PIMPL(QtCrumbBar)
};

#endif // QTCRUMBVIEW_H
