#include "codeeditor.h"
#include "jsedit.h"
#include <QtWidgets>
#include <QtNotificationBar>
#include <QtSyntaxHighlighter>
#include <QtAboutDialog>
#include <QtPatternEdit>

CodeEditor::CodeEditor(QWidget *parent)
    : QMainWindow(parent), aboutDialog(Q_NULLPTR)
{
    syntaxFiles.insert("SQL", "sql.json");
    syntaxFiles.insert("XML", "xml.json");
    syntaxFiles.insert("JSON", "json.json");
    syntaxFiles.insert("CSON", "cson.json");
    syntaxFiles.insert("C/C++", "cpp.json");


    codeEdit = new JSEdit(this);
    codeEdit->setMouseTracking(true);
    codeEdit->installEventFilter(this);
    //codeEdit->setFontFamily("Courier New");
    highlighter = new QtSyntaxHighlighter(codeEdit->document());
    notificationBar = new QtNotificationBar(codeEdit);

    QWidget* centralWidget = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);
    layout->addWidget(notificationBar);
    layout->addWidget(codeEdit);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    setCentralWidget(centralWidget);

    patternEdit = new QtPatternEdit(this);


    setIconSize(QSize(16, 16));
    createMenus();
    createActions();
}

CodeEditor::~CodeEditor() {
}

void CodeEditor::createMenus()
{
    menuBar()->setNativeMenuBar(false);
    fileMenu = menuBar()->addMenu(tr("File"));
    syntaxMenu = menuBar()->addMenu(tr("Syntax"));
    helpMenu = menuBar()->addMenu(tr("Help"));
    toolBar = addToolBar(tr("Standard"));
}

void CodeEditor::createActions()
{
    fileMenu->addAction(QIcon::fromTheme("document-open"), tr("Open"), this, SLOT(open()), QKeySequence::Open);
    fileMenu->addAction(QIcon::fromTheme("document-save"), tr("Save"), this, SLOT(save()), QKeySequence::Save);
    fileMenu->addSeparator();
    fileMenu->addAction(QIcon::fromTheme("application-exit"), tr("Exit"), this, SLOT(close()), QKeySequence::Quit);

    for (auto it = syntaxFiles.begin(); it != syntaxFiles.end(); ++it) {
        syntaxMenu->addAction(it.key(), this, SLOT(setSyntax()));
    }

    helpMenu->addAction(QIcon::fromTheme("application-about"), tr("About"), this, SLOT(about()));

    toolBar->addActions(fileMenu->actions());
    toolBar->addSeparator();
    toolBar->addAction(syntaxMenu->menuAction());
    toolBar->addSeparator();
    toolBar->addWidget(patternEdit);
}



void CodeEditor::open()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"));
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly|QFile::Text)) {
        QMessageBox::warning(this, tr("File Error"),
                             tr("Failed to open file '%1': %2").arg(fileName, file.errorString()));
        notificationBar->showMessage(tr("Failed to open file '%1': %2").arg(fileName, file.errorString()),
                                     QIcon::fromTheme("emblem-important"));
        return;
    }
    codeEdit->setPlainText(file.readAll());
    notificationBar->showMessage(tr("File '%1' was successfully loaded").arg(fileName),
                                 QIcon::fromTheme("emblem-default"));
}

void CodeEditor::save()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"));
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly|QFile::Text)) {
        QMessageBox::warning(this, tr("File Error"),
                             tr("Failed to save file '%1': %2").arg(fileName, file.errorString()));
        notificationBar->showMessage(tr("Failed to save file '%1': %2").arg(fileName, file.errorString()),
                                     QIcon::fromTheme("emblem-important"));
        return;
    }
    file.write(codeEdit->toPlainText().toLocal8Bit());
    notificationBar->showMessage(tr("File '%1' was successfully saved").arg(fileName),
                                 QIcon::fromTheme("emblem-default"));
}

void CodeEditor::setSyntax()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (!action)
        return;

    QString syntax = action->text();
    auto it = syntaxFiles.find(syntax);
    if (it == syntaxFiles.end())
        return;

    QFile file(it.value());
    if (!file.open(QFile::ReadOnly|QFile::Text)) {
        QMessageBox::warning(this, tr("Syntax highlighting error"),
                             tr("Failed to load highlighting rules for %1 syntax from file '%2': %3")
                             .arg(syntax, it.value(), file.errorString()));
        return;
    }

    QJsonParseError parseError;
    highlighter->load(&file, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        QMessageBox::warning(this, tr("Syntax highlighting error"),
                             tr("Failed to load highlighting rules for '%1' syntax from file '%2': %3")
                             .arg(syntax, it.value(), parseError.errorString()));
        return;
    }
}

void CodeEditor::about()
{
    if (aboutDialog == Q_NULLPTR)
    {
        QString systemInfo;
        systemInfo += "Arch: ";
        systemInfo += QSysInfo::currentCpuArchitecture();
        systemInfo += "\\n";
        systemInfo += "Platform: ";
        systemInfo += QSysInfo::prettyProductName();
        systemInfo += "\\n";

        aboutDialog = new QtAboutDialog(this);
        QFile file(":/application.json");
        file.open(QFile::ReadOnly);
        QString json = file.readAll();
        json.replace("%SystemInfo%", systemInfo);
        aboutDialog->setJson(QJsonDocument::fromJson(json.toUtf8()));
    }
    aboutDialog->exec();
}



bool CodeEditor::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == codeEdit && event->type() == QEvent::ToolTip)
    {
        QHelpEvent* helpEvent = static_cast<QHelpEvent*>(event);
        QTextCursor cursor = codeEdit->cursorForPosition(helpEvent->pos());

        QString text;
        int pos = cursor.positionInBlock();
        for (const QTextLayout::FormatRange& r : cursor.block().layout()->formats()) {
            if (pos >= r.start && pos <= r.start + r.length) {
                text = r.format.toolTip();
                break;
            }
        }

        if (!text.isEmpty())
           QToolTip::showText(helpEvent->globalPos(), text);
        else
           QToolTip::hideText();
    }
    return QObject::eventFilter(watched, event);
}

