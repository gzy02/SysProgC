package main

import (
	"C"
	"log"
	"net/rpc"
)

//
// Client
//

var globalAddress string

func connect() *rpc.Client {
	client, err := rpc.Dial("tcp", globalAddress)
	if err != nil {
		log.Fatal("dialing:", err)
	}
	return client
}

//export Config_IP
func Config_IP(addr *C.char) {
	globalAddress = C.GoString(addr)
}

//export RemoveServer
func RemoveServer(key *C.char) *C.char {
	client := connect()
	if client == nil {
		return C.CString("")
	}
	defer client.Close()

	keyGo := C.GoString(key)
	args := RemoveServerArgs{keyGo}
	reply := RemoveServerReply{}
	err := client.Call("KV_DB_Service.RemoveServer", &args, &reply)
	if err != nil {
		log.Println("error:", err)
		return C.CString("")
	}
	if reply.Error != "" {
		log.Println("KV_DB_Service.RemoveServer error:", reply.Error)
		return C.CString("")
	}
	return C.CString(reply.Value)
}

//export AddServer
func AddServer(key *C.char) *C.char {
	client := connect()
	if client == nil {
		return C.CString("")
	}
	defer client.Close()

	keyGo := C.GoString(key)
	args := AddServerArgs{keyGo}
	reply := AddServerReply{}
	err := client.Call("KV_DB_Service.AddServer", &args, &reply)
	if err != nil {
		log.Println("error:", err)
		return C.CString("")
	}
	if reply.Error != "" {
		log.Println("KV_DB_Service.AddServer error:", reply.Error)
		return C.CString("")
	}
	return C.CString(reply.Value)
}

//export GetConfig
func GetConfig() *C.char {
	client := connect()
	if client == nil {
		return C.CString("")
	}
	defer client.Close()

	args := GetConfigArgs{}
	reply := GetConfigReply{}
	err := client.Call("KV_DB_Service.GetConfig", &args, &reply)
	if err != nil {
		log.Println("error:", err)
		return C.CString("")
	}
	if reply.Error != "" {
		log.Println("KV_DB_Service.GetConfig error:", reply.Error)
		return C.CString("")
	}
	return C.CString(reply.Value)
}

//
// main
//

func main() {
}
