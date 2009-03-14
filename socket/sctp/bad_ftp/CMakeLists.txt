# Project Name
PROJECT (sctp_ftp)
# Set the variable
SET ( client_SRCS client.c)
SET ( server_SRCS server.c)
# executable built from ${client_SRCS}
ADD_EXECUTABLE( client ${client_SRCS} )
ADD_EXECUTABLE( server ${server_SRCS} )
