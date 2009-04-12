ifeq ($(CC),gcc)
	libs = "good"
else
	libs = "not good"
endif

ifneq ($(CC),gcc)
	lib = "bad"
else
	lib = "not bad"
endif

default:
	echo $(libs)
	echo $(lib)
	echo $(CC)
