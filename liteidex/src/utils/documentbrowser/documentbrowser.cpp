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
// Module: documentbrowser.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-7-7
// $Id: documentbrowser.cpp,v 1.0 2011-7-26 visualfc Exp $

#include "documentbrowser.h"
#include "extension/extension.h"

#include <QTextBrowser>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollBar>
#include <QStatusBar>
#include <QComboBox>
#include <QToolBar>
#include <QToolButton>
#include <QCheckBox>
#include <QAction>
#include <QRegExp>
#include <QFile>
#include <QFileInfo>
#include <QTextCodec>
#include <QDir>
#include <QDebug>

//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

DocumentBrowser::DocumentBrowser(LiteApi::IApplication *app, QObject *parent) :
    LiteApi::IDocumentBrowser(parent),
    m_liteApp(app)
{
    m_extension = new Extension;
    m_widget = new QWidget;

    m_textBrowser = new QTextBrowser;
    m_textBrowser->setOpenExternalLinks(false);
    m_textBrowser->setOpenLinks(false);

    m_toolBar = new QToolBar;
    m_toolBar->setIconSize(QSize(16,16));

    m_backwardAct = new QAction(QIcon(":/images/backward.png"),tr("Backward"),this);
    m_forwardAct = new QAction(QIcon(":/images/forward.png"),tr("Forward"),this);
    m_toolBar->addAction(m_backwardAct);
    m_toolBar->addAction(m_forwardAct);

    m_urlComboBox = new QComboBox;
    m_urlComboBox->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);

    m_toolBar->addSeparator();
    m_toolBar->addWidget(m_urlComboBox);


    m_statusBar = new QStatusBar;

    m_findComboBox = new QComboBox;
    m_findComboBox->setEditable(true);

    m_findNextAct = new QAction(tr("Next"),this);
    m_findNextAct->setShortcut(QKeySequence::FindNext);
    m_findNextAct->setToolTip(tr("FindNext\t")+QKeySequence(QKeySequence::FindNext).toString());
    m_findPrevAct = new QAction(tr("Prev"),this);
    m_findPrevAct->setShortcut(QKeySequence::FindPrevious);
    m_findPrevAct->setToolTip(tr("FindPrev\t")+QKeySequence(QKeySequence::FindPrevious).toString());

    QToolButton *findNext = new QToolButton;
    findNext->setDefaultAction(m_findNextAct);
    QToolButton *findPrev = new QToolButton;
    findPrev->setDefaultAction(m_findPrevAct);
    m_matchCaseCheckBox = new QCheckBox(tr("MatchCase"));
    m_matchWordCheckBox = new QCheckBox(tr("MatchWord"));
    m_useRegexCheckBox = new QCheckBox(tr("Regex"));

    m_statusBar->addPermanentWidget(m_findComboBox);
    m_statusBar->addPermanentWidget(findNext);
    m_statusBar->addPermanentWidget(findPrev);
    m_statusBar->addPermanentWidget(m_matchCaseCheckBox);
    m_statusBar->addPermanentWidget(m_matchWordCheckBox);
    m_statusBar->addPermanentWidget(m_useRegexCheckBox);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    mainLayout->addWidget(m_toolBar);
    mainLayout->addWidget(m_textBrowser);
    mainLayout->addWidget(m_statusBar);
    m_widget->setLayout(mainLayout);

    connect(m_textBrowser,SIGNAL(highlighted(QUrl)),this,SLOT(highlighted(QUrl)));
    connect(m_textBrowser,SIGNAL(anchorClicked(QUrl)),this,SLOT(anchorClicked(QUrl)));
    connect(m_backwardAct,SIGNAL(triggered()),this,SLOT(backward()));
    connect(m_forwardAct,SIGNAL(triggered()),this,SLOT(forward()));
    connect(m_findComboBox,SIGNAL(activated(QString)),this,SLOT(activatedFindText(QString)));
    connect(m_findNextAct,SIGNAL(triggered()),this,SLOT(findNext()));
    connect(m_findPrevAct,SIGNAL(triggered()),this,SLOT(findPrev()));
    connect(m_urlComboBox,SIGNAL(activated(QString)),this,SLOT(activatedUrl(QString)));
    connect(this,SIGNAL(backwardAvailable(bool)),m_backwardAct,SLOT(setEnabled(bool)));
    connect(this,SIGNAL(forwardAvailable(bool)),m_forwardAct,SLOT(setEnabled(bool)));

    m_liteApp->settings()->beginGroup("documentbrowser");
    m_matchWordCheckBox->setChecked(m_liteApp->settings()->value("matchword",true).toBool());
    m_matchCaseCheckBox->setChecked(m_liteApp->settings()->value("matchcase",true).toBool());
    m_useRegexCheckBox->setChecked(m_liteApp->settings()->value("useregex",false).toBool());
    m_liteApp->settings()->endGroup();

    m_extension->addObject("LiteApi.IDocumentBrowser",this);
    m_textBrowser->installEventFilter(m_liteApp->editorManager());

    emit backwardAvailable(false);
    emit forwardAvailable(false);
}

