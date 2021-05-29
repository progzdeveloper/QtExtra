#ifndef QTFLOATINGTOOLBUTTON_H
#define QTFLOATINGTOOLBUTTON_H

#include <QtWidgetsExtra>
#include <QAbstractButton>

class QMenu;
class QAction;
class QTWIDGETSEXTRA_EXPORT QtFloatingToolButton :
    public QAbstractButton
{
    Q_OBJECT
public:
    explicit QtFloatingToolButton(QWidget* parent = Q_NULLPTR);
    ~QtFloatingToolButton();

    void setAutoRaise(bool on);
    bool isAutoRaise() const;

    void setExpanded(bool on);
    bool isExpanded() const;

    void setAnimated(bool on);
    bool isAnimated() const;

    void setAlignment(Qt::Alignment align);
    Qt::Alignment alignment() const;

    void setToolButtonStyle(Qt::ToolButtonStyle style);
    Qt::ToolButtonStyle toolButtonStyle() const;

    void setMargins(const QMargins& margins);
    QMargins margins() const;

    void setDefaultAction(QAction* action);
    QAction* defaultAction() const;

    void setMenu(QMenu* menu);
    QMenu* menu() const;

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;

public Q_SLOTS:
    void showMenu();

Q_SIGNALS:
    void triggered(QAction *action);

protected:
    bool hitButton(const QPoint &pos) const Q_DECL_OVERRIDE;

protected:
    void changeEvent(QEvent *event) Q_DECL_OVERRIDE;
    bool event(QEvent* event) Q_DECL_OVERRIDE;
    void actionEvent(QActionEvent* event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
    void actionTriggered();

private:
    QT_PIMPL(QtFloatingToolButton)
};

#endif // QTFLOATINGTOOLBUTTON_H
