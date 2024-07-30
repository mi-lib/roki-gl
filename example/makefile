INCLUDE=`roki-gl-config -I`
LIB=`roki-gl-config -L`
LINK=`roki-gl-config -l`
DEF=`roki-gl-config -D`

TARGET=$(shell ls *.c | xargs -i basename {} .c | tr -s "\n" " ")

CC=gcc
CFLAGS=-Wall -O3 $(LIB) $(INCLUDE) $(DEF) -funroll-loops

all: $(TARGET)
%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LINK)
clean :
	rm -f *.o *~ core *test $(TARGET)
