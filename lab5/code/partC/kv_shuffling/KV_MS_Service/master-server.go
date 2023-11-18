package main

// #cgo CFLAGS: -g
// #cgo LDFLAGS: -L${SRCDIR}/ -lmaster-server
// #include "master-server.h"
// #include <stdlib.h>
import "C"
import (
	"fmt"
	"log"
	"net"
	"net/rpc"
	"unsafe"
)

import . "../util"

//
// Server
//

type ShardingMasterService struct {
}

func server() {
	service := &ShardingMasterService{}
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

func (kv *ShardingMasterService) AddServer(args *AddServerArgs, reply *AddServerReply) error {
	keyC := C.CString(args.Key)
	defer C.free(unsafe.Pointer(keyC))

	r := C.add_server(keyC)
	if r == nil {
		reply.Error = "key not found"
		return nil
	}

	fmt.Printf("AddServer result: %s\n", C.GoString(r))
	reply.Value = C.GoString(r)
	return nil
}

func (kv *ShardingMasterService) RemoveServer(args *RemoveServerArgs, reply *RemoveServerReply) error {
	keyC := C.CString(args.Key)
	defer C.free(unsafe.Pointer(keyC))

	r := C.remove_server(keyC)
	if r == nil {
		reply.Error = "key not found"
		return nil
	}

	fmt.Printf("RemoveServer result: %s\n", C.GoString(r))
	reply.Value = C.GoString(r)
	return nil
}

func (kv *ShardingMasterService) GetConfig(args *GetConfigArgs, reply *GetConfigReply) error {
	r := C.getconfig()
	if r == nil {
		reply.Error = "key not found"
		return nil
	}
	reply.Value = C.GoString(r)
	return nil
}

//
// main
//

func main() {
	C.config_init()
	fmt.Println("server started...")
	server()
}
