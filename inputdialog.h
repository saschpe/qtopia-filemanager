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

#ifndef INPUT_DIALOG_H
#define INPUT_DIALOG_H

#include <QDialog>

class QKeyEvent;
class QLabel;
class QLineEdit;

class InputDialog : public QDialog
{
	Q_OBJECT

public:
	InputDialog(QWidget *parent);

	void setText(const QString &text);
	QString text() const;
	void setLabel(const QString &text);
	QString label() const;

protected:
	void keyPressEvent(QKeyEvent *event);

private:
	QLabel* m_label;
	QLineEdit* m_edit;
};

#endif
