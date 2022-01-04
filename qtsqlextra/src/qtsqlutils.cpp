#include "qtsqlutils.h"
#include <QObject>
#include <QtSql>

#ifdef _WIN32
#include <windows.h>
#endif

#include <sql.h>
#include <sqlext.h>
#include <sqlucode.h>

static const char *sqltypenames[] = {
     QT_TRANSLATE_NOOP("QSqlVariant", "integer"),
     QT_TRANSLATE_NOOP("QSqlVariant", "unsigned integer"),
     QT_TRANSLATE_NOOP("QSqlVariant", "long integer"),
     QT_TRANSLATE_NOOP("QSqlVariant", "unsigned long integer"),
     QT_TRANSLATE_NOOP("QSqlVariant", "binary data"),
     QT_TRANSLATE_NOOP("QSqlVariant", "character"),
     QT_TRANSLATE_NOOP("QSqlVariant", "string"),
     QT_TRANSLATE_NOOP("QSqlVariant", "boolean"),
     QT_TRANSLATE_NOOP("QSqlVariant", "date"),
     QT_TRANSLATE_NOOP("QSqlVariant", "time"),
     QT_TRANSLATE_NOOP("QSqlVariant", "date/time"),
     QT_TRANSLATE_NOOP("QSqlVariant", "real")
};

inline QString quotedString(const QString& s) {
    QString result;
    result += '\'';
    result += s;
    result += '\'';
    return s;
}

QString QtSql::formatValue(const QVariant &value, QSqlDriver *driver)
{
    if (value.type() == QVariant::List)
    {
        QString ret;
        QTextStream ts(&ret);

        const QList<QVariant> lst = value.toList();

        ts << "(";
        for (auto it = lst.cbegin(); it != lst.cend(); ++it) {
            ts << QtSql::formatValue(*it, driver);
            if (it != lst.end()-1)
                ts << ",";
        }
        ts << ")";
        return ret;
    }
    if (!driver) {
        switch(value.type()) {
        case QVariant::ByteArray:
            return quotedString(value.toByteArray());
        case QVariant::Date:
            return quotedString(qvariant_cast<QDate>(value).toString("dd.MM.yyyy"));
        case QVariant::Time:
            return quotedString(qvariant_cast<QTime>(value).toString("hh:mm:ss"));
        case QVariant::DateTime:
            return quotedString(qvariant_cast<QDateTime>(value).toString("dd.MM.yyyy hh:mm:ss"));
        case QVariant::String:
        case QVariant::Char:
            return quotedString(value.toString());
        case QVariant::Bool:
            return QString::number(qvariant_cast<bool>(value) == true ? 1 : 0);
        default:
            return value.toString();
        }
    } else {
        static QSqlField field;
        field.setType(value.type());
        field.setValue(value);
        return driver->formatValue(field);
    }
}

QString QtSql::readableName(const QVariant::Type &type)
{
    QString name;
    switch(type)
    {
    case QVariant::Int:
        name = QObject::tr(sqltypenames[0]);
        break;
    case QVariant::UInt:
        name = QObject::tr(sqltypenames[1]);
        break;
    case QVariant::LongLong:
        name = QObject::tr(sqltypenames[2]);
        break;
    case QVariant::ULongLong:
        name = QObject::tr(sqltypenames[3]);
        break;
    case QVariant::BitArray:
    case QVariant::ByteArray:
        name = QObject::tr(sqltypenames[4]);
        break;
    case QVariant::Char:
        name = QObject::tr(sqltypenames[5]);
        break;
    case QVariant::String:
        name = QObject::tr(sqltypenames[6]);
        break;
    case QVariant::Bool:
        name = QObject::tr(sqltypenames[7]);
        break;
    case QVariant::Date:
        name = QObject::tr(sqltypenames[8]);
        break;
    case QVariant::Time:
        name = QObject::tr(sqltypenames[9]);
        break;
    case QVariant::DateTime:
        name = QObject::tr(sqltypenames[10]);
        break;
    case QVariant::Double:
        name = QObject::tr(sqltypenames[11]);
        break;
    default:
        name = QVariant::typeToName(type);
    }
    return name;
}

