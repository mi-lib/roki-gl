include ../config

CONFIG=roki-gl-config
BINDIR=$(PREFIX)/bin

INCLUDE=`$(CONFIG) -I`
LIB=`$(CONFIG) -L`
DEF=`$(CONFIG) -D`
LINK=`$(CONFIG) -l`

CC=gcc
CFLAGS=-Wall -O3 $(INCLUDE) $(LIB) $(DEF)

COMPILE=$(CC) $(CFLAGS) -o $@ $< $(LINK)
INSTALL=install -m 755
UNINSTALL=rm -f

TARGET=rk_view rk_anim rk_seq rk_pen

#####################################################################
all: $(TARGET)

%: %.c
	@echo " CC	" $<
	@$(COMPILE)
clean:
	@echo " CLEAN"
	-@rm -f *.o *~ core $(TARGET)
install: $(TARGET)
ifneq ($(TARGET),)
	@echo " INSTALL	" $^
	@$(INSTALL) $^ $(BINDIR)
endif
uninstall:
ifneq ($(TARGET),)
	@echo " UNINSTALL	" $(TARGET)
	@$(UNINSTALL) $(BINDIR)/$(TARGET)
endif
