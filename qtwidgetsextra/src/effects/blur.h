#pragma once
#include <QtWidgetsExtra>
#include <QImage>

QImage QTWIDGETSEXTRA_EXPORT stackBlurImage(const QImage& _image, int _radius, int _threadCount = 1);

QImage QTWIDGETSEXTRA_EXPORT boxBlurImage(const QImage& _image, const QRect& _rect, int _radius);
inline QImage boxBlurImage(const QImage& _image, int _radius)
{
    return boxBlurImage(_image, _image.rect(), _radius);
}


