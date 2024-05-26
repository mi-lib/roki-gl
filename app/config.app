APPLINK=
APPLINKCPP=
DEPENDENCY=

ifeq ($(CONFIG_USE_LIW),y)
	APPLINK=`liw-config -l`
	APPLINKCPP=`liw-config -lcpp`
	DEPENDENCY="liw=1.3.1;"
endif
