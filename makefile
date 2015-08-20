all: banditore client

banditore: header.c banditore.c
	gcc -W header.c banditore.c -o banditore

client: header.c client.c
	gcc -W header.c client.c -o client
 
clean:
	rm -f *.o
	