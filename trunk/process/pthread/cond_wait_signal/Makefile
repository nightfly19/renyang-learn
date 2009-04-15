
CC	= gcc

LIBS	= -lpthread

OBJ	= pthread_cond_wait_signal.o

TARGET	= pthread_cond_wait_signal

.PHONY: everything all clean distclean

everything: $(TARGET)

all: distclean everything

clean:
	rm -fv $(OBJ)

distclean: clean
	rm -fv $(TARGET)

$(OBJ):pthread_cond_wait_signal.c
	$(CC) -c $< -o $@

$(TARGET):$(OBJ)
	$(CC) $(LIBS) $< -o $@