DocumentBrowser::~DocumentBrowser()
{
    m_liteApp->settings()->beginGroup("documentbrowser");
    m_liteApp->settings()->setValue("matchword",m_matchWordCheckBox->isChecked());
    m_liteApp->settings()->setValue("matchcase",m_matchCaseCheckBox->isChecked());
    m_liteApp->settings()->setValue("useregex",m_useRegexCheckBox->isChecked());
    m_liteApp->settings()->endGroup();

    if (m_widget) {
        delete m_widget;
    }
    if (m_extension) {
        delete m_extension;
    }
}

LiteApi::IExtension *DocumentBrowser::extension()
{
    return m_extension;
}

bool DocumentBrowser::open(const QString &fileName,const QString &mimeType)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }    
    m_mimeType = mimeType;
    QFileInfo info(fileName);
    QString htmlType = m_liteApp->mimeTypeManager()->findFileMimeType(fileName);
    m_name = info.fileName();
    m_fileName = QDir::toNativeSeparators(fileName);

    QStringList paths = m_textBrowser->searchPaths();
    paths << info.absolutePath();
    m_textBrowser->setSearchPaths(paths);

    QByteArray ba = file.readAll();
    if (htmlType == "text/html") {
        QTextCodec *codec = QTextCodec::codecForHtml(ba);
        setUrlHtml(QUrl::fromLocalFile(fileName),codec->toUnicode(ba));
    } else {
        QTextCodec *codec = QTextCodec::codecForLocale();
        setUrlHtml(QUrl::fromLocalFile(fileName),codec->toUnicode(ba),false);
    }
    file.close();
    return true;
}

QWidget *DocumentBrowser::widget()
{
    return m_widget;
}

QString DocumentBrowser::name() const
{
    return m_name;
}

QString DocumentBrowser::fileName() const
{
    return m_fileName;
}

QString DocumentBrowser::mimeType() const
{
    return m_mimeType;
}

void DocumentBrowser::setName(const QString &t)
{
    m_name = t;
}

void DocumentBrowser::highlighted(QUrl url)
{
    m_statusBar->showMessage(url.toString());
}

void DocumentBrowser::activatedFindText(QString)
{
    findText(false);
}

void DocumentBrowser::findNext()
{
    findText(false);
}

void DocumentBrowser::findPrev()
{
    findText(true);
}

bool DocumentBrowser::findText(bool findBackward)
{
    QString text = m_findComboBox->currentText();
    if (text.isEmpty()) {
        return false;
    }
    QTextCursor cursor = m_textBrowser->textCursor();
    QTextDocument::FindFlags flags = 0;
    if (findBackward) {
        flags |= QTextDocument::FindBackward;
    }
    if (m_matchCaseCheckBox->isChecked()) {
        flags |= QTextDocument::FindCaseSensitively;
    }
    if (m_matchWordCheckBox->isChecked()) {
        flags |= QTextDocument::FindWholeWords;
    }
    QTextCursor find;
    if (m_useRegexCheckBox->isChecked()) {
        find = m_textBrowser->document()->find(QRegExp(text),cursor,flags);
    } else {
        find = m_textBrowser->document()->find(text,cursor,flags);
    }
    if (!find.isNull()) {
        m_textBrowser->setTextCursor(find);
        m_statusBar->showMessage(QString("find %1").arg(find.selectedText()));
        return true;
    }
    m_statusBar->showMessage("end find");
    return false;
}

void DocumentBrowser::setSearchPaths(const QStringList &paths)
{
    m_textBrowser->setSearchPaths(paths);
}

