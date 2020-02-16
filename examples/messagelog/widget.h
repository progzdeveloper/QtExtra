#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QFile>

class QPushButton;

class QtMessageLogView;
class QtMessageLogWidget;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();

private Q_SLOTS:
    void createMessage();
    void toggleTimer(bool on);
private:
    QtMessageLogWidget* widget;
    QtMessageLogView* view;
    QPushButton* generateButton;
    QPushButton* clearButton;
    QTimer* timer;
    QFile file;
};




#endif // WIDGET_H
