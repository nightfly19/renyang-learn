HEADERS     = stream_common.h scftp.h 
SOURCES     = stream_common.c scftp.cpp main.cpp
FORMS       = scftp.ui

LIBS 	+= -lsctp

# install
target.path = scftp
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS *.pro
sources.path = .
INSTALLS += target sources
