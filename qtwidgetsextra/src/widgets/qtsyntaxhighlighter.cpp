#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>
#include <QColor>
#include <QRegExp>
#include <QDebug>
#include "qtsyntaxhighlighter.h"

struct SyntaxRule
{
    QTextCharFormat format;
    int priority;
    SyntaxRule() : priority(0) {}
};

inline bool operator<(const SyntaxRule& lhs, const SyntaxRule& rhs) {
    return (lhs.priority < rhs.priority);
}

struct SingleLineSyntaxRule :
        public SyntaxRule
{
    QRegExp pattern;
};

struct MultiLineSyntaxRule:
        public SyntaxRule
{
    QRegExp patternFirst, patternLast;
};



class QtSyntaxHighlighterPrivate
{
public:
    QString syntax;
    QVector<SingleLineSyntaxRule> slRules;
    QVector<MultiLineSyntaxRule>  mlRules;

    void setupPattern(const QString& pattern, QRegExp& regExp);
    void readRule(const QJsonObject& jsRule, SingleLineSyntaxRule& rule);
    void readRule(const QJsonObject& jsRule, MultiLineSyntaxRule& rule);

    void readFormat(const QJsonObject& jsFormat, QTextCharFormat& fmt);
    void readFont(const QJsonObject& jsFont, QTextCharFormat& font);
    void readStyle(const QJsonObject& jsStyle, QTextCharFormat& fmt);
};


void QtSyntaxHighlighterPrivate::setupPattern(const QString &pattern, QRegExp &regExp)
{
    regExp.setPatternSyntax(QRegExp::RegExp2);
    if (pattern.startsWith("(?i)")) {
        QString s = pattern.mid(4);
        regExp.setPattern(s);
        regExp.setCaseSensitivity(Qt::CaseInsensitive);
    } else {
        regExp.setPattern(pattern);
    }
}

void QtSyntaxHighlighterPrivate::readRule(const QJsonObject &jsRule, SingleLineSyntaxRule &rule)
{
    rule.priority = jsRule["priority"].toDouble();
    setupPattern(jsRule["pattern"].toString(), rule.pattern);
    readFormat(jsRule["format"].toObject(), rule.format);
}

void QtSyntaxHighlighterPrivate::readRule(const QJsonObject &jsRule, MultiLineSyntaxRule &rule)
{
    rule.priority = jsRule["priority"].toDouble();

    QJsonObject jsPattern = jsRule["pattern"].toObject();
    setupPattern(jsPattern["first"].toString(), rule.patternFirst);
    setupPattern(jsPattern["last"].toString(), rule.patternLast);

    readFormat(jsRule["format"].toObject(), rule.format);
}

void QtSyntaxHighlighterPrivate::readFormat(const QJsonObject &jsFormat, QTextCharFormat &fmt)
{
    fmt.setToolTip(jsFormat["tooltip"].toString());
    readFont(jsFormat["font"].toObject(), fmt);
    readStyle(jsFormat["style"].toObject(), fmt);
}

void QtSyntaxHighlighterPrivate::readFont(const QJsonObject &jsFont, QTextCharFormat &fmt)
{
    double fontSize = jsFont["size"].toDouble();
    QString fontFamily = jsFont["family"].toString();

    if (!fontFamily.isEmpty())
        fmt.setFontFamily(fontFamily);

    if (fontSize != 0)
        fmt.setFontPointSize(fontSize);

    fmt.setFontWeight(jsFont["bold"].toBool() ? QFont::Bold : QFont::Normal);
    fmt.setFontItalic(jsFont["italic"].toBool());
    fmt.setFontUnderline(jsFont["underline"].toBool());
    fmt.setFontOverline(jsFont["overline"].toBool());
    fmt.setFontStrikeOut(jsFont["strikeout"].toBool());
}

