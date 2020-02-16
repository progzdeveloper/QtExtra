#ifndef QTSHORTCUTEDIT_H
#define QTSHORTCUTEDIT_H

#include <QWidget>
#include <QKeySequence>

#include <QtWidgetsExtra>

class QEvent;
class QLineEdit;

class QTWIDGETSEXTRA_EXPORT QtShortcutEdit :
        public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QKeySequence shortcut READ keySequence WRITE setKeySequence)
    Q_DISABLE_COPY(QtShortcutEdit)
public:
    QtShortcutEdit(QWidget *parent = 0);

    QKeySequence keySequence() const;
    bool eventFilter(QObject *o, QEvent *e);

public Q_SLOTS:
    void setKeySequence(const QKeySequence &sequence);

Q_SIGNALS:
    void keySequenceChanged(const QKeySequence &sequence);

protected:
    void focusInEvent(QFocusEvent *e);
    void focusOutEvent(QFocusEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
    void paintEvent(QPaintEvent *);
    bool event(QEvent *e);

private Q_SLOTS:
    void slotClearShortcut();

private:
    void handleKeyEvent(QKeyEvent *e);
    int translateModifiers(Qt::KeyboardModifiers state, const QString &text) const;

    int m_num;
    QKeySequence m_keySequence;
    QLineEdit *m_lineEdit;
};

#endif // QTSHORTCUTEDIT_H

