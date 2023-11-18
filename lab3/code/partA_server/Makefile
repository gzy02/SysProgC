all:square-server square-client web-server
	# empty

square-server:
	gcc square-server.c -o square-server.out
	./square-server.out

square-client:
	gcc square-client.c -o square-client.out
	./square-client.out

web-server:
	mkdir -p $(HOME)/mywebsite 
	cp index.html $(HOME)/mywebsite/index.html
	gcc web-server.c -o web-server.out
	./web-server.out

clean:
	rm -rf *.i *.s *.o *.out core.*



