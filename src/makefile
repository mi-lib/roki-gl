include ../config

ROOTDIR=..
LIBDIR=$(ROOTDIR)/lib
INCLUDE=-I$(ROOTDIR)/include -I$(PREFIX)/include

CC=gcc
CFLAGS=-ansi -Wall -fPIC -O3 $(INCLUDE) -funroll-loops
LD=gcc
LDFLAGS=-shared
SIGNUP=echo "RoKi-GL ver."$(VERSION)" Copyright (C) 2000 Tomomichi Sugihara (Zhidao)" >>

OBJ=rkgl_misc.o rkgl_camera.o rkgl_input.o\
	rkgl_optic.o\
	rkgl_shape.o rkgl_chain.o\
	rkgl_texture.o\
	rkgl_shadow.o\
	rkgl_select.o\
	rkgl_terra.o
DLIB=libroki-gl.so

ifeq ($(CONFIG_USE_GLX),y)
	OBJ += rkgl_glx.o
endif

ifeq ($(CONFIG_USE_GLUT),y)
	OBJ += rkgl_glut.o
endif

TARGET=$(DLIB)

all: $(TARGET)

$(DLIB): $(OBJ)
	@echo " LD	" $^
	@$(LD) $(LDFLAGS) -o $@ $^ > /dev/null; $(SIGNUP) $@
	-@mv $@ $(LIBDIR)
%.o: %.c
	@echo " CC	" $<
	@$(CC) $(CFLAGS) -c $< > /dev/null
clean:
	@echo " CLEAN"
	-@rm -f *.o *~ $(TARGET)
