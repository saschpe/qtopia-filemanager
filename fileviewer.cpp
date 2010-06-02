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

#include "fileviewer.h"
#include "filemanager.h"
#include "inputdialog.h"
#include "infoscreen.h"

#include "qdirmodel.h"

#include <QtopiaApplication>
#include <QSoftMenuBar>
#include <QMenu>
#include <QLayout>
#include <QTreeView>
#include <QListView>
#include <QAction>
#include <QActionGroup>
#include <QItemSelectionModel>
#include <QFileInfo>
#include <QMimeType>
#include <QtopiaIpcEnvelope>
#include <QRegExp>
#include <QKeyEvent>

FileViewer::FileViewer(FileManager *parent)
	: QWidget(parent), m_manager(parent)
	, m_listView(NULL)
	, m_treeView(NULL)
	, m_currentView(NULL)
	, m_markMode(false)
{
	m_dirModel = new QDirModel();
	m_dirModel->setFilter(QDir::AllEntries);
	m_selectionModel = new QItemSelectionModel(m_dirModel);
	m_selectionModel->setCurrentIndex(m_dirModel->index(0, 0), QItemSelectionModel::Select);

	setupUi();
	createActions();
	createMenu();
}

QListView* FileViewer::listView()
{
	if(!m_listView) {
        m_listView = new QListView(this);
		m_listView->setModel(m_dirModel);
		m_listView->setSelectionModel(m_selectionModel);
		m_listView->setIconSize(QSize(16, 16));
		m_listView->setVisible(false);
		m_listView->installEventFilter(this);
		m_listView->setRootIndex(m_dirModel->index(QDir::home().absolutePath()));
		
		connect(m_listView, SIGNAL(activated(const QModelIndex &)), this, SLOT(onSelect()));

		layout()->addWidget(m_listView);
    }

	m_listView->setGridSize(QSize());
	m_listView->setViewMode(QListView::ListMode);
	m_listView->setAlternatingRowColors(true);

    return m_listView;
}

QTreeView* FileViewer::treeView()
{
	if(!m_treeView) {
		m_treeView = new QTreeView(this);
		m_treeView->setModel(m_dirModel);
		m_treeView->setSelectionModel(m_selectionModel);
		m_treeView->setAlternatingRowColors(true);
		m_treeView->setVisible(false);
		m_treeView->installEventFilter(this);

		connect(m_treeView, SIGNAL(activated(const QModelIndex &)), this, SLOT(onSelect()));
		connect(m_treeView, SIGNAL(collapsed(const QModelIndex &)), this, SLOT(resizeColumn(const QModelIndex &)));
		connect(m_treeView, SIGNAL(expanded(const QModelIndex &)), this, SLOT(resizeColumn(const QModelIndex &)));
	
		layout()->addWidget(m_treeView);
	}
	return m_treeView;
}

QListView* FileViewer::iconView()
{
	if(!m_listView) {
        m_listView = new QListView(this);
		m_listView->setModel(m_dirModel);
		m_listView->setSelectionModel(m_selectionModel);
		m_listView->setIconSize(QSize(16, 16));
		m_listView->setVisible(false);
		m_listView->installEventFilter(this);
		m_listView->setRootIndex(m_dirModel->index(QDir::home().absolutePath()));
	
		connect(m_listView, SIGNAL(activated(const QModelIndex &)), this, SLOT(onSelect()));

		layout()->addWidget(m_listView);
    }
	
	m_listView->setGridSize(QSize(64, 48));
	m_listView->setViewMode(QListView::IconMode);
	m_listView->setAlternatingRowColors(false);

    return m_listView;
}

bool FileViewer::eventFilter(QObject* obj, QEvent *event)
{
	if(obj == m_listView || obj == m_treeView)
	{
		if(event->type() == QEvent::KeyPress)
		{
			QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
			if(keyEvent->key()==Qt::Key_Back)
			{
				doBack();
				return true;
			}
			else
				return false;
		}
		else
			return false;
	}
	else
		return QWidget::eventFilter(obj,event);
}

void FileViewer::setupUi()
{
	setLayout(new QHBoxLayout());
}

