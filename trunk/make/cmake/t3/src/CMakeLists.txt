# execuate by the main.c
ADD_EXECUTABLE(hello main.c)
# set the install value
INSTALL(TARGETS hello
	RUNTIME DESTINATION bin
	)
