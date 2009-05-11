# Project Name
PROJECT(one-to-one)
# set source
SET(SRC_LIST clnt.c  serv.c)

# add library,只有當對每一個target均需要此library，才使用LINK_LIBRARY，否則請使用TARGET_LINK_LIBRARY指令
# LINK_LIBRARIES(sctp)

# define the executable
ADD_EXECUTABLE(serv serv.c)
# add library for each target
TARGET_LINK_LIBRARIES(serv sctp)

#define the executable
ADD_EXECUTABLE(clnt clnt.c)
#add library for each target
TARGET_LINK_LIBRARIES(clnt sctp)
