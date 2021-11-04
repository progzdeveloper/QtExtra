#pragma once
#include <QtRangeNavigator>

class QAbstractItemView;

class QTWIDGETSEXTRA_EXPORT QtItemViewNavigator :
        public QtRangeNavigator
{
    Q_OBJECT
public:
    explicit QtItemViewNavigator(QWidget* parent = Q_NULLPTR);
    QtItemViewNavigator(QAbstractItemView *view, QWidget * parent = Q_NULLPTR);
    ~QtItemViewNavigator(void);

    void setView(QAbstractItemView *view);
    QAbstractItemView *view() const;

    /*void setIconSize(const QSize& size);
    QSize iconSize() const;*/
private Q_SLOTS:
    void moveTo(int row);
    void setIndex(const QModelIndex& index);
    void updateRange();
private:
    QT_PIMPL(QtItemViewNavigator)
};

