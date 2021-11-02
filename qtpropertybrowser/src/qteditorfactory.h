/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QTEDITORFACTORY_H
#define QTEDITORFACTORY_H

#include "qtpropertymanager.h"

#if QT_VERSION >= 0x040400
QT_BEGIN_NAMESPACE
#endif

class QtSpinBoxFactoryPrivate;

class QT_QTPROPERTYBROWSER_EXPORT QtSpinBoxFactory : public QtAbstractEditorFactory<QtIntPropertyManager>
{
    Q_OBJECT
public:
    explicit QtSpinBoxFactory(QObject *parent = Q_NULLPTR);
    ~QtSpinBoxFactory();
protected:
    void connectPropertyManager(QtIntPropertyManager *manager) Q_DECL_OVERRIDE;
    QWidget *createEditor(QtIntPropertyManager *manager, QtProperty *property, QWidget *parent) const Q_DECL_OVERRIDE;
    void disconnectPropertyManager(QtIntPropertyManager *manager) Q_DECL_OVERRIDE;
private:
    QScopedPointer<QtSpinBoxFactoryPrivate> d_ptr;
    Q_DECLARE_PRIVATE(QtSpinBoxFactory)
    Q_DISABLE_COPY(QtSpinBoxFactory)
    Q_PRIVATE_SLOT(d_func(), void slotPropertyChanged(QtProperty *, int))
    Q_PRIVATE_SLOT(d_func(), void slotRangeChanged(QtProperty *, int, int))
    Q_PRIVATE_SLOT(d_func(), void slotSingleStepChanged(QtProperty *, int))
    Q_PRIVATE_SLOT(d_func(), void slotSetValue(int))
    Q_PRIVATE_SLOT(d_func(), void slotEditorDestroyed(QObject *))
};

class QtSliderFactoryPrivate;

class QT_QTPROPERTYBROWSER_EXPORT QtSliderFactory : public QtAbstractEditorFactory<QtIntPropertyManager>
{
    Q_OBJECT
public:
    explicit QtSliderFactory(QObject *parent = Q_NULLPTR);
    ~QtSliderFactory();
protected:
    void connectPropertyManager(QtIntPropertyManager *manager) Q_DECL_OVERRIDE;
    QWidget *createEditor(QtIntPropertyManager *manager, QtProperty *property, QWidget *parent) const Q_DECL_OVERRIDE;
    void disconnectPropertyManager(QtIntPropertyManager *manager) Q_DECL_OVERRIDE;
private:
    QScopedPointer<QtSliderFactoryPrivate> d_ptr;
    Q_DECLARE_PRIVATE(QtSliderFactory)
    Q_DISABLE_COPY(QtSliderFactory)
    Q_PRIVATE_SLOT(d_func(), void slotPropertyChanged(QtProperty *, int))
    Q_PRIVATE_SLOT(d_func(), void slotRangeChanged(QtProperty *, int, int))
    Q_PRIVATE_SLOT(d_func(), void slotSingleStepChanged(QtProperty *, int))
    Q_PRIVATE_SLOT(d_func(), void slotSetValue(int))
    Q_PRIVATE_SLOT(d_func(), void slotEditorDestroyed(QObject *))
};

class QtScrollBarFactoryPrivate;

class QT_QTPROPERTYBROWSER_EXPORT QtScrollBarFactory : public QtAbstractEditorFactory<QtIntPropertyManager>
{
    Q_OBJECT
public:
    explicit QtScrollBarFactory(QObject *parent = Q_NULLPTR);
    ~QtScrollBarFactory();
protected:
    void connectPropertyManager(QtIntPropertyManager *manager) Q_DECL_OVERRIDE;
    QWidget *createEditor(QtIntPropertyManager *manager, QtProperty *property, QWidget *parent) const Q_DECL_OVERRIDE;
    void disconnectPropertyManager(QtIntPropertyManager *manager) Q_DECL_OVERRIDE;
private:
    QScopedPointer<QtScrollBarFactoryPrivate> d_ptr;
    Q_DECLARE_PRIVATE(QtScrollBarFactory)
    Q_DISABLE_COPY(QtScrollBarFactory)
    Q_PRIVATE_SLOT(d_func(), void slotPropertyChanged(QtProperty *, int))
    Q_PRIVATE_SLOT(d_func(), void slotRangeChanged(QtProperty *, int, int))
    Q_PRIVATE_SLOT(d_func(), void slotSingleStepChanged(QtProperty *, int))
    Q_PRIVATE_SLOT(d_func(), void slotSetValue(int))
    Q_PRIVATE_SLOT(d_func(), void slotEditorDestroyed(QObject *))
};

