all: auctioneer gambler agent

auctioneer: utility.c auctioneer.c
	gcc -W utility.c auctioneer.c -o auctioneer.out

gambler: utility.c gambler.c
	gcc -W utility.c gambler.c -o gambler.out

agent: utility.c agent.c
	gcc -W utility.c agent.c -o agent.out

clean:
	rm -f *.o
	