#ifndef DIALOG_H
#define DIALOG_H

#include <QUrl>
#include <QListWidget>
#include <QDialog>
#include <QDateTime>

class QDialogButtonBox;
class QAbstractButton;
class QLineEdit;

class QtPropertyWidget;

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = 0);
    ~Dialog();

protected:
    void resizeEvent(QResizeEvent* );

private slots:
    void buttonClicked(QAbstractButton* button);

private:
    QtPropertyWidget* browser;
    QtPropertyWidget* editor;
    QDialogButtonBox* buttonBox;
};

class ListWidget : public QListWidget
{
    Q_OBJECT
    Q_CLASSINFO("url", "URL Link")
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)

    Q_CLASSINFO("filter", "Name Filter")
    Q_PROPERTY(QString filter READ filter WRITE setFilter NOTIFY filterChanged)

public:
    ListWidget(QWidget* parent = 0);
    QString filter() const;
    QUrl url() const;

public Q_SLOTS:
    void setFilter(const QString &filter);
    void setUrl(const QUrl& url);

Q_SIGNALS:
    void urlChanged(const QUrl&);
    void filterChanged(const QString& filter);

private:
    QString m_filter;
    QUrl m_url;
};

struct QMetaObject;
class QtGadget
{
    Q_GADGET
    Q_PROPERTY(QString id READ identifier WRITE setIdentifier)

public:
    QtGadget();
    virtual ~QtGadget();

    void setIdentifier(const QString& name);
    QString identifier() const;

    virtual const QMetaObject* metaObject() const {
        return &QtGadget::staticMetaObject;
    }

private:
    QString mId;
};



class QContact :
        public QtGadget
{
    Q_GADGET

    Q_PROPERTY(QString firstName READ firstName WRITE setFirstName)
    Q_PROPERTY(QString lastName READ lastName WRITE setLastName)
    Q_PROPERTY(int age READ age WRITE setAge)
    Q_PROPERTY(qreal rating READ rating WRITE setRating)
    Q_PROPERTY(QDateTime birthday READ birthday WRITE setBirthday)
    Q_PROPERTY(bool married READ isMarried WRITE setMarried)

public:
    QContact();
    virtual ~QContact();

    const QMetaObject* metaObject() const {
        return &QContact::staticMetaObject;
    }

    void setFirstName(const QString& firstName);
    QString firstName() const;

    void setLastName(const QString& lastName);
    QString lastName() const;

    void setAge(int age);
    int age() const;

    void setBirthday(const QDateTime& dt);
    QDateTime birthday() const;

    void setRating(qreal rating);
    qreal rating() const;

    void setMarried(bool on);
    bool isMarried() const;

    void debugPrint() const;

private:
    QDateTime mBirthday;
    QString mFirstName;
    QString mLastName;
    qreal mRating;
    int mAge;
    bool mMarried;
};


#endif // DIALOG_H
