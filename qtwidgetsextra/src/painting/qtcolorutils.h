#ifndef QTCOLORUTILS_H
#define QTCOLORUTILS_H

#include <QColor>
#include <QPixmap>
#include <QIcon>

#include <QtWidgetsExtra>

inline uint qHash(const QColor& c) {
    return c.rgba();
}

QTWIDGETSEXTRA_EXPORT QString standardColorName(const QColor &color);
QTWIDGETSEXTRA_EXPORT QPixmap colorPixmap(const QColor& color, const QSize &size);


#endif
