# Project Name
PROJECT(HELLO)
# Add subdirection
ADD_SUBDIRECTORY(src bin)
# Install files FILES and README to the <prefix>/share/doc/cmake/t3
INSTALL(FILES COPYRIGHT README DESTINATION share/doc/cmake/t3)
# Install scripts to the <prefix>/bin
INSTALL(PROGRAMS runhello.sh DESTINATION bin)
# Install all the files of the doc/ to the <prefix>/share/doc/cmake/t3
# don't include the doc direction
INSTALL(DIRECTORY doc/ DESTINATION share/doc/cmake/t3)
