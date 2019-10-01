include ../config

INCLUDE=-I. -I../include -I$(PREFIX)/include
LIB=-L../lib -L$(PREFIX)/lib
BINDIR=$(PREFIX)/bin

CC=gcc
#CFLAGS=-Wall -O3 $(INCLUDE) $(LIB) -D__USE_GLEW
CFLAGS=-Wall -O3 $(INCLUDE) $(LIB)

#GLLINK=-lGLEW -lglut -lGLU -lGL
GLLINK=-lglut -lGLU -lGL

LINK=-lroki-gl $(GLLINK) `zx11-config -l` `liw-config -l` `roki-config -l`

COMPILE=$(CC) $(CFLAGS) -o $@ $< $(LINK)

INSTALL=install -m 755
UNINSTALL=rm -f

CONFIGGEN=zeda-config-gen
CONFIG=roki-gl-config
TARGET=$(CONFIG) rk_view rk_anim rk_seq rk_pen

#####################################################################
all: $(TARGET)

$(CONFIG):
	@echo " GENERATE" $@
	-@rm -f $@
	@$(CONFIGGEN) -I $(PREFIX)/include -L $(PREFIX)/lib -l "$(LINK)" -v $(VERSION) > $@

%: %.c
	@echo " CC	" $<
	@$(COMPILE)
install: $(TARGET)
	@echo " INSTALL" $^
	@$(INSTALL) $^ $(BINDIR)

clean:
	@echo " CLEAN"
	-@rm -f *.o *~ core $(TARGET)
uninstall:
	@echo " UNINSTALL	" $(TARGET)
	@cd $(BINDIR); $(UNINSTALL) $(TARGET); cd -
