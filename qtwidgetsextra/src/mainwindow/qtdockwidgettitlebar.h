#pragma once
#include <QDockWidget>

class QPaintEvent;

class QtDockWidgetTitleBarPrivate;

class QtDockWidgetTitleBar : 
        public QWidget
{
    Q_OBJECT
public:
    explicit QtDockWidgetTitleBar(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~QtDockWidgetTitleBar();
public slots:
    void closeWidget();
    void autoHide();
    void undock();
private slots:
    void updateButtons(bool autoHide);
    void clicked(int button);
    void changeFeatures(QDockWidget::DockWidgetFeatures features);

protected:
    void paintEvent(QPaintEvent *);

private:
    Q_DISABLE_COPY(QtDockWidgetTitleBar)
    Q_DECLARE_PRIVATE(QtDockWidgetTitleBar)
    QtDockWidgetTitleBarPrivate *d_ptr;
};

