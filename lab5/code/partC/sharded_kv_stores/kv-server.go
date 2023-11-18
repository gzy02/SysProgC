package main

// #cgo CFLAGS: -g -Wall
// #cgo LDFLAGS: -L${SRCDIR}/ -lkv-server
// #include "kv-server.h"
// #include <stdlib.h>
import "C"
import (
	"fmt"
	"log"
	"net"
	"net/rpc"
	"unsafe"
)

//
// Server
//

type KV_DB_Service struct {
}

func server() {
	service := &KV_DB_Service{}
	rpcs := rpc.NewServer()
	rpcs.Register(service)
	l, e := net.Listen("tcp", ":1234")
	if e != nil {
		log.Fatal("listen error:", e)
	}
	// i := 0
	for {
		// i = i + 1
		conn, err := l.Accept()
		// fmt.Println("accept one... %d", i)
		if err == nil {
			go rpcs.ServeConn(conn)
		} else {
			break
		}
	}
	fmt.Println("finished")
}

func (kv *KV_DB_Service) Put(args *PutArgs, reply *PutReply) error {
	keyC := C.CString(args.Key)
	valueC := C.CString(args.Value)

	defer C.free(unsafe.Pointer(keyC))
	defer C.free(unsafe.Pointer(valueC))

	C.put(keyC, valueC)
	// C.put(*C.char(args.Key), *C.char(args.Value))
	return nil
}

func (kv *KV_DB_Service) Append(args *AppendArgs, reply *AppendReply) error {
	keyC := C.CString(args.Key)
	valueC := C.CString(args.Value)

	defer C.free(unsafe.Pointer(keyC))
	defer C.free(unsafe.Pointer(valueC))

	C.append(keyC, valueC)
	// C.Append(*C.char(args.Key), *C.char(args.Value))
	return nil
}

func (kv *KV_DB_Service) Get(args *GetArgs, reply *GetReply) error {
	keyC := C.CString(args.Key)
	defer C.free(unsafe.Pointer(keyC))

	r := C.get(keyC)
	if r == nil {
		reply.Error = "key not found"
		return nil
	}

	fmt.Printf("Get result: %s\n", C.GoString(r))
	reply.Value = C.GoString(r)
	return nil
}

func (kv *KV_DB_Service) Delete(args *DeleteArgs, reply *DeleteReply) error {
	keyC := C.CString(args.Key)
	defer C.free(unsafe.Pointer(keyC))

	C.delete(keyC)

	return nil
}

//
// main
//

func main() {
	fmt.Println("server started...")
	server()
}
