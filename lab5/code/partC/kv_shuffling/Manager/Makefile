
master-client:
	# client
	go build -o master-rpcs.a -buildmode=c-archive master-client.go
	gcc master-client.c manager.c ../util_c/configuration.c ../util_c/hash-table.c master-rpcs.a -o master-client.out
	./master-client.out

clean:
	rm -rf *.i *.s *.o *.a *.so *-rpcs.h *.out