
CC	= gcc

LIBS	= -lpthread

OBJ	= pthread_cond_wait_signal.o

TARGET	= pthread_cond_wait_signal

.PHONY: everything all clean clean_target

everything: $(TARGET)

all: clean_target everything

clean:
	rm -fv $(OBJ)

clean_target: clean
	rm -fv $(TARGET)

$(OBJ):pthread_cond_wait_signal.c
	$(CC) -c $< -o $@

$(TARGET):$(OBJ)
	$(CC) $(LIBS) $< -o $@
