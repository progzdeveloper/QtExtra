#ifndef DIALOG_H
#define DIALOG_H

#include <QtWidgets>
#include <QDate>
#include <QTime>
#include <QDateTime>
#include <QtPropertyWidget>

class QtContact : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id WRITE setId)
    Q_PROPERTY(QString firstName READ firstName WRITE setFirstName)
    Q_PROPERTY(QString middleName READ middleName WRITE setMiddleName)
    Q_PROPERTY(QString lastName READ lastName WRITE setLastName)
    Q_PROPERTY(int age READ age WRITE setAge)
    Q_PROPERTY(QDate birthday READ birthday WRITE setBirthday)
    Q_PROPERTY(bool married READ isMarried WRITE setMarried)
    Q_PROPERTY(QDateTime registerDate READ registerDate WRITE setRegisterDate)
public:
    QtContact(QObject* parent = Q_NULLPTR);
    ~QtContact();

    void setId(const QString& id) {
        setObjectName(id);
    }

    QString id() const {
        return objectName();
    }

    void setFirstName(const QString& firstName);
    QString firstName() const;

    void setMiddleName(const QString& middleName);
    QString middleName() const;

    void setLastName(const QString& middleName);
    QString lastName() const;

    void setAge(int age);
    int age() const;

    void setBirthday(const QDate& birthday);
    QDate birthday() const;

    void setMarried(bool on);
    bool isMarried() const;

    void setRegisterDate(const QDateTime& dateTime);
    QDateTime registerDate() const;

private:
    QString mFirstName, mMiddleName, mLastName;
    QDateTime mRegDate;
    QDate mBirthday;
    int mAge;
    bool mMarried;
};

class QtContactExtended : public QtContact
{
    Q_OBJECT
    Q_PROPERTY(double rating READ rating WRITE setRating)
public:
    QtContactExtended(QObject* parent = Q_NULLPTR);
    ~QtContactExtended();

    void setRating(double rating);
    double rating() const;
private:
    double mRating;
};


class Dialog : public QDialog
{
    Q_OBJECT
    enum FormatType
    {
        JsonFormat = 0,
        XmlFormat = 1
    };
public:
    Dialog(QWidget *parent = Q_NULLPTR);
    ~Dialog();

public slots:
    void save();
    void load();

private slots:
    void refresh(const QString&, const QVariant&);
    void refresh();
private:
    QtContact* contact;
    QtPropertyWidget* objectEditor;
    QPlainTextEdit* textView;
    QComboBox* formatSelector;
    QPushButton* openButton;
    QPushButton* saveButton;
    QPushButton* closeButton;
};

#endif // DIALOG_H
