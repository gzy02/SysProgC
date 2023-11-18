all:
	echo 'nothing'

count-server:
	# server
	export SRCDIR=.
	gcc -c -shared -fPIC count-server.c -o libcount-server.so
	go run count-server.go count-rpcs.go

count-client:
	# client
	go build -buildmode=c-archive count-client.go count-rpcs.go
	gcc count-client.c count-rpcs.a -o count-client.out
	./count-client.out

clean:
	rm -rf *.i *.s *.o *.a *.so *-rpcs.h *.out