FileViewer::ViewType FileViewer::currentViewType() const
{
	if(m_currentView == m_listView) {
		if(m_listView->viewMode() == QListView::ListMode)
			return ListView;
		else if(m_listView->viewMode() == QListView::IconMode)
			return IconView;
		else
			return ListView;
	} else if(m_currentView == m_treeView) {
		return TreeView;
	} else {
		return ListView;	// Default view
	}
}

void FileViewer::setCurrentViewType(ViewType type)
{
	if(m_currentView)
		m_currentView->setVisible(false);
	if(m_currentView == m_treeView && m_currentView != 0)
		m_dirModel->setFilter(m_dirModel->filter() ^ QDir::NoDotAndDotDot);
	if(type == ListView) {
		m_currentView = listView();
		m_listViewAction->setChecked(true);
	} else if(type == TreeView) {
		m_currentView = treeView();
		m_treeViewAction->setChecked(true);
		m_dirModel->setFilter(m_dirModel->filter() | QDir::NoDotAndDotDot);
	} else if(type == IconView) {
		m_currentView = iconView();
		m_iconViewAction->setChecked(true);
	}
	m_currentView->setVisible(true);
	m_currentView->setFocus();
}

bool FileViewer::hiddenFilesShown() const
{
	return m_showHiddenAction->isChecked();
}

void FileViewer::setHiddenFilesShown(bool enabled)
{
	m_showHiddenAction->setChecked(enabled);
	onShowHidden();
}

bool FileViewer::sortOrderReversed() const
{
	return m_reverseSortOrderAction->isChecked();
}

void FileViewer::setSortOrderReversed(bool reversed)
{
	m_reverseSortOrderAction->setChecked(reversed);
	onSort();
}

FileViewer::SortType FileViewer::sorting() const
{
	if(m_sortByNameAction->isChecked())
		return ByName;
	else if(m_sortByTimeAction->isChecked())
		return ByTime;
	else if(m_sortBySizeAction->isChecked())
		return BySize;
	else if(m_sortByTypeAction->isChecked())
		return ByType;
	else {
		m_sortByNameAction->setChecked(true);
		return ByName;
	}
}

void FileViewer::setSorting(FileViewer::SortType type)
{
	if(type == ByName)
		m_sortByNameAction->setChecked(true);
	else if(type == ByTime)
		m_sortByTimeAction->setChecked(true);
	else if(type == BySize)
		m_sortBySizeAction->setChecked(true);
	else if(type == ByType)
		m_sortByTypeAction->setChecked(true);
	onSort();
}

