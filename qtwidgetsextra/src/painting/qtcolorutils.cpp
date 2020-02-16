#include "qtcolorutils.h"
#include <QtCore/QHash>

#include <QtGui/QPixmapCache>
#include <QtGui/QPainter>


QString standardColorName(const QColor &color)
{
    static QHash<QColor, QString> colorNames;
    if (colorNames.isEmpty()) {
        QColor c;
        QStringList names = QColor::colorNames();
        for (auto it = names.begin(); it != names.end(); ++it) {
            c.setNamedColor(*it);
            colorNames[c] = *it;
        }
    }
    auto it = colorNames.find(color) ;
    return (it == colorNames.end() ? color.name() : *it);
}

QPixmap colorPixmap(const QColor& color, const QSize& size)
{
    QPixmap pixmap(size);
    if (QPixmapCache::find("_qt_cached_" + color.name(), &pixmap)) {
        return pixmap;
    }
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    painter.drawRoundRect(pixmap.rect().adjusted(1, 1, -1, -1), 4, 4);
    return pixmap;
}