QString QtSql::typeName(const QVariant::Type &type)
{
    QString name;
    switch(type)
    {
    case QVariant::Int:
        name = "integer";
        break;
    case QVariant::UInt:
        name = "integer";
        break;
    case QVariant::LongLong:
        name = "bigint";
        break;
    case QVariant::ULongLong:
        name = "bigint";
        break;
    case QVariant::BitArray:
    case QVariant::ByteArray:
        name = "varbinary";
        break;
    case QVariant::Char:
        name = "char";
        break;
    case QVariant::String:
        name = "varchar";
        break;
    case QVariant::Bool:
        name = "boolean";
        break;
    case QVariant::Date:
        name = "date";
        break;
    case QVariant::Time:
        name = "time";
        break;
    case QVariant::DateTime:
        name = "datetime";
        break;
    case QVariant::Double:
        name = "real";
        break;
    default:
        name = QVariant::typeToName(type);
    }
    return name;
}

QString QtSql::driverName(const QString &driver)
{
    static QHash<QString, QString> driverMapping;
    if (driverMapping.empty())
    {
        // short names
        driverMapping["mssql"]     = "QODBC";
        driverMapping["ms sql"]    = "QODBC";
        driverMapping["odbc"]      = "QODBC";
        driverMapping["mysql"]     = "QMYSQL";
        driverMapping["my sql"]    = "QMYSQL";
        driverMapping["sqlite"]    = "QSQLITE";
        driverMapping["psql"]      = "QPSQL";
        driverMapping["pgsql"]     = "QPSQL";
        driverMapping["postgres"]  = "QPSQL";
        driverMapping["postgre"]   = "QPSQL";
        driverMapping["oci"]       = "QOCI";
        driverMapping["oracle"]    = "QOCI";
        driverMapping["db2"]       = "QDB2";
        driverMapping["ibase"]     = "QIBASE";
        driverMapping["interbase"] = "QIBASE";
        driverMapping["tds"]       = "QTDS";

        // long names
        driverMapping["ms sql server"]          = "QODBC";
        driverMapping["microsoft sql server"]   = "QODBC";
        driverMapping["postgre sql"]            = "QPSQL";
        driverMapping["postgres sql"]           = "QPSQL";
        driverMapping["ibm db2"]                = "QDB2";
        driverMapping["borland interbase"]      = "QIBASE";
        driverMapping["sybase adaptive server"] = "QTDS";
    }

    if (driver.isEmpty())
        return "unknown";

    if (driver[0] == 'Q' || driver[0] == 'q')
        return driver.toUpper();

    auto it = driverMapping.constFind(driver.toLower());
    return (it != driverMapping.cend() ? it.value() : "unknown");
}

QString QtSql::driverVendor(const QString &driver)
{
    struct NamePair {
        const char* first;
        const char* second;
    };

    static const NamePair driverMapping[] = {
        { "QODBC"  , "Open Database Connectivity Driver" },
        { "QSQLITE", "SQLite Database Driver" },
        { "QMYSQL" , "MySQL Driver" },
        { "QPSQL"  , "Postgre SQL Driver" },
        { "QOCI"   , "Oracle Driver" },
        { "QDB2"   , "IBM DB2 Driver" },
        { "QIBASE" , "Borland InterBase Driver" },
        { "QTDS"   , "Sybase Adaptive Server Driver" },
        { "QWMI"   , "WMI WQL Driver" }
    };
    static const size_t mappingSize = sizeof(driverMapping) / sizeof(driverMapping[0]);

    if (driver.isEmpty())
        return "Unknown Database Vendor";

    for (const NamePair* it = driverMapping, *end = driverMapping + mappingSize; it != end; ++it)
    {
        if (driver.startsWith(it->first))
        {
            QChar version = driver[driver.size()-1];
            if (version.isDigit())
                return QString("%1(v%2)").arg(it->second).arg(version);
            else
                return it->second;
        }
    }

    return "Unknown Database Vendor";
}