void FileViewer::createActions()
{
	m_selectAction = new QAction(tr("Select"), this);
	connect(m_selectAction, SIGNAL(triggered()), this, SLOT(onSelect()));
	m_showInfoAction = new QAction(tr("File info"), this);
	connect(m_showInfoAction, SIGNAL(triggered()), this, SLOT(onShowInfo()));
	m_markModeAction = new QAction(tr("Mark mode"), this);
	m_markModeAction->setCheckable(true);
	connect(m_markModeAction, SIGNAL(triggered()), this, SLOT(onMarkMode()));
	m_searchAction = new QAction(tr("Search"), this);
	connect(m_searchAction, SIGNAL(triggered()), this, SLOT(onSearch()));
	
	m_newFileAction = new QAction(tr("New file"), this);
	connect(m_newFileAction, SIGNAL(triggered()), this, SLOT(onNewFile()));
	m_newLinkAction = new QAction(tr("New link"), this);
	connect(m_newLinkAction, SIGNAL(triggered()), this, SLOT(onNewLink()));
	m_newFolderAction = new QAction(tr("New folder"), this);
	connect(m_newFolderAction, SIGNAL(triggered()), this, SLOT(onNewFolder()));
	m_renameAction = new QAction(tr("Rename"), this);
	connect(m_renameAction, SIGNAL(triggered()), this, SLOT(onRename()));
	m_deleteAction = new QAction(tr("Delete"), this);
	connect(m_deleteAction, SIGNAL(triggered()), this, SLOT(onDelete()));

	m_copyAction = new QAction(tr("Copy"), this);
	connect(m_copyAction, SIGNAL(triggered()), this, SLOT(onCopy()));
	m_cutAction = new QAction(tr("Cut"), this);
	connect(m_cutAction, SIGNAL(triggered()), this, SLOT(onCut()));
	m_pasteAction = new QAction(tr("Paste"), this);
	connect(m_pasteAction, SIGNAL(triggered()), this, SLOT(onPaste()));

	m_showHiddenAction = new QAction(tr("Show all files"), this);
	m_showHiddenAction->setCheckable(true);
	connect(m_showHiddenAction, SIGNAL(triggered()), this, SLOT(onShowHidden()));

	m_listViewAction = new QAction(tr("List view"), this);
	m_listViewAction->setCheckable(true);
	connect(m_listViewAction, SIGNAL(triggered()), this, SLOT(onListView()));
	m_treeViewAction = new QAction(tr("Tree view"), this);
	m_treeViewAction->setCheckable(true);
	connect(m_treeViewAction, SIGNAL(triggered()), this, SLOT(onTreeView()));
	m_iconViewAction = new QAction(tr("Icon view"), this);
	m_iconViewAction->setCheckable(true);
	connect(m_iconViewAction, SIGNAL(triggered()), this, SLOT(onIconView()));
	m_viewActions = new QActionGroup(this);
	m_viewActions->addAction(m_listViewAction);
	m_viewActions->addAction(m_treeViewAction);
	m_viewActions->addAction(m_iconViewAction);

	m_showSystemInfoAction = new QAction(tr("System info"), this);
	connect(m_showSystemInfoAction, SIGNAL(triggered()), this, SLOT(onShowSystemInfo()));

	m_reverseSortOrderAction = new QAction(tr("Reversed"), this);
	m_reverseSortOrderAction->setCheckable(true);
	connect(m_reverseSortOrderAction, SIGNAL(triggered()), this, SLOT(onSort()));

	m_sortByNameAction = new QAction(tr("By name"), this);
	m_sortByNameAction->setCheckable(true);
	connect(m_sortByNameAction, SIGNAL(triggered()), this, SLOT(onSort()));
	m_sortByTimeAction = new QAction(tr("By time"), this);
	m_sortByTimeAction->setCheckable(true);
	connect(m_sortByTimeAction, SIGNAL(triggered()), this, SLOT(onSort()));
	m_sortBySizeAction = new QAction(tr("By size"), this);
	m_sortBySizeAction->setCheckable(true);
	connect(m_sortByNameAction, SIGNAL(triggered()), this, SLOT(onSort()));
	m_sortByTypeAction = new QAction(tr("By type"), this);
	m_sortByTypeAction->setCheckable(true);
	connect(m_sortByTypeAction, SIGNAL(triggered()), this, SLOT(onSort()));

	m_sortActions = new QActionGroup(this);
	m_sortActions->addAction(m_sortByNameAction);
	m_sortActions->addAction(m_sortByTimeAction);
	m_sortActions->addAction(m_sortBySizeAction);
	m_sortActions->addAction(m_sortByTypeAction);

	m_exitAction = new QAction(tr("Exit"), this);
	connect(m_exitAction, SIGNAL(triggered()), this, SLOT(onQuit()));
}