class QtCheckBoxFactoryPrivate;

class QT_QTPROPERTYBROWSER_EXPORT QtCheckBoxFactory : public QtAbstractEditorFactory<QtBoolPropertyManager>
{
    Q_OBJECT
public:
    explicit QtCheckBoxFactory(QObject *parent = Q_NULLPTR);
    ~QtCheckBoxFactory();
protected:
    void connectPropertyManager(QtBoolPropertyManager *manager) Q_DECL_OVERRIDE;
    QWidget *createEditor(QtBoolPropertyManager *manager, QtProperty *property, QWidget *parent) const Q_DECL_OVERRIDE;
    void disconnectPropertyManager(QtBoolPropertyManager *manager) Q_DECL_OVERRIDE;
private:
    QScopedPointer<QtCheckBoxFactoryPrivate> d_ptr;
    Q_DECLARE_PRIVATE(QtCheckBoxFactory)
    Q_DISABLE_COPY(QtCheckBoxFactory)
    Q_PRIVATE_SLOT(d_func(), void slotPropertyChanged(QtProperty *, bool))
    Q_PRIVATE_SLOT(d_func(), void slotSetValue(bool))
    Q_PRIVATE_SLOT(d_func(), void slotEditorDestroyed(QObject *))
};

class QtDoubleSpinBoxFactoryPrivate;

class QT_QTPROPERTYBROWSER_EXPORT QtDoubleSpinBoxFactory : public QtAbstractEditorFactory<QtDoublePropertyManager>
{
    Q_OBJECT
public:
    explicit QtDoubleSpinBoxFactory(QObject *parent = Q_NULLPTR);
    ~QtDoubleSpinBoxFactory();
protected:
    void connectPropertyManager(QtDoublePropertyManager *manager) Q_DECL_OVERRIDE;
    QWidget *createEditor(QtDoublePropertyManager *manager, QtProperty *property, QWidget *parent) const Q_DECL_OVERRIDE;
    void disconnectPropertyManager(QtDoublePropertyManager *manager) Q_DECL_OVERRIDE;
private:
    QScopedPointer<QtDoubleSpinBoxFactoryPrivate> d_ptr;
    Q_DECLARE_PRIVATE(QtDoubleSpinBoxFactory)
    Q_DISABLE_COPY(QtDoubleSpinBoxFactory)
    Q_PRIVATE_SLOT(d_func(), void slotPropertyChanged(QtProperty *, double))
    Q_PRIVATE_SLOT(d_func(), void slotRangeChanged(QtProperty *, double, double))
    Q_PRIVATE_SLOT(d_func(), void slotSingleStepChanged(QtProperty *, double))
    Q_PRIVATE_SLOT(d_func(), void slotDecimalsChanged(QtProperty *, int))
    Q_PRIVATE_SLOT(d_func(), void slotSetValue(double))
    Q_PRIVATE_SLOT(d_func(), void slotEditorDestroyed(QObject *))
};

class QtLineEditFactoryPrivate;

class QT_QTPROPERTYBROWSER_EXPORT QtLineEditFactory : public QtAbstractEditorFactory<QtStringPropertyManager>
{
    Q_OBJECT
public:
    explicit QtLineEditFactory(QObject *parent = Q_NULLPTR);
    ~QtLineEditFactory();
protected:
    void connectPropertyManager(QtStringPropertyManager *manager) Q_DECL_OVERRIDE;
    QWidget *createEditor(QtStringPropertyManager *manager, QtProperty *property, QWidget *parent) const Q_DECL_OVERRIDE;
    void disconnectPropertyManager(QtStringPropertyManager *manager) Q_DECL_OVERRIDE;
private:
    QScopedPointer<QtLineEditFactoryPrivate> d_ptr;
    Q_DECLARE_PRIVATE(QtLineEditFactory)
    Q_DISABLE_COPY(QtLineEditFactory)
    Q_PRIVATE_SLOT(d_func(), void slotPropertyChanged(QtProperty *, const QString &))
    Q_PRIVATE_SLOT(d_func(), void slotRegExpChanged(QtProperty *, const QRegExp &))
    Q_PRIVATE_SLOT(d_func(), void slotSetValue(const QString &))
    Q_PRIVATE_SLOT(d_func(), void slotEditorDestroyed(QObject *))
};

