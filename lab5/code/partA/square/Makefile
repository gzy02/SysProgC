all:
	echo 'nothing'

square-server:
	# server
	export SRCDIR=.
	gcc -c -shared -fPIC square-server.c -o libsquare-server.so
	go run square-server.go square-rpcs.go

square-client:
	# client
	go build -buildmode=c-archive square-client.go square-rpcs.go
	gcc square-client.c square-rpcs.a -o square-client.out
	./square-client.out

clean:
	rm -rf *.i *.s *.o *.a *.so *-rpcs.h *.out
