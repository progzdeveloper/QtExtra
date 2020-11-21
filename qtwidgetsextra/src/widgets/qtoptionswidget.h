#pragma once
#include <QWidget>

#include <QtWidgetsExtra>

class QWidget;
class QListWidgetItem;


class QTWIDGETSEXTRA_EXPORT QtOptionsWidget :
        public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(QtOptionsWidget)
public:
    explicit QtOptionsWidget(QWidget *parent = Q_NULLPTR);
    ~QtOptionsWidget();

    int addWidget ( QWidget * widget );
    int count () const;
    int currentIndex () const;
    QWidget * currentWidget () const;
    int indexOf ( QWidget * widget ) const;
    QWidget * widget ( int index ) const;

public Q_SLOTS:
    void setCurrentIndex ( int index );
    void setCurrentWidget ( QWidget * widget );

private Q_SLOTS:
    void changePage(QListWidgetItem *current,
                    QListWidgetItem *previous);

Q_SIGNALS:
    void currentChanged ( int index );
    void widgetRemoved ( int index );

private:
    QT_PIMPL(QtOptionsWidget)
};


