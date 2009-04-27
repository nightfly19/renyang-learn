METASOURCES = AUTO
pluginsdir = $(pkgdatadir)/plugins
plugins_LTLIBRARIES = libaddressbook.la
libaddressbook_la_LDFLAGS = -avoid-version -module
libaddressbook_la_SOURCES = addressbookwidget.ui editdialog.ui addressbookplugin.cpp addressbook.cpp listview.cpp image_collection.cpp

image_collection.cpp: \
		images/delete.png \
		images/edit.png \
		images/save.png \
		images/open.png \
		images/a.png \
		images/d.png \
		images/g.png \
		images/j.png \
		images/m.png \
		images/p.png \
		images/s.png \
		images/v.png \
		images/defaultPicture.png \
		images/nonePicture.png \
		images/addressbook.png \
		images/incoming.png \
		images/outcoming.png \
		images/search.png \
		images/new.png
	$(UIC)  -embed addressbook -o $@ $(srcdir)/images/delete.png $(srcdir)/images/edit.png $(srcdir)/images/save.png $(srcdir)/images/open.png $(srcdir)/images/a.png $(srcdir)/images/d.png $(srcdir)/images/g.png $(srcdir)/images/j.png $(srcdir)/images/m.png $(srcdir)/images/p.png $(srcdir)/images/s.png $(srcdir)/images/v.png $(srcdir)/images/defaultPicture.png $(srcdir)/images/nonePicture.png $(srcdir)/images/addressbook.png $(srcdir)/images/incoming.png $(srcdir)/images/outcoming.png $(srcdir)/images/search.png $(srcdir)/images/new.png

LIBRARY_MAKEFILE:=1

include $(OPENH323DIR)/openh323u.mak

# set the include path found by configure
INCLUDES = -I../../libcore $(all_includes)

KDE_OPTIONS = qtonly
AM_CXXFLAGS = $(STDCCFLAGS) -I$(PWLIBDIR)/include

DISTCLEANFILES = image_collection.cpp

picturesdir = $(pkgdatadir)/pictures/addressbook
pictures_DATA = pictures/Apple.png pictures/Arch.png pictures/Astronaut.png \
	pictures/BeachBall.png pictures/BeachBall2.png \
	pictures/Blowfish.png pictures/BlueMarble1.png pictures/Burger.png \
	pictures/Butterfly.png pictures/Butterfly2.png pictures/Cactus.png \
	pictures/Car.png pictures/Car2.png pictures/Cow.png pictures/Daemon.png \
	pictures/Dice.png pictures/Dog.png pictures/Dog2.png \
	pictures/Dragon.png pictures/Elephant.png pictures/Flower.png \
	pictures/Flower2.png pictures/Flower3.png pictures/Frog.png \
	pictures/Ghost.png pictures/Guitar.png pictures/Heart.png \
	pictures/Cheese.png pictures/Konqui.png pictures/Lava.png \
	pictures/Lion.png pictures/Man.png pictures/Monkey.png \
	pictures/Night.png pictures/Pearl.png pictures/Pig.png \
	pictures/Pineapple.png pictures/Rabbit.png pictures/Ring.png \
	pictures/Scream.png pictures/Shark.png pictures/Skull.png \
	pictures/Speakers.png pictures/Splash.png pictures/Star.png \
	pictures/Stars.png pictures/Star3.png pictures/Star4.png \
	pictures/Teddybear.png pictures/Tiger.png pictures/Tukan.png \
	pictures/Turtle.png pictures/UFO.png pictures/Watch.png \
	pictures/Wind.png
