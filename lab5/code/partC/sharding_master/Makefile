all:
	echo 'nothing'

master-server:
	# server
	export SRCDIR=.
	gcc -shared -fPIC master-server.c configuration.c -o libmaster-server.so
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.;go run master-server.go master-rpcs.go

master-client:
	# client
	go build -buildmode=c-archive master-client.go master-rpcs.go
	gcc master-client.c configuration.c master-rpcs.a -o master-client.out
	./master-client.out

clean:
	rm -rf *.i *.s *.o *.a *.so *-rpcs.h *.out

