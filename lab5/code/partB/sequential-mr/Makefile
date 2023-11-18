
all: counter-mr.so seq-mr.out

counter-mr.so: 
	gcc -g -shared -fPIC -o counter-mr.so ../util/counter-mr.c ../util/key-value.c ../lib/list.c

seq-mr.out: 
	gcc -g seq-mr.c ../util/key-value.c ../lib/list.c file-struct.c ../lib/concur-hash.c -o seq-mr.out -ldl
	./seq-mr.out 4 ./counter-mr.so a.txt b.txt c.txt d.txt e.txt f.txt g.txt h.txt i.txt j.txt

clean:
	rm -rf *.so *.out ./output/