void FileViewer::createMenu()
{
	QMenu* menu = QSoftMenuBar::menuFor(this);

	menu->addAction(m_selectAction);
	menu->addAction(m_showInfoAction);
	menu->addAction(m_markModeAction);
	menu->addAction(m_searchAction);
	menu->addSeparator();

	QMenu* addMenu = new QMenu(tr("Add"), this);
	addMenu->addAction(m_newFileAction);
	addMenu->addAction(m_newLinkAction);
	addMenu->addAction(m_newFolderAction);
	menu->addMenu(addMenu);

	QMenu* editMenu = new QMenu(tr("Edit"), this);
	editMenu->addAction(m_copyAction);
	editMenu->addAction(m_cutAction);
	editMenu->addAction(m_pasteAction);
	editMenu->addSeparator();
	editMenu->addAction(m_renameAction);
	editMenu->addAction(m_deleteAction);
	menu->addMenu(editMenu);

	QMenu* viewMenu = new QMenu(tr("View"), this);
	viewMenu->addAction(m_showHiddenAction);
	viewMenu->addSeparator();
	viewMenu->addAction(m_listViewAction);
	viewMenu->addAction(m_iconViewAction);
	viewMenu->addAction(m_treeViewAction);
	viewMenu->addSeparator();

	QMenu* sortMenu = new QMenu(tr("Sort"), this);
	sortMenu->addAction(m_reverseSortOrderAction);
	sortMenu->addSeparator();
	sortMenu->addAction(m_sortByNameAction);
	sortMenu->addAction(m_sortBySizeAction);
	sortMenu->addAction(m_sortByTypeAction);
	sortMenu->addAction(m_sortByTimeAction);
	
	viewMenu->addMenu(sortMenu);
	viewMenu->addSeparator();
	viewMenu->addAction(m_showSystemInfoAction);

	menu->addMenu(viewMenu);
	menu->addSeparator();
	menu->addAction(m_exitAction);
}

void FileViewer::resizeColumn(const QModelIndex & index)
{
	if(m_treeView)
		m_treeView->resizeColumnToContents(index.column());
}

void FileViewer::doBack()
{
	if(m_currentView==m_listView)
	{
		QString path = m_dirModel->fileInfo(m_listView->rootIndex()).absolutePath();
		m_currentView->setRootIndex(m_dirModel->index(path));
	}
	else
	{
		QString path = m_dirModel->fileInfo(m_selectionModel->currentIndex()).absolutePath();
		m_selectionModel->select(m_dirModel->index(path), QItemSelectionModel::SelectCurrent);
		m_selectionModel->setCurrentIndex(m_dirModel->index(path), QItemSelectionModel::SelectCurrent);
		m_treeView->collapse(m_dirModel->index(path));
	}
}

void FileViewer::enterMarkMode()
{
	m_markMode = true;
	setSelectionMode(QAbstractItemView::MultiSelection);
}

void FileViewer::leaveMarkMode()
{
	m_markMode = false;
	setSelectionMode(QAbstractItemView::SingleSelection);
	m_selectionModel->clearSelection();
	m_markModeAction->setChecked(false);
}

void FileViewer::setSelectionMode(QAbstractItemView::SelectionMode mode)
{
	if(m_listView)
		m_listView->setSelectionMode(mode);
	if(m_treeView)
		m_treeView->setSelectionMode(mode);
}