class QtDateEditFactoryPrivate;

class QT_QTPROPERTYBROWSER_EXPORT QtDateEditFactory : public QtAbstractEditorFactory<QtDatePropertyManager>
{
    Q_OBJECT
public:
    explicit QtDateEditFactory(QObject *parent = Q_NULLPTR);
    ~QtDateEditFactory();
protected:
    void connectPropertyManager(QtDatePropertyManager *manager) Q_DECL_OVERRIDE;
    QWidget *createEditor(QtDatePropertyManager *manager, QtProperty *property, QWidget *parent) const Q_DECL_OVERRIDE;
    void disconnectPropertyManager(QtDatePropertyManager *manager) Q_DECL_OVERRIDE;
private:
    QScopedPointer<QtDateEditFactoryPrivate> d_ptr;
    Q_DECLARE_PRIVATE(QtDateEditFactory)
    Q_DISABLE_COPY(QtDateEditFactory)
    Q_PRIVATE_SLOT(d_func(), void slotPropertyChanged(QtProperty *, const QDate &))
    Q_PRIVATE_SLOT(d_func(), void slotRangeChanged(QtProperty *,
                        const QDate &, const QDate &))
    Q_PRIVATE_SLOT(d_func(), void slotSetValue(const QDate &))
    Q_PRIVATE_SLOT(d_func(), void slotEditorDestroyed(QObject *))
};

class QtTimeEditFactoryPrivate;

class QT_QTPROPERTYBROWSER_EXPORT QtTimeEditFactory : public QtAbstractEditorFactory<QtTimePropertyManager>
{
    Q_OBJECT
public:
    explicit QtTimeEditFactory(QObject *parent = Q_NULLPTR);
    ~QtTimeEditFactory();
protected:
    void connectPropertyManager(QtTimePropertyManager *manager) Q_DECL_OVERRIDE;
    QWidget *createEditor(QtTimePropertyManager *manager, QtProperty *property, QWidget *parent) const Q_DECL_OVERRIDE;
    void disconnectPropertyManager(QtTimePropertyManager *manager) Q_DECL_OVERRIDE;
private:
    QScopedPointer<QtTimeEditFactoryPrivate> d_ptr;
    Q_DECLARE_PRIVATE(QtTimeEditFactory)
    Q_DISABLE_COPY(QtTimeEditFactory)
    Q_PRIVATE_SLOT(d_func(), void slotPropertyChanged(QtProperty *, const QTime &))
    Q_PRIVATE_SLOT(d_func(), void slotSetValue(const QTime &))
    Q_PRIVATE_SLOT(d_func(), void slotEditorDestroyed(QObject *))
};

class QtDateTimeEditFactoryPrivate;

class QT_QTPROPERTYBROWSER_EXPORT QtDateTimeEditFactory : public QtAbstractEditorFactory<QtDateTimePropertyManager>
{
    Q_OBJECT
public:
    QtDateTimeEditFactory(QObject *parent = 0);
    ~QtDateTimeEditFactory();
protected:
    void connectPropertyManager(QtDateTimePropertyManager *manager) Q_DECL_OVERRIDE;
    QWidget *createEditor(QtDateTimePropertyManager *manager, QtProperty *property, QWidget *parent) const Q_DECL_OVERRIDE;
    void disconnectPropertyManager(QtDateTimePropertyManager *manager) Q_DECL_OVERRIDE;
private:
    QScopedPointer<QtDateTimeEditFactoryPrivate> d_ptr;
    Q_DECLARE_PRIVATE(QtDateTimeEditFactory)
    Q_DISABLE_COPY(QtDateTimeEditFactory)
    Q_PRIVATE_SLOT(d_func(), void slotPropertyChanged(QtProperty *, const QDateTime &))
    Q_PRIVATE_SLOT(d_func(), void slotSetValue(const QDateTime &))
    Q_PRIVATE_SLOT(d_func(), void slotEditorDestroyed(QObject *))
};

