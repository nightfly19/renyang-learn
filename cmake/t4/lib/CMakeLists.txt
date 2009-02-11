# set the ${LIBHELLO_SRC} to be hello.c
SET(LIBHELLO_SRC hello.c)
# build a dynanic library
ADD_LIBRARY(hello SHARED ${LIBHELLO_SRC})
# build a static library
ADD_LIBRARY(hello_static STATIC ${LIBHELLO_SRC})
# remove the name of static dynanic library
SET_TARGET_PROPERTIES(hello_static PROPERTIES OUTPUT_NAME hello)
# don't clean the same name, when build the library
SET_TARGET_PROPERTIES(hello PROPERTIES CLEAN_DIRECT_OUTPUT 1)
SET_TARGET_PROPERTIES(hello_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
# set the number of version
SET_TARGET_PROPERTIES(hello PROPERTIES VERSION 1.2 SOVERSION 1)
# install the library to the system
# the dymanic library put into lib
# the static library put into lib
INSTALL(TARGETS hello hello_static
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)
# install the header to the system
INSTALL(FILES hello.h DESTINATION include/hello)
