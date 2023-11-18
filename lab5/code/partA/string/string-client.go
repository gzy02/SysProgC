package main

/*
#include <string.h>
*/
import "C"

import (
	"log"
	"net/rpc"
)

// 
// Client
// 

func connect() *rpc.Client {
	client, err := rpc.Dial("tcp", "127.0.0.1:1234")
	if err != nil {
		log.Fatal("dialing:", err)
	}
	return client
}

//export CountString
func CountString(str *C.char) int {
	client := connect()
	args := StringArgs{C.GoString(str)}
	reply := StringReply{}
	err := client.Call("StringService.CountString", &args, &reply)
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
