#pragma once
#include <QPlainTextEdit>

#include <QtWidgetsExtra>

class QTWIDGETSEXTRA_EXPORT QtContentView :
        public QPlainTextEdit
{
    Q_OBJECT
    Q_DISABLE_COPY(QtContentView)
    friend class LineNumberArea;

public:
    explicit QtContentView(QWidget *parent = Q_NULLPTR);
    ~QtContentView(void);

    bool lineNumbersShown() const;

public Q_SLOTS:
    void showLineNumbers(bool on);

private Q_SLOTS:
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect&, int);
    void highlightCurrentLine();

protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private:
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int  lineNumberAreaWidth();
    QWidget *lineNumberArea;
};
