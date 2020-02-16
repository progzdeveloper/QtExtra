#include "qtpropertyproxyobject.h"



QtPropertyProxyObject::QtPropertyProxyObject( QObject *parent /*= 0*/ ) : QObject(parent)
{
}

QtPropertyProxyObject::~QtPropertyProxyObject()
{
}

void QtPropertyProxyObject::apply()
{
	// does nothing
}

void QtPropertyProxyObject::restoreDefaults()
{
	// does nothing
}