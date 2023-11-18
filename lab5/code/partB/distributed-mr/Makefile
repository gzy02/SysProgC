all:
	echo 'nothing'

counter-mr.so: 
	gcc -g -shared -fPIC -o ./counter-mr.so ../util/counter-mr.c ../util/key-value.c ../lib/list.c

mr-server:
	# server
	# 若发生错误请在控制台键入 export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:$$(pwd)
	export SRCDIR=.
	gcc -shared -fPIC mr-server.c ../lib/concur-hash.c file-struct.c -o libmr-server.so
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.;go run mr-server.go mr-rpcs.go

mr-client:counter-mr.so
	# client
	go build -buildmode=c-archive mr-client.go mr-rpcs.go
	gcc mr-client.c mr.c ../lib/concur-hash.c ../lib/list.c ../util/key-value.c mr-rpcs.a -o mr-client.out
	./mr-client.out ./counter-mr.so 4

clean:
	rm -rf *.i *.s *.o *.a *.so *-rpcs.h *.out core.* *.txt ./tmp ./output 