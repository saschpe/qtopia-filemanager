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

#include "inputdialog.h"

#include <QVBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>

InputDialog::InputDialog(QWidget *parent)
	: QDialog(parent)
{
	m_edit = new QLineEdit(this);
	m_label = new QLabel(this);
	m_label->setVisible(false);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(m_label);
	layout->addWidget(m_edit);
	setLayout(layout);
	setFocusProxy(m_edit);
}

void InputDialog::setText(const QString &text)
{
	m_edit->setText(text);
}

QString InputDialog::text() const
{
	return m_edit->text();
}

void InputDialog::setLabel(const QString &text)
{
	if(!text.isEmpty()) {
		m_label->setText(text);
		m_label->setVisible(true);
	} else {
		m_label->setVisible(false);
	}
}

QString InputDialog::label() const
{
	return m_label->text();
}

void InputDialog::keyPressEvent(QKeyEvent *event)
{
	if(event->key() == Qt::Key_Select) {
		accept();
	} else {
		QDialog::keyPressEvent(event);
	}
}
