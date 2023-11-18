package main

// #cgo CFLAGS: -g -Wall
// #cgo LDFLAGS: -L${SRCDIR}/ -lkv-server
// #include "kv-server.h"
// #include <stdlib.h>
import "C"

import (
	"bufio"
	"fmt"
	"log"
	"net"
	"net/rpc"
	"os"
	"strings"
	"sync"
	"unsafe"

	. "../util"
)

type KV_DB_Service struct {
	addr string
}

type Server struct {
	listener net.Listener
	rpcs     *rpc.Server
}

func (kv *KV_DB_Service) Delete_kv_range(args *Delete_kv_rangeArgs, reply *Delete_kv_rangeReply) error {
	r := C.delete_kv_range(C.int(args.L), C.int(args.R))
	reply.Value = C.GoString(r)
	//fmt.Printf("\t%s DELETE_KV_RANGE 200 %d %d\n", kv.addr, args.L, args.R)
	return nil
}

func (kv *KV_DB_Service) Get_all_kv(args *Get_all_kv_Args, reply *Get_all_kv_Reply) error {
	r := C.get_all_kv()
	reply.Value = C.GoString(r)
	//fmt.Printf("\t%s GET_ALL_KV 200 %s\n", kv.addr, reply.Value)
	return nil
}

func (kv *KV_DB_Service) Put(args *PutArgs, reply *PutReply) error {
	keyC := C.CString(args.Key)
	valueC := C.CString(args.Value)
	defer C.free(unsafe.Pointer(keyC))
	defer C.free(unsafe.Pointer(valueC))

	C.put(keyC, valueC)
	fmt.Printf("\t%s PUT 200 %s: %s\n", kv.addr, args.Key, args.Value)
	return nil
}

func (kv *KV_DB_Service) Get(args *GetArgs, reply *GetReply) error {
	keyC := C.CString(args.Key)
	defer C.free(unsafe.Pointer(keyC))

	r := C.get(keyC)
	if r == nil {
		reply.Error = "key not found"
		fmt.Printf("\t%s GET 404 %s: %s\n", kv.addr, args.Key, reply.Error)
		return nil
	}
	reply.Value = C.GoString(r)
	fmt.Printf("\t%s GET 200 %s: %s\n", kv.addr, args.Key, reply.Value)
	return nil
}

func (kv *KV_DB_Service) Delete(args *DeleteArgs, reply *DeleteReply) error {
	keyC := C.CString(args.Key)
	defer C.free(unsafe.Pointer(keyC))
	C.delete(keyC)
	fmt.Printf("\t%s DELETE 200 %s\n", kv.addr, args.Key)
	return nil
}

func NewServer(addr string) (*Server, error) {
	service := &KV_DB_Service{addr: addr}
	rpcs := rpc.NewServer()
	rpcs.Register(service)
	l, e := net.Listen("tcp", addr)
	if e != nil {
		return nil, e
	}
	return &Server{listener: l, rpcs: rpcs}, nil
}

func (s *Server) Serve() {
	for {
		conn, err := s.listener.Accept()
		if err == nil {
			go s.rpcs.ServeConn(conn)
		} else {
			break
		}
	}
	fmt.Println("\tServer on", s.listener.Addr().String(), "finished")
}

func (s *Server) Close() error {
	return s.listener.Close()
}

func main() {
	cmdChan := make(chan string)
	servers := make(map[string]*Server)
	var mu sync.Mutex
	fmt.Println("Enter command (ADD/REMOVE) and server address (e.g., ADD localhost:1234): ")
	go func() {
		reader := bufio.NewReader(os.Stdin)
		for {
			cmd, _ := reader.ReadString('\n')
			cmd = strings.TrimSpace(cmd)
			cmdChan <- cmd
		}
	}()

	for cmd := range cmdChan {
		parts := strings.SplitN(cmd, " ", 2)
		if len(parts) != 2 {
			fmt.Println("\tInvalid command")
			continue
		}

		action, addr := parts[0], parts[1]

		switch action {
		case "ADD":
			server, err := NewServer(addr)
			if err != nil {
				log.Println("\tFailed to start server on", addr, ":", err)
				continue
			}
			mu.Lock()
			servers[addr] = server
			mu.Unlock()
			go server.Serve()
			fmt.Println("\tServer started on", addr)

		case "REMOVE":
			mu.Lock()
			server, ok := servers[addr]
			mu.Unlock()
			if ok {
				server.Close()
				mu.Lock()
				delete(servers, addr)
				mu.Unlock()
				fmt.Println("\tServer on", addr, "stopped")
			} else {
				fmt.Println("\tNo server running on", addr)
			}

		default:
			fmt.Println("\tUnknown command:", action)
		}
	}
}
