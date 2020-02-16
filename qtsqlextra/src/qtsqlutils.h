#ifndef QTSQLUTILS_H
#define QTSQLUTILS_H


#include <QString>
#include <QVariant>

#include <QtSqlExtra>

class QSqlDriver;


namespace QtSql {

/*!
 * \brief formatValue Format QVariant value to suitable SQL string representation
 *
 * Return an SQL string representation of value.
 * \param value variant value
 * \param driver SQL driver pointer
 * \note Method provided for convinience.
 * \return string representation of stored value in appropriate SQL dialect
 */
QTSQLEXTRA_EXPORT QString formatValue(const QVariant& value, QSqlDriver *driver = 0);

/*!
 * \brief readableName Format QVariant::Type to suitable human-readable type name
 *
 * Return an human-readable name of value type.
 * \param value variant value
 * \return SQL type name of value
 */
QTSQLEXTRA_EXPORT QString readableName(const QVariant::Type& type);

/*!
 * \brief typeName Format QVariant::Type to suitable SQL type name
 *
 * Return an SQL suitable name of value type.
 * \param value variant value
 * \return SQL type name of value
 */
QTSQLEXTRA_EXPORT QString typeName(const QVariant::Type& type);

/*!
 * \brief typeName Format type name of value to suitable SQL type name
 *
 * This is overloaded method.
 * Return an SQL suitable name of value type.
 * \param value variant value
 * \return SQL type name of value
 */
inline QString typeName(const QVariant& v) {
    return typeName(v.type());
}

/*!
 * \brief driverName Format QtSql driver name
 *
 * Return an QtSql driver name.
 * \param driver vendor driver name
 * \return QtSql driver name
 */
QTSQLEXTRA_EXPORT QString driverName(const QString& driver);


/*!
 * \brief driverVendor Format vendor driver name from Qt name
 *
 * Return an vendor driver name.
 * \param driver QtSql driver name
 * \return vendor driver name
 */
QTSQLEXTRA_EXPORT QString driverVendor(const QString& driver);

/*!
 * \brief ODBCDriversList
 * \param errorString non-empty string representation of error if
 * driver list extracting fails
 * \return string list of installed ODBC SQL drivers
 */
QTSQLEXTRA_EXPORT QStringList ODBCDriversList(QString* errorString = Q_NULLPTR);


} // end namespace QtSql


#endif // QTSQLUTILS_H
