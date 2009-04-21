## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = nostdinc subdir-objects

bin_PROGRAMS = ihu
ihu_SOURCES = \
	src/Config.cpp src/Config.h\
	src/Error.cpp src/Error.h\
	src/Logger.cpp src/Logger.h\
	src/FilePlayer.cpp src/FilePlayer.hpp\
	src/Receiver.cpp src/Receiver.hpp\
	src/TcpServer.cpp src/TcpServer.hpp\
	src/Transmitter.cpp src/Transmitter.hpp\
	src/Player.cpp src/Player.hpp\
	src/Packet.cpp src/Packet.h\
	src/PacketHandler.cpp src/PacketHandler.h\
	src/Recorder.cpp src/Recorder.hpp\
	src/Phone.cpp src/Phone.hpp\
	src/Call.cpp src/Call.hpp\
	src/Blowfish.cpp src/Blowfish.h\
	src/Rsa.cpp src/Rsa.h\
	src/Images.cpp src/ring_sounds.h\
	src/trayicon.cpp src/trayicon.hpp\
	src/Ihu2Spx.cpp src/Ihu2Spx.h\
	src/IhuNoGui.cpp src/IhuNoGui.hpp\
	src/Settings.cpp src/Settings.hpp\
	src/LogViewer.cpp src/LogViewer.hpp\
	src/PlayerDialog.cpp src/PlayerDialog.hpp\
	src/CallTab.cpp src/CallTab.hpp\
	src/Ihu.cpp src/Ihu.hpp\
	src/main.cpp
nodist_ihu_SOURCES = \
	src/moc_Receiver.cpp\
	src/moc_FilePlayer.cpp\
	src/moc_TcpServer.cpp\
	src/moc_Transmitter.cpp\
	src/moc_Player.cpp\
	src/moc_Recorder.cpp\
	src/moc_Phone.cpp\
	src/moc_Call.cpp\
	src/moc_trayicon.cpp\
	src/moc_IhuNoGui.cpp\
	src/moc_CallTab.cpp\
	src/moc_PlayerDialog.cpp\
	src/moc_Settings.cpp\
	src/moc_LogViewer.cpp\
	src/moc_Ihu.cpp

SUBDIRS = doc icon

ihu_CXXFLAGS = @QT_CXXFLAGS@ -Wall
ihu_LDADD = @QT_LIBS@ @OGG_LIBS@ -lSoundTouch

moc_%.cpp: %.hpp
	$(QT_MOC) $< -o $@

AM_CPPFLAGS = -DPKGDATADIR=\"$(pkgdatadir)\"
