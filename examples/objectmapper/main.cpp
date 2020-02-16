#include "dialog.h"
#include <QApplication>
#include <QSqlRecord>
#include <QSqlField>
#include <QPoint>
#include <QDebug>

template<class T>
struct QtSqlMapper;

template<>
struct QtSqlMapper<QPoint>
{
    static const QSqlRecord& record() {
        static QSqlRecord r;
        if (r.isEmpty()) {
            r.append(QSqlField("x", QVariant::Int));
            r.append(QSqlField("y", QVariant::Int));
        }
        return r;
    }

    static QSqlRecord map(const QPoint& p) {
        QSqlRecord r(record());
        r.setValue("x", p.x());
        r.setValue("y", p.y());
        return r;
    }

    static QPoint unmap(const QSqlRecord& r) {
        QPoint p;
        p.setX(r.value("x").value<int>());
        p.setY(r.value("y").value<int>());
        return p;
    }
};

template<class T>
inline QSqlRecord& operator& (QSqlRecord& r, const T& p)
{
    r = QtSqlMapper<T>::map(r);
    return r;
}

template<class T>
inline QSqlRecord& operator& (QSqlRecord& r, T& p)
{
    p = QtSqlMapper<T>::unmap(r);
    return r;
}


template<class T>
inline QSqlRecord& operator<< (QSqlRecord& r, const T& p)
{
    QString name;
    QSqlRecord rec = QtSqlMapper<T>::map(p);
    for (int i = 0; i < rec.count(); i++) {
        name = rec.fieldName(i);
        if (!r.contains(name)) {
            r.append(rec.field(i));
        }
        r.setValue(name, rec.value(i));
    }
    return r;
}

template<class T>
inline QSqlRecord& operator>> (QSqlRecord& r, T& p)
{
    return r & p;
}

int main(int argc, char *argv[])
{
    QPoint p1(5, 7), p2(0, 0);
    QSqlRecord r;
    r << p1;
    qDebug() << r;
    r >> p2;
    qDebug() << p2;

    QApplication a(argc, argv);
    Dialog w;
    w.show();

    return a.exec();
}