class QtKeySequenceEditorFactoryPrivate;

class QT_QTPROPERTYBROWSER_EXPORT QtKeySequenceEditorFactory : public QtAbstractEditorFactory<QtKeySequencePropertyManager>
{
    Q_OBJECT
public:
    explicit QtKeySequenceEditorFactory(QObject *parent = Q_NULLPTR);
    ~QtKeySequenceEditorFactory();
protected:
    void connectPropertyManager(QtKeySequencePropertyManager *manager) Q_DECL_OVERRIDE;
    QWidget *createEditor(QtKeySequencePropertyManager *manager, QtProperty *property, QWidget *parent) const Q_DECL_OVERRIDE;
    void disconnectPropertyManager(QtKeySequencePropertyManager *manager) Q_DECL_OVERRIDE;
private:
    QScopedPointer<QtKeySequenceEditorFactoryPrivate> d_ptr;
    Q_DECLARE_PRIVATE(QtKeySequenceEditorFactory)
    Q_DISABLE_COPY(QtKeySequenceEditorFactory)
    Q_PRIVATE_SLOT(d_func(), void slotPropertyChanged(QtProperty *, const QKeySequence &))
    Q_PRIVATE_SLOT(d_func(), void slotSetValue(const QKeySequence &))
    Q_PRIVATE_SLOT(d_func(), void slotEditorDestroyed(QObject *))
};

class QtCharEditorFactoryPrivate;

class QT_QTPROPERTYBROWSER_EXPORT QtCharEditorFactory : public QtAbstractEditorFactory<QtCharPropertyManager>
{
    Q_OBJECT
public:
    explicit QtCharEditorFactory(QObject *parent = Q_NULLPTR);
    ~QtCharEditorFactory();
protected:
    void connectPropertyManager(QtCharPropertyManager *manager) Q_DECL_OVERRIDE;
    QWidget *createEditor(QtCharPropertyManager *manager, QtProperty *property, QWidget *parent) const Q_DECL_OVERRIDE;
    void disconnectPropertyManager(QtCharPropertyManager *manager) Q_DECL_OVERRIDE;
private:
    QScopedPointer<QtCharEditorFactoryPrivate> d_ptr;
    Q_DECLARE_PRIVATE(QtCharEditorFactory)
    Q_DISABLE_COPY(QtCharEditorFactory)
    Q_PRIVATE_SLOT(d_func(), void slotPropertyChanged(QtProperty *, const QChar &))
    Q_PRIVATE_SLOT(d_func(), void slotSetValue(const QChar &))
    Q_PRIVATE_SLOT(d_func(), void slotEditorDestroyed(QObject *))
};

class QtEnumEditorFactoryPrivate;

class QT_QTPROPERTYBROWSER_EXPORT QtEnumEditorFactory : public QtAbstractEditorFactory<QtEnumPropertyManager>
{
    Q_OBJECT
public:
    explicit QtEnumEditorFactory(QObject *parent = Q_NULLPTR);
    ~QtEnumEditorFactory();
protected:
    void connectPropertyManager(QtEnumPropertyManager *manager) Q_DECL_OVERRIDE;
    QWidget *createEditor(QtEnumPropertyManager *manager, QtProperty *property, QWidget *parent) const Q_DECL_OVERRIDE;
    void disconnectPropertyManager(QtEnumPropertyManager *manager) Q_DECL_OVERRIDE;
private:
    QScopedPointer<QtEnumEditorFactoryPrivate> d_ptr;
    Q_DECLARE_PRIVATE(QtEnumEditorFactory)
    Q_DISABLE_COPY(QtEnumEditorFactory)
    Q_PRIVATE_SLOT(d_func(), void slotPropertyChanged(QtProperty *, int))
    Q_PRIVATE_SLOT(d_func(), void slotEnumNamesChanged(QtProperty *,
                        const QStringList &))
    Q_PRIVATE_SLOT(d_func(), void slotEnumIconsChanged(QtProperty *,
                        const QMap<int, QIcon> &))
    Q_PRIVATE_SLOT(d_func(), void slotSetValue(int))
    Q_PRIVATE_SLOT(d_func(), void slotEditorDestroyed(QObject *))
};

