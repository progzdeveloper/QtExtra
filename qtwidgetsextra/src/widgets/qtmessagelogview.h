#ifndef QTMESSAGELOGVIEW_H
#define QTMESSAGELOGVIEW_H

#include <QRegularExpression>
#include <QWidget>
#include <QtWidgetsExtra>


class QAbstractItemModel;
class QTableView;


class QTWIDGETSEXTRA_EXPORT QtMessageLogView : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(Fields visibleFields READ visibleFields WRITE setVisibleFields NOTIFY visibleFieldsChanged)
    Q_PROPERTY(int rotationLimit READ rotationLimit WRITE setRotationLimit)
    Q_PROPERTY(QRegularExpression expression READ regExp WRITE setRegExp)
    Q_PROPERTY(QString categoryFilter READ categoryFilter WRITE setCategoryFilter)
    Q_PROPERTY(QString messageFilter READ messageFilter WRITE setMessageFilter)

public:
    enum Field
    {
        None      = 0x0,
        Timestamp = 0x1,
        Level     = 0x2,
        Code      = 0x4,
        Category  = 0x8,
        Message   = 0x10,
        FieldMask = Level|Code|Timestamp|Category|Message
    };
    Q_DECLARE_FLAGS(Fields, Field)
    Q_FLAG(Fields)

    explicit QtMessageLogView(QWidget *parent = Q_NULLPTR);
    ~QtMessageLogView();

    QAbstractItemModel* model() const;
    QTableView* view() const;

    QRegularExpression regExp() const;
    uint rotationLimit() const;
    Fields visibleFields() const;

    void setDefaultValue(Field field, const QVariant& value);
    QVariant defaultValue(Field field) const;

    void setFieldFilter(Field f, const QVariant& value, Qt::MatchFlags flags = Qt::MatchExactly);
    QPair<QVariant, Qt::MatchFlags> fieldFilter(Field f) const;
    QString categoryFilter() const;
    QString messageFilter() const;

public Q_SLOTS:
    void sort();

    void setRegExp(const QRegularExpression& re);
    void setRotationLimit(uint limit);
    void setVisibleFields(Fields f);

    void setCategoryFilter(const QString& pattern, Qt::MatchFlags flags = Qt::MatchContains);
    void setMessageFilter(const QString& pattern, Qt::MatchFlags flags = Qt::MatchContains);
    void clearFilters(Fields fields);

    void clear();

    void message(const QDateTime& timestamp, int level, int code, const QString& category, const QString& text);

    void message(int level, int code, const QString& category, const QString& text);

    void message(int level, int code, const QString &text);

    void message(int level, const QString &text);

    void message(const QString &category, const QString &text);

    void message(const QString &text);

    virtual void feed(const QString& line);

protected:
    virtual void interpret(const QRegularExpressionMatch& match);
    virtual QDateTime timestamp(const QRegularExpressionMatch& match) const;
    virtual int level(const QRegularExpressionMatch& match) const;
    virtual int code(const QRegularExpressionMatch& match) const;
    virtual QString category(const QRegularExpressionMatch& match) const;
    virtual QString text(const QRegularExpressionMatch& match) const;


Q_SIGNALS:
    void visibleFieldsChanged(Fields);

private:
    QT_PIMPL(QtMessageLogView)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QtMessageLogView::Fields)

#endif // QTMESSAGELOGVIEW_H
