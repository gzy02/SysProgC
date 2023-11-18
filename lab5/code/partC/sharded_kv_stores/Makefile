all:
	echo 'nothing'

kv-server:
	# server
	export SRCDIR=.
	gcc -shared -fPIC list.c kv-server.c -o libkv-server.so
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.;go run kv-server.go kv-rpcs.go

kv-client:
	# client
	go build -buildmode=c-archive kv-client.go kv-rpcs.go
	gcc kv-client.c kv-rpcs.a -o kv-client.out
	./kv-client.out

clean:
	rm -rf *.i *.s *.o *.a *.so *-rpcs.h *.out

