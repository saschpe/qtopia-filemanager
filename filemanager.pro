qtopia_project(qtopia app)
TARGET=filemanager
CONFIG+=qtopia_main no_quicklaunch no_singleexec

HEADERS+=\
	filemanager.h \
	fileviewer.h \
	infoscreen.h \
	inputdialog.h \
	qdirmodel.h

SOURCES+=main.cpp \
	filemanager.cpp \
	fileviewer.cpp \
	infoscreen.cpp \
	inputdialog.cpp \
	qdirmodel.cpp

desktop.files=filemanager.desktop
desktop.path=/apps/Applications
desktop.trtarget=filemanager-nct
desktop.hint=desktop 

pics.files=pics/*
pics.path=/pics/filemanager
pics.hint=pics

help.source=help
help.files=filemanager.html
help.hint=help

INSTALLS+=desktop pics help

pkg.name=filemanager
pkg.desc=A File Manager
pkg.version=1.2
pkg.maintainer=Trolltech (www.trolltech.com)
pkg.license=GPL
pkg.domain=trusted
