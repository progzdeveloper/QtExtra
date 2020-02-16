#ifndef DEMOWIDGET_H
#define DEMOWIDGET_H

#include <QWidget>

class QFormLayout;
class QVBoxLayout;
class DemoWidget : public QWidget
{
    Q_OBJECT

public:
    DemoWidget(QWidget *parent = 0);
    ~DemoWidget();

    void insertWidget(QWidget* w);
private:
    QVBoxLayout* layout;
};

#endif // DEMOWIDGET_H
