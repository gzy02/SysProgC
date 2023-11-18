
kv-client:
	# client
	go build -o kv-rpcs.a -buildmode=c-archive kv-client.go
	gcc kv-client.c ../util_c/configuration.c ../util_c/hash-table.c kv-rpcs.a -o kv-client.out
	./kv-client.out

clean:
	rm -rf *.i *.s *.o *.a *.so *-rpcs.h *.out