# Project Name
PROJECT(HELLO)
# Set the variable value
SET(SRC_LIST main.c)
# send message
MESSAGE(STATUS "This is BINARY dir " ${PROJECT_BINARY_DIR})
MESSAGE(STATUS "This is SOURCE dir "${PROJECT_SOURCE_DIR})
# the binary file deliever from ${SRC_LIST})
ADD_EXECUTABLE(hello ${SRC_LIST})
