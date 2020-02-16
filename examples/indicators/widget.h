#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

class QAbstractButton;
class QtPropertyWidget;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();
public Q_SLOTS:
    void switchLight();
private:
    QtPropertyWidget* browser;
    QAbstractButton* indicator;
};

#endif // WIDGET_H
