include ../../config

DIR=../..

CONFIGTOOL=roki-gl-config
INCLUDE=-I$(DIR)/include `$(CONFIGTOOL) -I`
LIB=-L$(DIR)/lib `$(CONFIGTOOL) -L`
LINK=`$(CONFIGTOOL) -l`

ifeq ($(CONFIG_USE_GLUT),y)
	LINK += -lglut
endif

CC=gcc
CFLAGS=-ansi -Wall -O3 $(LIB) $(INCLUDE) -funroll-loops

%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LINK)
clean :
	rm -f *.o *~ core *test
