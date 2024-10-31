CFLAGS=`roki-gl-config --cflags`
LINK=`roki-gl-config -l`

TARGET=$(shell ls *.c | xargs -i basename {} .c | tr -s "\n" " ")

CC=gcc
CFLAGS+=-Wall -O3 -funroll-loops

all: $(TARGET)
%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LINK)
clean :
	rm -f *.o *~ core *test $(TARGET)
