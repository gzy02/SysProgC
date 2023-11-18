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
	client, err := rpc.Dial("tcp", ":2345")
	if err != nil {
		log.Fatal("dialing:", err)
	}
	return client
}

//export Stack_init
func Stack_init() {
	client := connect()
	args := StackArgs{}
	reply := StackReply{}
	err := client.Call("StackService.Stack_init", &args, &reply)
	if err != nil {
		log.Fatal("error:", err)
	}
	client.Close()

}

//export Stack_push
func Stack_push(value int) {
	client := connect()
	args := StackArgs{value}
	reply := StackReply{}
	err := client.Call("StackService.Stack_push", &args, &reply)
	if err != nil {
		log.Fatal("error:", err)
	}
	client.Close()

}

//export Stack_pop
func Stack_pop() int{
	client := connect()
	args := StackArgs{}
	reply := StackReply{}
	err := client.Call("StackService.Stack_pop", &args, &reply)
	if err != nil {
		log.Fatal("error:", err)
	}
	client.Close()
	return reply.Result
}

//export Stack_size
func Stack_size() int{
	client := connect()
	args := StackArgs{}
	reply := StackReply{}
	err := client.Call("StackService.Stack_size", &args, &reply)
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
