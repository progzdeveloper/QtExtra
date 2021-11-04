#ifndef QTPATTERNEDIT_H
#define QTPATTERNEDIT_H

#include <QtWidgets/QLineEdit>
#include <QtWidgetsExtra>

class QTWIDGETSEXTRA_EXPORT QtPatternEdit :
        public QLineEdit
{
    Q_OBJECT
    Q_PROPERTY(Options options READ options WRITE setOptions NOTIFY optionsChanged)

public:
    enum Option
    {
        NoOption      = 0x00,
        CaseSensitive = 0x01,
        WholeWords    = 0x02,
        RegularExpr   = 0x04
    };
    Q_DECLARE_FLAGS(Options, Option)

    explicit QtPatternEdit(QWidget *parent = Q_NULLPTR);
    QtPatternEdit(const QString &pattern, QWidget *parent = Q_NULLPTR);
    ~QtPatternEdit();

    void setOptions(Options opts);
    Options options() const;

private Q_SLOTS:
    void showOptions();
    void toggleOption(bool on);

Q_SIGNALS:
    void optionsChanged(Options);

private:
    QT_PIMPL(QtPatternEdit)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QtPatternEdit::Options)

#endif // QTPATTERNEDIT_H
