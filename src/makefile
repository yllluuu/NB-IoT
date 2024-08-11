LIBNAME=nbiot
INSTPATH=`pwd`/../lib/ 
INCPATH=`pwd`/../inc/ 
GPIODINC=`pwd`/../lib/gpiod/install/include
GPIODLIB=`pwd`/../lib/gpiod/install/lib


CFLAGS+=-I${INCPATH}
CFLAGS+=-I${EVPATH}
CFLAGS+=-L${GPIODLIB} -lgpiod -lpthread

CC=gcc
AR=ar 


all: static_lib
	@make install
	@make clear

dynamic_lib:
	${CC} -shared -fPIC *.c -o lib${LIBNAME}.so ${CFLAGS}

static_lib:
	${CC} -c *.c ${CFLAGS}
	${AR} -rcs lib${LIBNAME}.a *.o

install:
	cp -rf lib${LIBNAME}.* ${INSTPATH}

uninstall:
	rm -rf `pwd`/../lib/lib${LIBNAME}.*

clear:
	rm -f *.o
	rm -f lib${LIBNAME}.*
