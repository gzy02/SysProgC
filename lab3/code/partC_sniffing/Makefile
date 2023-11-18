all:
	gcc promiscuous_main.c packet-analyze.c -o main.out
	// sniffing requires super user privilege
	sudo ./main.out

sniff:
	gcc -o analysis.out filter.c main.c packet-process.c
	sudo ./analysis.out

clean:
	rm -rf *.out
