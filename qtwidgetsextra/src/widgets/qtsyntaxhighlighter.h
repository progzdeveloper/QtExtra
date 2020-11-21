#ifndef QTSYNTAXHIGHLIGHTER_H
#define QTSYNTAXHIGHLIGHTER_H
#include <QJsonDocument>
#include <QJsonParseError>
#include <QSyntaxHighlighter>

#include <QtWidgetsExtra>

class QTWIDGETSEXTRA_EXPORT QtSyntaxHighlighter :
        public QSyntaxHighlighter
{
    Q_OBJECT
    Q_DISABLE_COPY(QtSyntaxHighlighter)
public:
    explicit QtSyntaxHighlighter(QObject* parent = Q_NULLPTR);
    explicit QtSyntaxHighlighter(QTextDocument *parent);
    ~QtSyntaxHighlighter();

    void load(const QJsonObject& json);
    void load(const QJsonDocument& json);
    void load(QIODevice *device, QJsonParseError* error = Q_NULLPTR);
    void load(const QByteArray& bytes, QJsonParseError* error = Q_NULLPTR);

    QString syntax() const;

    // QSyntaxHighlighter interface
protected:
    void highlightBlock(const QString &text) Q_DECL_OVERRIDE;

private:
    QT_PIMPL(QtSyntaxHighlighter)
};

#endif // QTSYNTAXHIGHLIGHTER_H
