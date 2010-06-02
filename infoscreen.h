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

#ifndef INFOSCREEN_H
#define INFOSCREEN_H

#include <QTextEdit>

class QFileInfo;

class InfoScreen : public QTextEdit
{
	Q_OBJECT

public:
	InfoScreen(QWidget *parent = 0);

	void setFileInfo(const QFileInfo &info);
	void setSearchResult(const QList<QFileInfo> &list);
};

#endif // INFOSCREEN_H
