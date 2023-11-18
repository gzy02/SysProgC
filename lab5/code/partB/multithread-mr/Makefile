all: counter-mr.so mthd-mr.out

counter-mr.so: 
	gcc -g -shared -fPIC -o ./counter-mr.so ../util/counter-mr.c ../util/key-value.c ../lib/list.c

mthd-mr.out:
	gcc -g main.c task.c mr.c ../lib/list.c ../util/key-value.c file-struct.c ../lib/concur-hash.c -o mthd-mr.out
	./mthd-mr.out ./counter-mr.so 8 a.txt b.txt c.txt d.txt e.txt f.txt g.txt h.txt i.txt j.txt

clean:
	rm -rf *.i *.s *.o *.out core.* *.so *.txt ./output 