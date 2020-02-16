#pragma once
#include <QtWidgets/QDialog>
class QtRangeNavigator;
class QConfigWizard : 
        public QDialog
{
    Q_OBJECT
public:
    QConfigWizard(QWidget* parent = Q_NULLPTR);
public slots:
    void changePage(int page);
private:
    QtRangeNavigator* navigator;
};
