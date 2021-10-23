all: auctioneer gambler

auctioneer: utility.c auctioneer.c
	gcc -W utility.c auctioneer.c -o auctioneer.out

gambler: utility.c gambler.c
	gcc -W utility.c gambler.c -o gambler.out
 
clean:
	rm -f *.o
	