#############################################################################
# Makefile for building: i18n
# Generated by qmake (1.07a) (Qt 3.3.7) on: Fri Sep 19 10:52:19 2008
# Project:  i18n.pro
# Template: app
# Command: $(QMAKE) -o Makefile i18n.pro
#############################################################################

####### Compiler, tools and options

CC       = gcc
CXX      = g++
LEX      = flex
YACC     = yacc
CFLAGS   = -pipe -I/usr/include/mysql -I/usr/include/freetype2 -I/usr/include/postgresql -fno-exceptions -Wall -W -g -D_REENTRANT  -DQT_TABLET_SUPPORT -DQT_THREAD_SUPPORT -DQT_SHARED
CXXFLAGS = -pipe -I/usr/include/mysql -I/usr/include/freetype2 -I/usr/include/postgresql -fno-exceptions -Wall -W -g -D_REENTRANT  -DQT_TABLET_SUPPORT -DQT_THREAD_SUPPORT -DQT_SHARED
LEXFLAGS = 
YACCFLAGS= -d
INCPATH  = -I/usr/share/qt3/mkspecs/linux-g++ -I. -I/usr/include/freetype2 -I/usr/include/qt3 -I/usr/X11R6/include -I.moc/debug-shared-mt/
LINK     = g++
LFLAGS   = -fno-exceptions
LIBS     = $(SUBLIBS) -L/usr/share/qt3/lib -L/usr/X11R6/lib -lqt-mt -ljpeg -lpng -lz -lXi -lXrender -lXrandr -lXcursor -lXinerama -lXft -lfreetype -lfontconfig -lXext -lX11 -lm -lSM -lICE -ldl -lpthread
AR       = ar cqs
RANLIB   = 
MOC      = /usr/share/qt3/bin/moc
UIC      = /usr/share/qt3/bin/uic -L /usr/share/qt3/plugins
QMAKE    = /usr/share/qt3/bin/qmake
TAR      = tar -cf
GZIP     = gzip -9f
COPY     = cp -f
COPY_FILE= $(COPY)
COPY_DIR = $(COPY) -r
INSTALL_FILE= $(COPY_FILE)
INSTALL_DIR = $(COPY_DIR)
DEL_FILE = rm -f
SYMLINK  = ln -sf
DEL_DIR  = rmdir
MOVE     = mv -f
CHK_DIR_EXISTS= test -d
MKDIR    = mkdir -p

####### Output directory

OBJECTS_DIR = .obj/debug-shared-mt/

####### Files

HEADERS = mywidget.h
SOURCES = main.cpp \
		mywidget.cpp
OBJECTS = .obj/debug-shared-mt/main.o \
		.obj/debug-shared-mt/mywidget.o
FORMS = 
UICDECLS = 
UICIMPLS = 
SRCMOC   = .moc/debug-shared-mt/moc_mywidget.cpp
OBJMOC = .obj/debug-shared-mt/moc_mywidget.o
DIST	   = i18n.pro
QMAKE_TARGET = i18n
DESTDIR  = 
TARGET   = i18n

first: all
####### Implicit rules

.SUFFIXES: .c .o .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

####### Build rules

all: Makefile $(TARGET)

$(TARGET):  $(UICDECLS) $(OBJECTS) $(OBJMOC)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJMOC) $(OBJCOMP) $(LIBS)

mocables: $(SRCMOC)
uicables: $(UICDECLS) $(UICIMPLS)

$(MOC): 
	( cd /usr/share/qt3/src/moc && $(MAKE) )

Makefile: i18n.pro ../../.qmake.cache /usr/share/qt3/mkspecs/linux-g++/qmake.conf /usr/share/qt3/lib/libqt-mt.prl
	$(QMAKE) -o Makefile i18n.pro
qmake: 
	@$(QMAKE) -o Makefile i18n.pro

dist: 
	@mkdir -p .obj/debug-shared-mt/i18n && $(COPY_FILE) --parents $(SOURCES) $(HEADERS) $(FORMS) $(DIST) .obj/debug-shared-mt/i18n/ && $(COPY_FILE) --parents mywidget_cs.ts mywidget_de.ts mywidget_el.ts mywidget_en.ts mywidget_eo.ts mywidget_fr.ts mywidget_it.ts mywidget_jp.ts mywidget_ko.ts mywidget_no.ts mywidget_ru.ts mywidget_zh.ts .obj/debug-shared-mt/i18n/ && ( cd `dirname .obj/debug-shared-mt/i18n` && $(TAR) i18n.tar i18n && $(GZIP) i18n.tar ) && $(MOVE) `dirname .obj/debug-shared-mt/i18n`/i18n.tar.gz . && $(DEL_FILE) -r .obj/debug-shared-mt/i18n

mocclean:
	-$(DEL_FILE) $(OBJMOC)
	-$(DEL_FILE) $(SRCMOC)

uiclean:

yaccclean:
lexclean:
clean: mocclean
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core $(TARGET)


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) $(TARGET)


FORCE:

####### Compile

.obj/debug-shared-mt/main.o: main.cpp mywidget.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/debug-shared-mt/main.o main.cpp

.obj/debug-shared-mt/mywidget.o: mywidget.cpp mywidget.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/debug-shared-mt/mywidget.o mywidget.cpp

.obj/debug-shared-mt/moc_mywidget.o: .moc/debug-shared-mt/moc_mywidget.cpp  mywidget.h 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/debug-shared-mt/moc_mywidget.o .moc/debug-shared-mt/moc_mywidget.cpp

.moc/debug-shared-mt/moc_mywidget.cpp: $(MOC) mywidget.h
	$(MOC) mywidget.h -o .moc/debug-shared-mt/moc_mywidget.cpp

####### Install

install:  

uninstall:  

