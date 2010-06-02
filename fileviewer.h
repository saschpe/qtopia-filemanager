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

#ifndef FILEVIEWER_H
#define FILEVIEWER_H

class FileManager;
class QDirModel;	// redefined locally

#include <QWidget>
#include <QAbstractItemView>

class QTreeView;
class QListView;
class QAction;
class QActionGroup;
class QItemSelectionModel;
class QRegExp;
class QDir;
class QFileInfo;
class QKeyEvent;

class FileViewer : public QWidget
{
	Q_OBJECT
	
public:
	enum ViewType {
		ListView = 0x0,
		TreeView = 0x1,
		IconView = 0x2,
	};

	enum SortType {
		ByName = 0x0,
		ByTime = 0x1,
		BySize = 0x2,
		ByType = 0x3,
	};
	
	FileViewer(FileManager *parent = 0);
	
	FileViewer::ViewType currentViewType() const;
	void setCurrentViewType(ViewType type);

	bool hiddenFilesShown() const;
	void setHiddenFilesShown(bool enabled);
	
	bool sortOrderReversed() const;
	void setSortOrderReversed(bool reversed);

	FileViewer::SortType sorting() const;
	void setSorting(FileViewer::SortType type);
    
protected:
	bool eventFilter(QObject* obj, QEvent *event);

	QListView* listView();
	QTreeView* treeView();
	QListView* iconView();

private slots:
	void onSelect();
	void onMarkMode();
	void onShowInfo();
	void onSearch();

	void onNewFile();
	void onNewLink();
	void onNewFolder();

	void onRename();
	void onDelete();
	void onCopy();
	void onCut();
	void onPaste();

	void onShowHidden();
	void onListView();
	void onTreeView();
	void onIconView();
	void onShowSystemInfo();
	void onSort();
	void onQuit();

	void resizeColumn(const QModelIndex & index);
	
private:
	void setupUi();
	void createMenu();
	void createActions();
	void doBack();
	
	void enterMarkMode();
	void leaveMarkMode();
	void setSelectionMode(QAbstractItemView::SelectionMode mode);
	bool copyRecursive(const QModelIndexList& list, const QModelIndex& destination);
	bool copy(const QString& from, const QString& to);
	void remove(const QString& path);
	void recursiveFind(const QDir &dir, const QRegExp &pattern, QList<QFileInfo>* results);
	
	FileManager* m_manager;			// pointer to parent

    QListView* m_listView;			// Never use those views directly, use the corresponding
    QTreeView* m_treeView;			// private accessors or the public currentView()
	QAbstractItemView* m_currentView;

    QDirModel* m_dirModel;
	QItemSelectionModel* m_selectionModel;
	QModelIndexList m_clipBoardList;

    QAction* m_selectAction;
	QAction* m_markModeAction;	
	QAction* m_searchAction;
    QAction* m_newFileAction;
    QAction* m_newLinkAction;
	QAction* m_newFolderAction;
    QAction* m_renameAction;
    QAction* m_deleteAction;
    QAction* m_copyAction;
	QAction* m_cutAction;
	QAction* m_pasteAction;
	QAction* m_showHiddenAction;
	QAction* m_listViewAction;
	QAction* m_treeViewAction;
	QAction* m_iconViewAction;
	QAction* m_showInfoAction;
	QAction* m_showSystemInfoAction;
	QAction* m_showTaskInfoAction;
	QAction* m_reverseSortOrderAction;
	QAction* m_sortByNameAction;
	QAction* m_sortByTimeAction;
	QAction* m_sortBySizeAction;
	QAction* m_sortByTypeAction;
	QAction* m_exitAction;
	QActionGroup* m_viewActions;
	QActionGroup* m_sortActions;

	bool m_markMode;
	bool m_cut;
};

#endif /*FILEVIEWER_H*/
