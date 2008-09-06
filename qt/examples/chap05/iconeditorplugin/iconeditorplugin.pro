TEMPLATE      = lib
CONFIG       += plugin
HEADERS       = ../iconeditor/iconeditor.h
SOURCES       = iconeditorplugin.cpp \
                ../iconeditor/iconeditor.cpp
IMAGES        = images/iconeditor.png
DESTDIR       = $(QTDIR)/plugins/designer
