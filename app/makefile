include ../config

INCLUDE=$(shell echo `zx11-config -I` `liw-config -I` `roki-config -I` | tr ' ' '\n' | sort | uniq | xargs)
LIB=$(shell echo `zx11-config -L` `liw-config -L` `roki-config -L` | tr ' ' '\n' | sort | uniq | xargs)
DEF=$(shell echo `zx11-config -D` `liw-config -D` `roki-config -D` | tr ' ' '\n' | sort | uniq | xargs)
BINDIR=$(PREFIX)/bin

CC=gcc
CFLAGS=-Wall -O3 $(INCLUDE) $(LIB) $(DEF)

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
	@$(CONFIGGEN) -I "$(INCLUDE)" -L "$(LIB)" -D "$(DEF)" -l "$(LINK)" -v $(VERSION) > $@
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