bool FileViewer::copy(const QString& from, const QString& to)
{
	bool success = QFile::copy(from, to);
	if(!success) {
		if(QFile(to).exists()) {
			if(QMessageBox::question(this, tr("Confirm overwrite"), tr("Really overwrite existing file(s)?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
				if(!QFile::remove(to))
					QMessageBox::critical(this, tr("Overwrite failed"), tr("Overwrite file(s) failed"));
				success = QFile::copy(from, to);
			}
		}
	}

	return success;
}

bool FileViewer::copyRecursive(const QModelIndexList& list, const QModelIndex& destination)
{
	bool retval = true;

	foreach(QModelIndex index, list)	{
		if(m_dirModel->isDir(index)) {
			QString dirname = m_dirModel->fileInfo(index).fileName();
			QDir olddir = QDir(m_dirModel->fileInfo(destination).absoluteFilePath());
			olddir.mkdir(dirname);
			
			QModelIndex newdir = m_dirModel->index(olddir.absoluteFilePath(dirname));

			QModelIndexList list;
			QDir dir = QDir(m_dirModel->fileInfo(index).absoluteFilePath());
			QStringList entries = dir.entryList(QDir::AllEntries | QDir::Hidden | QDir::System | QDir::NoDotAndDotDot);
			foreach(QString entry, entries)	{
				QModelIndex tmp = m_dirModel->index(dir.absoluteFilePath(entry));
				if(tmp.isValid())
					list.append(tmp);
			}
			retval = retval && copyRecursive(list, newdir);
		}
		else {
			QString from = m_dirModel->fileInfo(index).absoluteFilePath();
			QString to = m_dirModel->fileInfo(destination).absoluteFilePath() + "/" + m_dirModel->fileInfo(index).fileName();
	    	retval = retval && copy(from, to);
		}
	}

	return retval;
}

void FileViewer::remove(const QString& path)
{
	if(QFileInfo(path).isDir())
	{
		QStringList entries = QDir(path).entryList(QDir::AllEntries | QDir::Hidden | QDir::System | QDir::NoDotAndDotDot);
		foreach(QString entry, entries)
			remove(path+"/"+ entry);

		QFileInfo(path).dir().rmdir(QDir(path).dirName());
	}
	else {
		if(QFile(path).exists()) 
			if(!QFile::remove(path))
				QMessageBox::critical(this, tr("Delete failed"), tr("Delete failed: %1").arg(path));
	}
}

/*
 * Lots of slots to follow
 */

void FileViewer::onSelect()
{
	QModelIndex index = m_selectionModel->currentIndex();

	if(!m_markMode)	{
		QFileInfo fileInfo = m_dirModel->fileInfo(index);

		if(fileInfo.isDir()) {
			if(m_listView) {
				m_listView->setRootIndex(index);
				m_selectionModel->clear();
			} else if(m_treeView) {
				m_treeView->expand(index);
			}
		} else {
			QMimeType mimeType(fileInfo.absoluteFilePath());
			QContent defaultApp = mimeType.application();

			if(defaultApp.isValid()) {
				QStringList args;
				args.append(fileInfo.absoluteFilePath());
				defaultApp.execute(static_cast<QStringList &>(args));	
			} else {
				if(QMessageBox::question(this, tr("No default application"), tr("Open with text editor?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
					QMimeType textMime("text/plain");
					QContent textEditor = textMime.application();
					QStringList args;
					args.append(fileInfo.absoluteFilePath());
					textEditor.execute(static_cast<QStringList &>(args));
				}
			}
		}
	}
}

void FileViewer::onMarkMode()
{
	if(m_markMode)
		leaveMarkMode();
	else
		enterMarkMode();
}

void FileViewer::recursiveFind(const QDir &dir, const QRegExp &pattern, QList<QFileInfo>* results)
{
	foreach(QFileInfo child, dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot)) {
		if(pattern.exactMatch(child.fileName())) {
			results->append(child.absoluteFilePath());
		}
		if(child.isDir()) {
			QDir childDir = QDir(child.absoluteFilePath());
			recursiveFind(childDir, pattern, results);
		}
	}
}

void FileViewer::onSearch()
{
	InputDialog* dlg = new InputDialog(this);
	dlg->setWindowTitle("Search for:");
	dlg->setLabel(tr("Accepts wildcards (*, ?)"));
	if(QtopiaApplication::execDialog(dlg) == QDialog::Accepted) {
		QString search = dlg->text();
		if(search.isEmpty())
			return;
	
		QRegExp wildcardPattern(search);
		wildcardPattern.setPatternSyntax(QRegExp::Wildcard);

		QModelIndexList indexList = m_selectionModel->selectedIndexes();
		if(indexList.isEmpty()) {
			QModelIndex index = m_selectionModel->currentIndex();
			if(!m_dirModel->isDir(index)) {
				if(m_currentView == m_listView)
					index = m_listView->rootIndex();
				else
				{
					index = index.parent();
				}
			}
			indexList.append(index);
		}

		QList<QFileInfo> resultList;
		foreach(QModelIndex index, indexList) {
			if(m_dirModel->isDir(index)) {
				QFileInfo info = m_dirModel->fileInfo(index);
				QDir dir = QDir(info.absoluteFilePath());
				recursiveFind(dir, wildcardPattern, &resultList);
			}
		}

		// TODO: not the best solution, maybe refine it a bit
		InfoScreen *infoScreen = m_manager->infoScreen();
		infoScreen->setSearchResult(resultList);
		m_manager->setCurrentWidget(infoScreen);
	}
	delete dlg;
	leaveMarkMode();
}

void FileViewer::onShowInfo()
{
	if(m_selectionModel->hasSelection()) {
		QList<QModelIndex> indexList = m_selectionModel->selectedIndexes();
		QList<QModelIndex> newIndexList;
		foreach(QModelIndex index, indexList) {
			if(index.column() == 0)
				newIndexList.append(index);
		}
		if(newIndexList.size() == 1) {
			QFileInfo info = m_dirModel->fileInfo(newIndexList.at(0));

			// TODO: not the best solution, maybe refine it a bit
			InfoScreen *infoScreen = m_manager->infoScreen();
			infoScreen->setFileInfo(info);
			m_manager->setCurrentWidget(infoScreen);
		} else {
			QMessageBox::warning(this, tr("To many items"), tr("To many items selected"));
		}
	} else {
		QMessageBox::warning(this, tr("No selection"), tr("No selection"));
	}
}

void FileViewer::onNewFile()
{
	leaveMarkMode();

	InputDialog *dlg = new InputDialog(this);
	dlg->setWindowTitle(tr("File name:"));
	if(QtopiaApplication::execDialog(dlg) == QDialog::Accepted) {
		QString name = dlg->text();
		if(name.isEmpty())
			return;
		
		QModelIndex index = m_selectionModel->currentIndex();
		if(!m_dirModel->isDir(index)) {
			if(m_currentView == m_listView) {
				index = m_listView->rootIndex();
			} else {
				index = index.parent();
			}
		}
		QFileInfo info = m_dirModel->fileInfo(index);

		name = info.absoluteFilePath() + "/" + name;
		if(!QFile(name).exists()) {
			if(!QFile(name).open(QIODevice::ReadWrite)) {
				QMessageBox::critical(this, tr("File creation failed"), tr("File creation failed"));
			} else {
				QFile(name).close();
				m_dirModel->refresh();
			}
		} else {
			QMessageBox::warning(this, tr("File already exists"), tr("File already exists"));
		}
	}
	delete dlg;
}

void FileViewer::onNewLink()
{
	leaveMarkMode();

	QString name;
	InputDialog *dlg = new InputDialog(this);
	dlg->setWindowTitle(tr("Link name:"));
	if(QtopiaApplication::execDialog(dlg) == QDialog::Accepted) {
		name = dlg->text();
		if(name.isEmpty())
			return;
	}
	dlg->setWindowTitle(tr("Link target:"));
	dlg->setLabel(tr("Enter absolute path"));
	if(QtopiaApplication::execDialog(dlg) == QDialog::Accepted) {
		QString target = dlg->text();
		if(target.isEmpty())
			return;

		QModelIndex index = m_selectionModel->currentIndex();
		if(!m_dirModel->isDir(index))
		{
			if(m_currentView == m_listView)
				index= m_listView->rootIndex();
			else
			{
				index= index.parent();
			}
		}
		QFileInfo info = m_dirModel->fileInfo(index);

		name = info.absoluteFilePath() + "/" + name;
		if(!QFile(name).exists()) {
			if(!QFile(target).link(name)) {
				QMessageBox::critical(this, tr("File creation failed"), tr("File creation failed"));
			} else {
				m_dirModel->refresh();
			}
		} else {
			QMessageBox::warning(this, tr("File already exists"), tr("File already exists"));
		}
	}
	delete dlg;
}

void FileViewer::onNewFolder()
{
	leaveMarkMode();

	InputDialog* dlg = new InputDialog(this);
	dlg->setWindowTitle(tr("Folder name:"));
	if(QtopiaApplication::execDialog(dlg) == QDialog::Accepted) {
		QString name = dlg->text();
		if(name.isEmpty())
			return;
		
		QModelIndex index = m_selectionModel->currentIndex();
		if(!m_dirModel->isDir(index))
		{
			if(m_currentView == m_listView)
				index= m_listView->rootIndex();
			else
			{
				index= index.parent();
			}
		}
		QFileInfo info = m_dirModel->fileInfo(index);
		QString folder = info.absoluteFilePath() + "/" + name;
	
		if(!QFile(folder).exists()) {
			if(!QDir(info.absoluteFilePath()).mkdir(name))
				QMessageBox::critical(this, tr("Folder creation failed"), tr("Folder creation failed"));
			else
				m_dirModel->refresh();
		} else {
			QMessageBox::warning(this, tr("Folder already exists"), tr("Folder already exists"));
		}
	}
	delete dlg;
}

void FileViewer::onRename()
{
	InputDialog* dlg = new InputDialog(this);
	dlg->setWindowTitle("Rename to:");
	QModelIndexList list = m_selectionModel->selectedIndexes();

	foreach(QModelIndex index, list) {
		QFileInfo info = m_dirModel->fileInfo(index);
		dlg->setText(info.fileName());
		if(QtopiaApplication::execDialog(dlg) == QDialog::Accepted) {
			QString newName = dlg->text();
			if(newName.isEmpty())
				return;
			newName = info.absolutePath() + "/" + newName;
			if(info.absoluteFilePath() != newName)
				if(!QFile(info.absoluteFilePath()).rename(newName))
					QMessageBox::critical(this, tr("Rename failed"), tr("Rename failed"));
		}
	}
	delete dlg;
	leaveMarkMode();
	m_dirModel->refresh();
}

void FileViewer::onDelete()
{
	if(QMessageBox::question(this, tr("Confirm deletion"), tr("Really delete current selection?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes){
		QModelIndexList list = m_selectionModel->selectedIndexes();
		foreach(QModelIndex index, list) {
			if(index.column()==0)
			{
				QFileInfo info = m_dirModel->fileInfo(index);
				remove(info.absoluteFilePath());
			}
		}
		leaveMarkMode();
		m_dirModel->refresh();
	}
}

void FileViewer::onCopy()
{
	m_cut = false;
	m_clipBoardList.clear();
	QModelIndexList list = m_selectionModel->selectedIndexes();
	foreach(QModelIndex index, list) {
		if(index.column()==0)
			m_clipBoardList.append(index);
	}
	leaveMarkMode();
}

void FileViewer::onCut()
{
	onCopy();
	m_cut=true;
}

void FileViewer::onPaste()
{
	if(!m_markMode) {
		if(m_clipBoardList.count())	{
			QModelIndex destination= m_selectionModel->currentIndex();
			if(!m_dirModel->isDir(destination))
			{
				if(m_currentView == m_listView)
					destination= m_listView->rootIndex();
				else
				{
					destination= destination.parent();
				}
			}
			if(copyRecursive(m_clipBoardList,destination) && m_cut)
		    {
				foreach(QModelIndex index,m_clipBoardList)
				{
					remove(m_dirModel->fileInfo(index).absoluteFilePath());
				}
			}
			m_dirModel->refresh();
		}
	}
}

void FileViewer::onShowHidden()
{
	if(m_showHiddenAction->isChecked()) {
		m_dirModel->setFilter(m_dirModel->filter() | QDir::Hidden | QDir::System);
	} else if(m_dirModel->filter().testFlag(QDir::Hidden)){
		m_dirModel->setFilter(m_dirModel->filter() ^ QDir::Hidden ^ QDir::System);
	}
}

void FileViewer::onListView()
{
	setCurrentViewType(ListView);
}

void FileViewer::onTreeView()
{
	setCurrentViewType(TreeView);
}

void FileViewer::onIconView()
{
	setCurrentViewType(IconView);
}

void FileViewer::onShowSystemInfo()
{
	QtopiaIpcEnvelope e("QPE/Application/sysinfo", "raise()");
}

void FileViewer::onSort()
{
	QDir::SortFlags flags;
	if(m_reverseSortOrderAction->isChecked())
		flags |= QDir::Reversed;

	if(m_sortByNameAction->isChecked())
		flags |= QDir::Name;
	else if(m_sortByTimeAction->isChecked())
		flags |= QDir::Time;
	else if(m_sortBySizeAction->isChecked())
		flags |= QDir::Size;
	else if(m_sortByTypeAction->isChecked())
		flags |= QDir::Type;

	m_dirModel->setSorting(flags);
}

void FileViewer::onQuit()
{
	QtopiaApplication::quit();
}
