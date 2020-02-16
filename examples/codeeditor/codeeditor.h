#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QMainWindow>

class QtAboutDialog;
class QtNotificationBar;
class QtSyntaxHighlighter;

class QtPatternEdit;

class JSEdit;

class CodeEditor :
        public QMainWindow
{
    Q_OBJECT

public:
    CodeEditor(QWidget *parent = 0);
    ~CodeEditor();

public Q_SLOTS:
    void open();
    void save();
    void setSyntax();
    void about();
private:
    void createMenus();
    void createActions();

private:
    QHash<QString, QString> syntaxFiles;
    JSEdit* codeEdit;
    QtSyntaxHighlighter* highlighter;
    QMenu* fileMenu;
    QMenu* syntaxMenu;
    QMenu* helpMenu;
    QToolBar* toolBar;
    QtNotificationBar* notificationBar;
    QtAboutDialog* aboutDialog;
    QtPatternEdit* patternEdit;
    // QObject interface
public:
    virtual bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;
};

#endif // CODEEDITOR_H
