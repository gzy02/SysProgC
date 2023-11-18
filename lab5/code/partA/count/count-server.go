package main

// #cgo CFLAGS: -g -Wall
// #cgo LDFLAGS: -L${SRCDIR}/ -lcount-server
// #include "count-server.h"
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

type CountService struct{
}

func server() {
	service := &CountService{}
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

func (s *CountService) Increment(args *IncrementArgs, reply *IncrementReply) error {
	reply.Count = int(C.increment())
	return nil
}

func (s *CountService) Decrement(args *DecrementArgs, reply *DecrementReply) error {
	reply.Count = int(C.decrement())
	return nil
}

func (s *CountService) GetCount(args *DecrementArgs, reply *DecrementReply) error {
	reply.Count = int(C.get_count())
	return nil
}

//
// main
//

func main() {
	fmt.Println("server started...")
	server()
}