QUrl DocumentBrowser::resolveUrl(const QUrl &url) const
{
    if (!url.isRelative())
        return url;

    // For the second case QUrl can merge "#someanchor" with "foo.html"
    // correctly to "foo.html#someanchor"
    if (!(m_url.isRelative()
          || (m_url.scheme() == QLatin1String("file")
              && !QFileInfo(m_url.toLocalFile()).isAbsolute()))
          || (url.hasFragment() && url.path().isEmpty())) {
        return m_url.resolved(url);
    }

    // this is our last resort when current url and new url are both relative
    // we try to resolve against the current working directory in the local
    // file system.
    QFileInfo fi(m_url.toLocalFile());
    if (fi.exists()) {
        return QUrl::fromLocalFile(fi.absolutePath() + QDir::separator()).resolved(url);
    }

    return url;
}

void DocumentBrowser::setUrlHtml(const QUrl &url,const QString &data)
{
    setUrlHtml(url,data,false);
}

void DocumentBrowser::setUrlHtml(const QUrl &url,const QString &data,bool html)
{
    const HistoryEntry &historyEntry = createHistoryEntry();
    if (html) {
        m_textBrowser->setHtml(data);
    } else {
        m_textBrowser->setText(data);
    }
    m_url = resolveUrl(url);
    if (!url.fragment().isEmpty()) {
        m_textBrowser->scrollToAnchor(url.fragment());
    } else {
        m_textBrowser->horizontalScrollBar()->setValue(0);
        m_textBrowser->verticalScrollBar()->setValue(0);
    }

    int index = m_urlComboBox->findText(m_url.toString());
    if (index == -1) {
        m_urlComboBox->addItem(m_url.toString());
        index = m_urlComboBox->count()-1;
    }
    m_urlComboBox->setCurrentIndex(index);

    if (!m_backwardStack.isEmpty() && url == m_backwardStack.top().url)
    {
        restoreHistoryEntry(m_backwardStack.top());
        return;
    }

    if (!m_backwardStack.isEmpty()) {
        m_backwardStack.top() = historyEntry;
    }

    HistoryEntry entry;
    entry.url = url;
    m_backwardStack.push(entry);

    emit backwardAvailable(m_backwardStack.count() > 1);

    if (!m_forwardStack.isEmpty() && url == m_forwardStack.top().url) {
        m_forwardStack.pop();
        emit forwardAvailable(m_forwardStack.count() > 0);
    } else {
        m_forwardStack.clear();
        emit forwardAvailable(false);
    }
}

void DocumentBrowser::activatedUrl(QString text)
{
    if (text.isEmpty()) {
        return;
    }
    QUrl url(text);
    requestUrl(url);
}

DocumentBrowser::HistoryEntry DocumentBrowser::createHistoryEntry() const
{
    HistoryEntry entry;
    entry.url = m_url;
    entry.hpos = m_textBrowser->horizontalScrollBar()->value();
    entry.vpos = m_textBrowser->verticalScrollBar()->value();
    return entry;
}

void DocumentBrowser::restoreHistoryEntry(const HistoryEntry &entry)
{
    m_url = entry.url;
    m_textBrowser->horizontalScrollBar()->setValue(entry.hpos);
    m_textBrowser->verticalScrollBar()->setValue(entry.vpos);
}

void DocumentBrowser::backward()
{
    if (m_backwardStack.count() <= 1) {
        return;
    }
    m_forwardStack.push(createHistoryEntry());
    m_backwardStack.pop();
    emit requestUrl(m_backwardStack.top().url);
    emit backwardAvailable(m_backwardStack.count() > 1);
    emit forwardAvailable(true);
}

void DocumentBrowser::forward()
{
    if (m_forwardStack.isEmpty()) {
        return;
    }
    if (!m_backwardStack.isEmpty()) {
        m_backwardStack.top() = createHistoryEntry();
    }
    m_backwardStack.push(m_forwardStack.pop());
    emit requestUrl(m_backwardStack.top().url);
    emit backwardAvailable(true);
    emit forwardAvailable(m_forwardStack.count() > 0);
}

void DocumentBrowser::anchorClicked(QUrl url)
{
    QString text = url.toString(QUrl::RemoveFragment);
    if (text.isEmpty() && !url.fragment().isEmpty()) {
        m_textBrowser->scrollToAnchor(url.fragment());
        return;
    }
    emit requestUrl(url);
}
