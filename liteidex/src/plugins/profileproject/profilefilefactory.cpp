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
// Module: profilefilefactory.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: profilefilefactory.cpp,v 1.0 2011-5-12 visualfc Exp $

#include "profilefilefactory.h"
#include "profilefile.h"
#include "profileproject.h"

//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

ProfileFileFactory::ProfileFileFactory(LiteApi::IApplication *app, QObject *parent)
    : LiteApi::IFileFactory(parent),
      m_liteApp(app)
{
    m_mimeTypes.append("text/x-profile");
    m_mimeTypes.append("text/x-gopro");
}

QStringList ProfileFileFactory::mimeTypes() const
{
    return m_mimeTypes;
}

LiteApi::IFile *ProfileFileFactory::open(const QString &fileName, const QString &mimeType)
{
    if (!m_mimeTypes.contains(mimeType)) {
        return 0;
    }
    ProfileProject *project = new ProfileProject(m_liteApp);
    project->setModelFile(new ProfileFile(m_liteApp,project));
    if (!project->open(fileName,mimeType)) {
        delete project;
        return 0;
    }
    m_liteApp->projectManager()->setCurrentProject(project);
    return project->file();
}

QString ProfileFileFactory::target(const QString &fileName, const QString &mimetype) const
{
    if (!m_mimeTypes.contains(mimetype)) {
        return QString();
    }
    QString target;
    ProfileFile *file = new ProfileFile(m_liteApp,0);
    if (file->open(fileName,mimetype)) {
        target = file->target();
    }
    delete file;
    return target;
}
