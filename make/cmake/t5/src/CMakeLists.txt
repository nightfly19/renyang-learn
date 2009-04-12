# the execuate file "main" will be generate by main.c
ADD_EXECUTABLE(main main.c)
# add the library to search the header file
INCLUDE_DIRECTORIES(/usr/include/hello)
# add the shared library
# TARGET_LINK_LIBRARIES(main libhello.so)
# add the static library
TARGET_LINK_LIBRARIES(main libhello.a)
