#pragma once
#include <QFrame>

#include <QtWidgetsExtra>

class QMainWindow;

class QListWidgetItem;

class QtWindowManagerPrivate;
class QTWIDGETSEXTRA_EXPORT QtWindowManager :
        public QFrame
{
    Q_OBJECT

public:
    QtWindowManager(QWidget* parent = 0, Qt::WindowFlags flags = Qt::ToolTip);
    ~QtWindowManager();

    void setMainWindow(QMainWindow* w);
    QMainWindow* mainWindow() const;

    void setHighlightEnabled(bool on);
    bool isHighlightEnaled() const;

protected:
    void paintEvent(QPaintEvent* e);
    void showEvent(QShowEvent* e);
    void hideEvent(QHideEvent* e);
    bool eventFilter(QObject* object, QEvent* e);

private:
    bool filterKeyEvent(QObject* object, QKeyEvent* e);
    bool execAction(int key);

private slots:
    void itemClicked(QListWidgetItem*);

private:
    Q_DISABLE_COPY(QtWindowManager)
    Q_DECLARE_PRIVATE(QtWindowManager)
    QtWindowManagerPrivate* d_ptr;
};
