package main

// #cgo CFLAGS: -g -Wall
// #cgo LDFLAGS: -L${SRCDIR}/ -lsquare-server
// #include "square-server.h"
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

type SquareService struct {
}

func server() {
	service := &SquareService{}
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

func (square *SquareService) Square(args *SquareArgs, reply *SquareReply) error {

	// if we can want to write in go...
	//reply.Result = args.Value * args.Value
	r := C.square(C.int(args.Value))
	fmt.Printf("c result: %d\n", r)
	reply.Result = int(r)

	return nil
}

//
// main
//

func main() {
	fmt.Println("server started...")
	server()
}
