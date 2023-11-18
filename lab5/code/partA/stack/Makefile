all:
	echo 'nothing'

stack-server:
	# server
	export SRCDIR=.
	gcc -c -shared -fPIC stack-server.c -o libstack-server.so
	go run stack-server.go stack-rpcs.go

stack-client:
	# client
	go build -buildmode=c-archive stack-client.go stack-rpcs.go
	gcc stack-client.c stack-rpcs.a -o stack-client.out
	./stack-client.out

constack-client:
	# client
	go build -buildmode=c-archive stack-client.go stack-rpcs.go
	gcc constack-client.c stack-rpcs.a -o constack-client.out
	./constack-client.out

clean:
	rm -rf *.i *.s *.o *.a *.so *-rpcs.h *.out

