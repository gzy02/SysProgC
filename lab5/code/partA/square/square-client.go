package main

import (
	"C"
	"log"
	"net/rpc"
)

//
// Client
//

func connect() *rpc.Client {
	client, err := rpc.Dial("tcp", ":1234")
	if err != nil {
		log.Fatal("dialing:", err)
	}
	return client
}

//export Square
func Square(value int) int {
	client := connect()
	args := SquareArgs{value}
	reply := SquareReply{}
	err := client.Call("SquareService.Square", &args, &reply)
	if err != nil {
		log.Fatal("error:", err)
	}
	client.Close()
	return reply.Result
}

//
// main
//

func main() {
}
