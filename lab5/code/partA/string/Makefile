all:
	echo 'nothing'

string-server:
	# server
	export SRCDIR=.
	gcc -c -shared -fPIC string-server.c -o libstring-server.so
	go run string-server.go string-rpcs.go

string-client:
	# client
	go build -buildmode=c-archive string-client.go string-rpcs.go
	gcc string-client.c string-rpcs.a -o string-client.out
	./string-client.out

clean:
	rm -rf *.i *.s *.o *.a *.so *-rpcs.h *.out

