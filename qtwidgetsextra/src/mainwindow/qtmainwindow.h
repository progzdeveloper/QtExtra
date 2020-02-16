#pragma once
#include <QMainWindow>
#include <QByteArray>

#include <QtWidgetsExtra>

class QContextMenuEvent;
class QtDockWidget;
class QMenu;

class QtMainWindowPrivate;

class QTWIDGETSEXTRA_EXPORT QtMainWindow :
        public QMainWindow
{
    Q_OBJECT
public:
    explicit QtMainWindow(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~QtMainWindow();

    void addDockWidget(Qt::DockWidgetArea area, QtDockWidget * dockwidget);
    void addDockWidget(Qt::DockWidgetArea area, QtDockWidget * dockwidget, Qt::Orientation orientation);

private slots:
    void changeDockLocation(Qt::DockWidgetArea area);
    void changeDockState(bool on);
    void focusChanged(QWidget * old, QWidget * now );

protected:
    virtual QMenu *createContextMenu();
    void contextMenuEvent(QContextMenuEvent *e);

    QByteArray saveDockState() const;
    void restoreDockState(const QByteArray& bytes);

private:
    QT_PIMPL(QtMainWindow)
};

