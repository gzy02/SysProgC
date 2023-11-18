package main

import "C"
import (
	"log"
	"net/rpc"
)

func connect() *rpc.Client {
	client, err := rpc.Dial("tcp", ":1234")
	if err != nil {
		log.Fatal("dialing:", err)
	}
	return client
}

//export increment
func increment() int {
	client := connect()
	args := IncrementArgs{}
	reply := IncrementReply{}
	err := client.Call("CountService.Increment", &args, &reply)
	if err != nil {
		log.Fatal("error:", err)
	}
	client.Close()
	return reply.Count
}

//export decrement
func decrement() int {
	client := connect()
	args := DecrementArgs{}
	reply := DecrementReply{}
	err := client.Call("CountService.Decrement", &args, &reply)
	if err != nil {
		log.Fatal("error:", err)
	}
	client.Close()
	return reply.Count
}

//export get_count
func get_count() int {
	client := connect()
	args := GetCountArgs{}
	reply := GetCountReply{}
	err := client.Call("CountService.GetCount", &args, &reply)
	if err != nil {
		log.Fatal("error:", err)
	}
	client.Close()
	return reply.Count
}

func main() {}
