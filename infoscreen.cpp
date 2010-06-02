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

#include "infoscreen.h"

#include <QFileInfo>
#include <QString>
#include <QDateTime>
#include <QMenu>
#include <QSoftMenuBar>
#include <QMimeType>

InfoScreen::InfoScreen(QWidget *parent)
	: QTextEdit(parent)
{
	setReadOnly(true);

	QMenu* menu = QSoftMenuBar::menuFor(this);
	QSoftMenuBar::setLabel(this, Qt::Key_Select, "", "", QSoftMenuBar::AnyFocus); 
	menu->clear();
}

void InfoScreen::setSearchResult(const QList<QFileInfo> &list)
{
	QString html = "<h4>" + tr("Search Results") + "</h4><hr/><table>";

	foreach(QFileInfo info, list) {
		html += QString("<tr><td align=\"left\">%1</td></tr>").arg(info.absoluteFilePath());
	}

	html += "</table>";
	document()->setHtml(html);
}

void InfoScreen::setFileInfo(const QFileInfo &info)
{
	QStringList keys;
	QStringList values;

	QString name = info.fileName();
	if(info.isSymLink()) {
		name += " -> " + info.symLinkTarget();
	}

	QString html = "<h4>" + name + "</h4><hr/><table cellspacing=\"5\">";

	keys << tr("Owner:");
	values << info.owner();
	keys << tr("Group:");
	values << info.group();
	keys << tr("Permissions:");
	QString p;
	if(info.isFile()) {
		p += "-";
	} else if(info.isSymLink()) {
		p += "l";
	} else if(info.isDir()) {
		p += "d";
	}
	(info.permission(QFile::ReadOwner))	? p += "r" : p += "-";
	(info.permission(QFile::WriteOwner))? p += "w" : p += "-";
	(info.permission(QFile::ExeOwner)) 	? p += "x" : p += "-";
	(info.permission(QFile::ReadGroup)) ? p += "r" : p += "-";
	(info.permission(QFile::WriteGroup))? p += "w" : p += "-";
	(info.permission(QFile::ExeGroup))	? p += "x" : p += "-";
	(info.permission(QFile::ReadOther)) ? p += "r" : p += "-";
	(info.permission(QFile::WriteOther))? p += "w" : p += "-";
	(info.permission(QFile::ExeOther))	? p += "x" : p += "-";
	values << p;
	keys << tr("Size:");
	if(info.size() > 1024)
		values << QString("%1kB").arg(info.size() / 1024);
	else
		values << QString("%1B").arg(info.size());
	keys << tr("MimeType:");
	QMimeType mimeType(info.absoluteFilePath());
	values << mimeType.id();
	keys << tr("Created:");
	values << info.created().toString(Qt::LocalDate);
	keys << tr("Last modified:");
	values << info.lastModified().toString(Qt::LocalDate);
	keys << tr("Last read:");
	values << info.lastRead().toString(Qt::LocalDate);

	for(int i = 0; i < keys.count(); i++) {
		QString key = keys[i];
		QString value = values[i];
		html += QString("<tr><th align=\"right\">%1</th><td align=\"left\">%2</td></tr>").arg(key, value);
	}

	html += "</table>";
	document()->setHtml(html);
}
