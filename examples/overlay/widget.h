#ifndef WIDGET_H
#define WIDGET_H

#include <QPointer>
#include <QGraphicsEffect>
#include <QWidget>
#include <QTableWidget>
#include <QPushButton>

class QTimer;
class QPropertyAnimation;

class QtProgressEffect;
class QtPlaceholderEffect;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();

public Q_SLOTS:
    void insertRow();
    void removeRow();

    void process();
    void cancel();

private Q_SLOTS:
    void toggleAnimation(bool on);
    void openLink(const QString& url);
private:
    QTableWidget* view;
    QPushButton* addRowButton;
    QPushButton* delRowButton;
    QPushButton* processButton;
    QPushButton* cancelButton;
    QPropertyAnimation* animation;
    QtProgressEffect* progressEffect;
    QTimer* timer;
};






#endif // WIDGET_H
