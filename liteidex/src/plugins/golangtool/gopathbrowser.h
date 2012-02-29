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
// Module: gopathbrowser.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2012-2-27
// $Id: gopathbrowser.h,v 1.0 2012-2-28 visualfc Exp $

#ifndef GOPATHBROWSER_H
#define GOPATHBROWSER_H

#include <QWidget>
#include "liteapi/liteapi.h"
#include "liteenvapi/liteenvapi.h"
#include <QModelIndex>
#include <QFileInfo>
#include <QDir>

class QTreeView;
class GopathModel;
class GopathBrowser : public QObject
{
    Q_OBJECT
    
public:
    explicit GopathBrowser(LiteApi::IApplication *app,QObject *parent = 0);
    ~GopathBrowser();
    QWidget *widget() const;
    void setPathList(const QStringList &pathList);
    void addPathList(const QString &path);
    QStringList pathList() const;
    QStringList systemGopathList() const;
    void setStartIndex(const QModelIndex &index);
    QString startPath() const;
public slots:
    void pathIndexChanged(const QModelIndex & index);
    void openPathIndex(const QModelIndex &index);
    void reloadEnv();
    void currentEditorChanged(LiteApi::IEditor*);
    void treeViewContextMenuRequested(const QPoint &pos);
    void openEditor();
    void newFile();
    void newFileWizard();
    void renameFile();
    void removeFile();
    void newFolder();
    void renameFolder();
    void removeFolder();
    void openShell();
    void openExplorer();
signals:
    void startPathChanged(const QString& path);
protected:
    QFileInfo contextFileInfo() const;
    QDir contextDir() const;
    static QString getShellCmd(LiteApi::IApplication *app);
    static QStringList getShellArgs(LiteApi::IApplication *app);
private:
    LiteApi::IApplication *m_liteApp;
    QWidget *m_widget;
    QTreeView *m_pathTree;
    GopathModel *m_model;
    QStringList m_pathList;
    QFileInfo m_contextInfo;
    QMenu   *m_fileMenu;
    QMenu   *m_folderMenu;
    QAction *m_openEditorAct;
    QAction *m_newFileAct;
    QAction *m_newFileWizardAct;
    QAction *m_removeFileAct;
    QAction *m_renameFileAct;
    QAction *m_newFolderAct;
    QAction *m_removeFolderAct;
    QAction *m_renameFolderAct;
    QAction *m_openShellAct;
    QAction *m_openExplorerAct;
};

#endif // GOPATHBROWSER_H