class QtCursorEditorFactoryPrivate;

class QT_QTPROPERTYBROWSER_EXPORT QtCursorEditorFactory : public QtAbstractEditorFactory<QtCursorPropertyManager>
{
    Q_OBJECT
public:
    explicit QtCursorEditorFactory(QObject *parent = Q_NULLPTR);
    ~QtCursorEditorFactory();
protected:
    void connectPropertyManager(QtCursorPropertyManager *manager) Q_DECL_OVERRIDE;
    QWidget *createEditor(QtCursorPropertyManager *manager, QtProperty *property, QWidget *parent) const Q_DECL_OVERRIDE;
    void disconnectPropertyManager(QtCursorPropertyManager *manager) Q_DECL_OVERRIDE;
private:
    QScopedPointer<QtCursorEditorFactoryPrivate> d_ptr;
    Q_DECLARE_PRIVATE(QtCursorEditorFactory)
    Q_DISABLE_COPY(QtCursorEditorFactory)
    Q_PRIVATE_SLOT(d_func(), void slotPropertyChanged(QtProperty *, const QCursor &))
    Q_PRIVATE_SLOT(d_func(), void slotEnumChanged(QtProperty *, int))
    Q_PRIVATE_SLOT(d_func(), void slotEditorDestroyed(QObject *))
};

class QtColorEditorFactoryPrivate;

class QT_QTPROPERTYBROWSER_EXPORT QtColorEditorFactory : public QtAbstractEditorFactory<QtColorPropertyManager>
{
    Q_OBJECT
public:
    explicit QtColorEditorFactory(QObject *parent = Q_NULLPTR);
    ~QtColorEditorFactory();
protected:
    void connectPropertyManager(QtColorPropertyManager *manager) Q_DECL_OVERRIDE;
    QWidget *createEditor(QtColorPropertyManager *manager, QtProperty *property, QWidget *parent) const Q_DECL_OVERRIDE;
    void disconnectPropertyManager(QtColorPropertyManager *manager) Q_DECL_OVERRIDE;
private:
    QScopedPointer<QtColorEditorFactoryPrivate> d_ptr;
    Q_DECLARE_PRIVATE(QtColorEditorFactory)
    Q_DISABLE_COPY(QtColorEditorFactory)
    Q_PRIVATE_SLOT(d_func(), void slotPropertyChanged(QtProperty *, const QColor &))
    Q_PRIVATE_SLOT(d_func(), void slotEditorDestroyed(QObject *))
    Q_PRIVATE_SLOT(d_func(), void slotSetValue(const QColor &))
};

class QtFontEditorFactoryPrivate;

class QT_QTPROPERTYBROWSER_EXPORT QtFontEditorFactory : public QtAbstractEditorFactory<QtFontPropertyManager>
{
    Q_OBJECT
public:
    explicit QtFontEditorFactory(QObject *parent = Q_NULLPTR);
    ~QtFontEditorFactory();
protected:
    void connectPropertyManager(QtFontPropertyManager *manager) Q_DECL_OVERRIDE;
    QWidget *createEditor(QtFontPropertyManager *manager, QtProperty *property, QWidget *parent) const Q_DECL_OVERRIDE;
    void disconnectPropertyManager(QtFontPropertyManager *manager) Q_DECL_OVERRIDE;
private:
    QScopedPointer<QtFontEditorFactoryPrivate> d_ptr;
    Q_DECLARE_PRIVATE(QtFontEditorFactory)
    Q_DISABLE_COPY(QtFontEditorFactory)
    Q_PRIVATE_SLOT(d_func(), void slotPropertyChanged(QtProperty *, const QFont &))
    Q_PRIVATE_SLOT(d_func(), void slotEditorDestroyed(QObject *))
    Q_PRIVATE_SLOT(d_func(), void slotSetValue(const QFont &))
};

#if QT_VERSION >= 0x040400
QT_END_NAMESPACE
#endif

#endif
