
TEMPLATE = app
INCLUDEPATH += . ./common ./security ./sctp
TARGET = Dr.ta

CONFIG += release

#CXXFLAGS+=-fexceptions

#LIBS += -lasound -lgmpxx  -ljack
LIBS += -lasound -lgmpxx
LIBS += -lspeex
LIBS += -lSoundTouch
LIBS += -lsctp

LIBS += -lpthread

OBJECTS_DIR = .obj
MOC_DIR = .moc

include (common/common.pri)
include (security/security.pri)
include (sctp/sctp.pri)

# UI
INTERFACES += DrtaMW.ui\
	      dlg_sctp.ui

# Headers 
HEADERS += AudioPlayer.h PhoneSound.h\
	Receiver.h\
	Config.h\
	TcpServer.h\
	Transmitter.h\
	PacketHandler.h\
	Recorder.h\
	Packet.h\
	Logger.h\
	LogViewer.h\
	Settings.h\
	DrtaMW2.h

# Source
SOURCES += main.cpp\
	 AudioPlayer.cpp\
	Receiver.cpp\
	Config.cpp\
	TcpServer.cpp\
	Transmitter.cpp\
	PacketHandler.cpp\
	Recorder.cpp\
	Packet.cpp\
	Logger.cpp\
	LogViewer.cpp\
	Settings.cpp\
	Images.cpp\
	DrtaMW2.cpp\
	DrtaMW2_sctp_ext.cpp\

