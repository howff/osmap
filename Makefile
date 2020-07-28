all: Makefile.lib.$$SWDEVARCH
	rm -f libosmap.a
	make -f Makefile.lib.$$SWDEVARCH
	cp libosmap.a ${SWDEVLOCAL}/lib/libosmap.a
	-@chmod g=u ${SWDEVLOCAL}/lib/libsat.a *.o *.c *.h *.cpp

Makefile.lib.$$SWDEVARCH: osmap.pro
	qmake3 -o Makefile.lib.$$SWDEVARCH       osmap.pro

qct2png: qct2png.cpp qct.cpp qct.h qct.o inpoly.c inpoly.o
	cc -I. -o qct2png qct2png-main.cpp qct.o inpoly.o

# Tidy up
clean:
	rm -f *.o core
