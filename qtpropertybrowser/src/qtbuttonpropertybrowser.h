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

#ifndef QTBUTTONPROPERTYBROWSER_H
#define QTBUTTONPROPERTYBROWSER_H

#include "qtpropertybrowser.h"

#if QT_VERSION >= 0x040400
QT_BEGIN_NAMESPACE
#endif

class QtButtonPropertyBrowserPrivate;

class QT_QTPROPERTYBROWSER_EXPORT QtButtonPropertyBrowser : public QtAbstractPropertyBrowser
{
    Q_OBJECT
public:

    explicit QtButtonPropertyBrowser(QWidget *parent = Q_NULLPTR);
    ~QtButtonPropertyBrowser();

    void setExpanded(QtBrowserItem *item, bool expanded);
    bool isExpanded(QtBrowserItem *item) const;

    // QtAbstractPropertyBrowser interface
public:
    void setItemVisible(QtBrowserItem *item, bool visible) Q_DECL_OVERRIDE;
    bool isItemVisible(QtBrowserItem *item) const Q_DECL_OVERRIDE;

Q_SIGNALS:

    void collapsed(QtBrowserItem *item);
    void expanded(QtBrowserItem *item);

protected:
    void itemInserted(QtBrowserItem *item, QtBrowserItem *afterItem) Q_DECL_OVERRIDE;
    void itemRemoved(QtBrowserItem *item) Q_DECL_OVERRIDE;
    void itemChanged(QtBrowserItem *item) Q_DECL_OVERRIDE;

private:

    QScopedPointer<QtButtonPropertyBrowserPrivate> d_ptr;
    Q_DECLARE_PRIVATE(QtButtonPropertyBrowser)
    Q_DISABLE_COPY(QtButtonPropertyBrowser)
    Q_PRIVATE_SLOT(d_func(), void slotUpdate())
    Q_PRIVATE_SLOT(d_func(), void slotEditorDestroyed())
    Q_PRIVATE_SLOT(d_func(), void slotToggled(bool))
};

#if QT_VERSION >= 0x040400
QT_END_NAMESPACE
#endif

#endif
