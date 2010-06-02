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

#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "fileviewer.h"
#include "infoscreen.h"

#include <QStackedWidget>

class QKeyEvent;

class FileManager : public QStackedWidget
{
    Q_OBJECT
    
public:
    FileManager(QWidget *parent= 0, Qt::WindowFlags flags= 0);
	virtual ~FileManager();

	InfoScreen* infoScreen();
	FileViewer* fileViewer();

protected:
	void keyPressEvent(QKeyEvent *event);
    
private:
	void loadSettings();
	void saveSettings();

    FileViewer* m_viewer;
    InfoScreen* m_infoScreen;
};

#endif // FILEMANAGER_H
