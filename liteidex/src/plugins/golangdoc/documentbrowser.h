/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011 LiteIDE Team. All rights reserved.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
**
** In addition, as a special exception,  that plugins developed for LiteIDE,
** are allowed to remain closed sourced and can be distributed under any license .
** These rights are included in the file LGPL_EXCEPTION.txt in this package.
**
**************************************************************************/
// Module: documentbrowser.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-7-7
// $Id: documentbrowser.h,v 1.0 2011-7-7 visualfc Exp $

#ifndef DOCUMENTBROWSER_H
#define DOCUMENTBROWSER_H

#include "browsereditor/browsereditorimpl.h"

class QTextBrowser;
class DocumentBrowser : public BrowserEditorImpl
{
    Q_OBJECT
public:
    DocumentBrowser(LiteApi::IApplication *app);
    ~DocumentBrowser();
    virtual QWidget *widget();
    virtual QString displayName() const;
    virtual QIcon icon() const;
    QTextBrowser *browser();
protected:
    LiteApi::IApplication   *m_liteApp;
    QWidget *m_widget;
    QTextBrowser *m_browser;
};

#endif // DOCUMENTBROWSER_H