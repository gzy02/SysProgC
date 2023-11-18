
master-server:
	# server
	export SRCDIR=.
	gcc -shared -fPIC master-server.c ../util_c/configuration.c ../util_c/hash-table.c -o libmaster-server.so
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.;go run master-server.go


clean:
	rm -rf *.i *.s *.o *.a *.so *-rpcs.h *.out

