CFLAGS=-O2

main: main.o display.o fcgi_util.o

clean:
	rm -f main *.o
