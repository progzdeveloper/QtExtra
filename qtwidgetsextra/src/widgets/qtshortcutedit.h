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
    explicit QtShortcutEdit(QWidget *parent = Q_NULLPTR);

    QKeySequence keySequence() const;

public Q_SLOTS:
    void setKeySequence(const QKeySequence &sequence);

Q_SIGNALS:
    void keySequenceChanged(const QKeySequence &sequence);

protected:
    bool eventFilter(QObject *o, QEvent *e) Q_DECL_OVERRIDE;
    void focusInEvent(QFocusEvent *e) Q_DECL_OVERRIDE;
    void focusOutEvent(QFocusEvent *e) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
    void keyReleaseEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    bool event(QEvent *e) Q_DECL_OVERRIDE;

private Q_SLOTS:
    void slotClearShortcut();

private:
    void handleKeyEvent(QKeyEvent *e);
    int translateModifiers(Qt::KeyboardModifiers state, const QString &text) const;

private:
    int m_num;
    QKeySequence m_keySequence;
    QLineEdit *m_lineEdit;
};

#endif // QTSHORTCUTEDIT_H

