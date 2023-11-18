kv-server:
	# server
	export SRCDIR=.
	gcc -shared -fPIC kv-server.c ../util_c/hash-table.c -o libkv-server.so
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.;go run kv-server.go

clean:
	rm -rf *.i *.s *.o *.a *.so *-rpcs.h *.out

