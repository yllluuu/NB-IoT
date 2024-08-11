APPNAME=nbiot

LIBPATH=./lib/
GPIODLIB=./lib/gpiod/install/lib
INCPATH=./inc/
GPIODINC=./lib/gpiod/install/include


CFLAGS+=-I${INCPATH}
CFLAGS+=-I${GPIODINC}


LDFLAGS+=-L${LIBPATH} 
LDFLAGS+=-L${GPIODLIB} 



CC=gcc

all:
	make -C lib/gpiod/ 
	make -C src
	${CC} ${CFLAGS} -g main.c -o ${APPNAME} ${LDFLAGS} -lnbiot -lgpiod -pthread

clean:
	make clean -C src
	rm -f ${APPNAME}

run:
		sudo LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${LIBPATH}  ./nbiot
