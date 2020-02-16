#ifndef QTCODEWINDOW_H
#define QTCODEWINDOW_H

#include <QWidget>

class QPlainTextEdit;
class QtOverviewWidget;


class QtCodeWindow : public QWidget
{
    Q_OBJECT
public:
    explicit QtCodeWindow(QWidget *parent = nullptr);
    ~QtCodeWindow();

    bool load();

private:
    QPlainTextEdit* edit;
    QtOverviewWidget* overview;
};

#endif // QTCODEWINDOW_H
