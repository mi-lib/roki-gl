include ../config

BINDIR=$(PREFIX)/bin

INSTALL=install -m 755
UNINSTALL=rm -f

INCLUDE=$(shell echo `zx11-config -I` `liw-config -I` `roki-config -I` | tr ' ' '\n' | sort | uniq | xargs)
LIB=$(shell echo `zx11-config -L` `liw-config -L` `roki-config -L` | tr ' ' '\n' | sort | uniq | xargs)
DEF=$(shell echo `zx11-config -D` `liw-config -D` `roki-config -D` | tr ' ' '\n' | sort | uniq | xargs)

LINK=-lroki-gl -lglut -lGLU -lGL `zx11-config -l` `liw-config -l` `roki-config -l`

CONFIGGEN=`which zeda-config-gen`
CONFIG=roki-gl-config

#####################################################################
all: $(CONFIG)

$(CONFIG):
	@echo " GENERATE	" $@
	-@rm -f $@
	@$(CONFIGGEN) -I "$(INCLUDE)" -L "$(LIB)" -D "$(DEF)" -l "$(LINK)" -v $(VERSION) > $@
install: $(CONFIG)
	@echo " INSTALL	" $^
	@$(INSTALL) $^ $(BINDIR)
clean:
	@echo " CLEAN"
	-@rm -f *.o *~ core $(CONFIG)
uninstall:
	@echo " UNINSTALL	" $(CONFIG)
	@$(UNINSTALL) $(BINDIR)/$(CONFIG)
