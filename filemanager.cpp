/****************************************************************************
**
** Copyright (C) 2007-2008 TROLLTECH ASA. All rights reserved.
**
** This file is part of the Phone Edition of the Qtopia Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "filemanager.h"

#include <QKeyEvent>
#include <QSettings>

FileManager::FileManager(QWidget *parent, Qt::WindowFlags flags)
    : QStackedWidget(parent)
	, m_viewer(NULL)
	, m_infoScreen(NULL)
{
 	setWindowFlags(flags);
	setCurrentWidget(fileViewer());

	loadSettings();
}

FileManager::~FileManager()
{
	saveSettings();
}
	
InfoScreen* FileManager::infoScreen()
{
	if(!m_infoScreen) {
		m_infoScreen = new InfoScreen(this);
		addWidget(m_infoScreen);
	}
	return m_infoScreen;
}

FileViewer* FileManager::fileViewer()
{
	if(!m_viewer) {
		m_viewer = new FileViewer(this);
		addWidget(m_viewer);
	}
	return m_viewer;
}
	
void FileManager::keyPressEvent(QKeyEvent *event)
{
	if(event->key() == Qt::Key_Back) {
		if(currentIndex() == indexOf(infoScreen())) {
			setCurrentWidget(fileViewer());
			return;
		}
	} else {
		QStackedWidget::keyPressEvent(event);
	}
}

void FileManager::loadSettings()
{
	QSettings settings("Trolltech", "FileManager");

	if(m_viewer) {
		m_viewer->setHiddenFilesShown(settings.value("view/showhidden").value<bool>());
		m_viewer->setCurrentViewType(static_cast<FileViewer::ViewType>(settings.value("view/type").value<int>()));
		m_viewer->setSortOrderReversed(settings.value("view/sort/reversed").value<bool>());
		m_viewer->setSorting(static_cast<FileViewer::SortType>(settings.value("view/sort/type").value<int>()));
	}
}

void FileManager::saveSettings()
{
	QSettings settings("Trolltech", "FileManager");

	if(m_viewer) {
		settings.setValue("view/showhidden", m_viewer->hiddenFilesShown());
		settings.setValue("view/type", m_viewer->currentViewType());
		settings.setValue("view/sort/reversed", m_viewer->sortOrderReversed());
		settings.setValue("view/sort/type", m_viewer->sorting());
	}
}
