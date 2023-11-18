package main

// #cgo CFLAGS: -g -Wall
// #cgo LDFLAGS: -L${SRCDIR}/ -lstack-server
// #include "stack-server.h"
import "C"
import (
	"fmt"
	"log"
	"net"
	"net/rpc"
)

//
// Server
//

type StackService struct {
}

func server() {
	service := &StackService{}
	rpcs := rpc.NewServer()
	rpcs.Register(service)
	l, e := net.Listen("tcp", ":2345")
	if e != nil {
		log.Fatal("listen error:", e)
	}
	i := 0
	for {
		i = i + 1
		conn, err := l.Accept()
		fmt.Println("accept one... %d", i)
		if err == nil {
			go rpcs.ServeConn(conn)
		} else {
			break
		}
	}
	fmt.Println("finished")
}

func (stack *StackService) Stack_init(args *StackArgs, reply *StackReply) error {

	C.stack_init()

	return nil
}

func (stack *StackService) Stack_push(args *StackArgs, reply *StackReply) error {

	C.stack_push(C.int(args.Value))

	return nil
}

func (stack *StackService) Stack_pop(args *StackArgs, reply *StackReply) error {

	r := C.stack_pop()
	reply.Result = int(r)

	return nil
}

func (stack *StackService) Stack_size(args *StackArgs, reply *StackReply) error {

	r := C.stack_size()
	reply.Result = int(r)

	return nil
}

//
// main
//

func main() {
	fmt.Println("server started...")
	server()
	C.stack_init()
}
