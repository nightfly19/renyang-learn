TEMPLATE	= app
#TARGET		= car-sim
TARGET		= divsim
CONFIG		+= qt warn_on release
#DEPENDPATH	= ../../include

OBJECTS_DIR = .obj
MOC_DIR = .moc

REQUIRES	= 
#INCLUDEPATHi += ./network
#INCLUDEPATHi += ./event

INTERFACES += cmainwindow.ui\
		InfoViewer.ui\
		dlg/dlgcarsetting.ui\
		dlg/initial_set.ui\
		dlg/initial_set_dsrc.ui\
		dlg/dlgsimsetting.ui\
		dlg/settimeratio.ui

HEADERS		+= canvas.h \
		global.h\
		BouncyLogo.h\
		MainWindow.h\
		Coordinator.h\
		Car.h\
		InternetCar.h\
		WifiCar.h\
		DSRCCar.h\
		FigureEditor.h\
		Road.h\
		ImageItem.h\
		InfoViewerImp.h\
		InfoElt.h\
		pos.h

SOURCES		+= canvas.cpp main.cpp\
		global.cpp\
		BouncyLogo.cpp\
		MainWindow.cpp\
		Coordinator.cpp\
		Car.cpp\
		InternetCar.cpp\
		WifiCar.cpp\
		DSRCCar.cpp\
		FigureEditor.cpp\
		Road.cpp\
		ImageItem.cpp\
		InfoViewerImp.cpp\
		InfoElt.cpp\
		pos.cpp

IMAGES		+= car_g.png car_b.png car_r.png car_y.png

include(event/event.pro)
include(network/network.pro)