void QtSyntaxHighlighterPrivate::readStyle(const QJsonObject &jsStyle, QTextCharFormat &fmt)
{
    fmt.setBackground(QColor(jsStyle["background"].toString()));
    fmt.setForeground(QColor(jsStyle["foreground"].toString()));
    fmt.setUnderlineColor(QColor(jsStyle["linecolor"].toString()));

    QTextCharFormat::UnderlineStyle style;
    QString styleStr = jsStyle["linestyle"].toString();

    if (styleStr == "single")            style = QTextCharFormat::SingleUnderline;
    else if (styleStr == "dash")         style = QTextCharFormat::DashUnderline;
    else if (styleStr == "dot")          style = QTextCharFormat::DotLine;
    else if (styleStr == "dash-dot")     style = QTextCharFormat::DashDotLine;
    else if (styleStr == "dash-dot-dot") style = QTextCharFormat::DashDotDotLine;
    else if (styleStr == "wave")         style = QTextCharFormat::WaveUnderline;
    else                                 style = QTextCharFormat::NoUnderline;

    fmt.setUnderlineStyle(style);
}




QtSyntaxHighlighter::QtSyntaxHighlighter(QObject *parent) :
    QSyntaxHighlighter(parent),
    d_ptr(new QtSyntaxHighlighterPrivate)
{
}

QtSyntaxHighlighter::QtSyntaxHighlighter(QTextDocument *parent) :
    QSyntaxHighlighter(parent),
    d_ptr(new QtSyntaxHighlighterPrivate)
{

}

QtSyntaxHighlighter::~QtSyntaxHighlighter()
{
}

void QtSyntaxHighlighter::load(const QJsonObject &json)
{
    Q_D(QtSyntaxHighlighter);
    d->mlRules.clear();
    d->slRules.clear();
    d->syntax = json["syntax"].toString();
    QJsonArray jsRules = json["rules"].toArray();
    QJsonObject jsRule;
    for (int i = 0; i < jsRules.size(); i++) {
         jsRule = jsRules[i].toObject();
         if (jsRule["pattern"].isString()) {
             SingleLineSyntaxRule rule;
             d->readRule(jsRule, rule);
             d->slRules << rule;
         } else {
             MultiLineSyntaxRule rule;
             d->readRule(jsRule, rule);
             d->mlRules << rule;
         }
    }
    std::sort(d->slRules.begin(), d->slRules.end());
    std::sort(d->mlRules.begin(), d->mlRules.end());
    rehighlight();
}

void QtSyntaxHighlighter::load(const QJsonDocument &json)
{
    load(json.object());
}

void QtSyntaxHighlighter::load(QIODevice *device, QJsonParseError *error)
{
    if (device) {
        load(device->readAll(), error);
    }
}

void QtSyntaxHighlighter::load(const QByteArray &bytes, QJsonParseError *error)
{
    load(QJsonDocument::fromJson(bytes, error));
}

QString QtSyntaxHighlighter::syntax() const
{
    Q_D(const QtSyntaxHighlighter);
    return d->syntax;
}

void QtSyntaxHighlighter::highlightBlock(const QString &text)
{
    Q_D(QtSyntaxHighlighter);

    for (const SingleLineSyntaxRule &rule : d->slRules)
    {
        int index = rule.pattern.indexIn(text);
        while (index >= 0) {
            int length = rule.pattern.matchedLength();
            setFormat(index, length, rule.format);
            index = rule.pattern.indexIn(text, index + length);
        }
    }
    setCurrentBlockState(0);

    for (const MultiLineSyntaxRule &rule : d->mlRules)
    {
        int startIndex = 0;
        if (previousBlockState() != 1)
            startIndex = rule.patternFirst.indexIn(text);

        while (startIndex >= 0)
        {
            int endIndex = rule.patternLast.indexIn(text, startIndex);
            int length;
            if (endIndex == -1) {
                setCurrentBlockState(1);
                length = text.length() - startIndex;
            } else {
                length = endIndex - startIndex + rule.patternLast.matchedLength();
            }
            setFormat(startIndex, length, rule.format);
            startIndex = rule.patternFirst.indexIn(text, startIndex + length);
        }
    }
}
