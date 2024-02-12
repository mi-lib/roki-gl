INCLUDE=`roki-gl-config -I`
LIB=`roki-gl-config -L`
LINK=`roki-gl-config -l`
DEF=`roki-gl-config -D`

CC=gcc
# CFLAGS=-ansi -Wall -O3 $(LIB) $(INCLUDE) $(DEF) -funroll-loops
CFLAGS=-g -ansi -Wall -O0 $(LIB) $(INCLUDE) $(DEF) -funroll-loops

%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LINK)
clean :
	rm -f *.o *~ core *test
