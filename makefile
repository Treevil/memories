all: banditore

banditore: header.c banditore.c
	gcc -W header.c banditore.c -o banditore

clean:
	rm -f *.o