inline static QString fromSQLTCHAR(const QVarLengthArray<SQLTCHAR>& input, int size=-1)
{
    QString result;

    int realsize = qMin(size, input.size());
    if(realsize > 0 && input[realsize-1] == 0)
        realsize--;
    switch(sizeof(SQLTCHAR)) {
        case 1:
            result=QString::fromUtf8((const char *)input.constData(), realsize);
            break;
        case 2:
            result=QString::fromUtf16((const ushort *)input.constData(), realsize);
            break;
        case 4:
            result=QString::fromUcs4((const uint *)input.constData(), realsize);
            break;
        default:
            qCritical("sizeof(SQLTCHAR) is %d. Don't know how to handle this.", int(sizeof(SQLTCHAR)));
    }
    return result;
}

static QString qtODBCWarnMessage(SQLHANDLE handle, int *nativeCode = 0)
{
    SQLINTEGER nativeCode_ = 0;
    SQLSMALLINT msgLen = 0;
    SQLRETURN r = SQL_NO_DATA;
    SQLTCHAR state_[SQL_SQLSTATE_SIZE+1];
    QVarLengthArray<SQLTCHAR> description_(SQL_MAX_MESSAGE_LENGTH);
    QString result;
    int i = 1;

    description_[0] = 0;
    do {
        r = SQLGetDiagRec(SQL_HANDLE_ENV,
                          handle,
                          i,
                          state_,
                          &nativeCode_,
                          0,
                          0,
                          &msgLen);
        if ((r == SQL_SUCCESS || r == SQL_SUCCESS_WITH_INFO) && msgLen > 0)
            description_.resize(msgLen+1);
        r = SQLGetDiagRec(SQL_HANDLE_ENV,
                            handle,
                            i,
                            state_,
                            &nativeCode_,
                            description_.data(),
                            description_.size(),
                            &msgLen);
        if (r == SQL_SUCCESS || r == SQL_SUCCESS_WITH_INFO) {
            if (nativeCode)
                *nativeCode = nativeCode_;
            const QString tmpstore = fromSQLTCHAR(description_, msgLen);
            if(result != tmpstore) {
                if(!result.isEmpty())
                    result += QLatin1Char(' ');
                result += tmpstore;
            }
        } else if (r == SQL_ERROR || r == SQL_INVALID_HANDLE) {
            return result;
        }
        ++i;
    } while (r != SQL_NO_DATA);
    return result;
}


QStringList QtSql::ODBCDriversList(QString* errorString)
{
    QStringList driverList;
    // Get an Environment handle
    SQLHENV hEnv = SQL_NULL_HENV;
    // SQLAllocHandle() is for ODBC 3.x
    SQLRETURN ret = ::SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    // If you do not have ODBC 3.x try with the old version:
    // SQLRETURN ret = SQLAllocEnv(&hEnv);
    if(!SQL_SUCCEEDED(ret))
    {
        if (errorString) {
            *errorString = QObject::tr("Failed to allocate SQL handle: ") + qtODBCWarnMessage(hEnv);
        }
        return driverList;
    }
    // set odbc version (this is required, if not set we get 'function sequence error')
    ret = SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_UINTEGER);
    if(!SQL_SUCCEEDED(ret))
    {
        if (errorString) {
            *errorString = QObject::tr("Failed to setup ODBC attributes: ") + qtODBCWarnMessage(hEnv);
        }
        ::SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
        return driverList;
    }

    QVarLengthArray<SQLTCHAR> buffer(4096);

    for(;;) {
        SQLSMALLINT length;
        SQLRETURN state = SQLDrivers(hEnv, SQL_FETCH_NEXT, buffer.data(), buffer.size(), &length, Q_NULLPTR, 0, Q_NULLPTR);
        if (state == SQL_NO_DATA || !SQL_SUCCEEDED(state))
            break;

        driverList << fromSQLTCHAR(buffer, length);
    }

    ::SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
    return driverList;
}
