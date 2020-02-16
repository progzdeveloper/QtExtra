#ifndef PROGRESSWIDGET_H
#define PROGRESSWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QJsonObject>

class ProgressWidget : public QWidget
{
    Q_OBJECT

public:
    ProgressWidget(QWidget *parent = 0);
    ~ProgressWidget();

public Q_SLOTS:
    void execute();

private:
    QJsonObject json;
    QPushButton* execButton;
};

#endif // PROGRESSWIDGET_H
