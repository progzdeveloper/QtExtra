#pragma once
#include <QtWidgets/QDialog>

class QListWidget;
class QListWidgetItem;
class QStackedWidget;

// ###
class QtSlideController;

class QConfigWidget : public QDialog
{
    Q_OBJECT

public:
    QConfigWidget(QWidget* parent = Q_NULLPTR);

public slots:
    void changePage(QListWidgetItem *current, QListWidgetItem *previous);

private:
    void createIcons();

    QListWidget *contentsWidget;
    QStackedWidget *pagesWidget;
    QtSlideController* controller;
};

