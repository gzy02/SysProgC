package main

import (
	"C"
	"log"
	"net/rpc"
)

import . "../util"

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

//export Delete_kv_range
func Delete_kv_range(l C.int, r C.int) *C.char {
	client := connect()
	if client == nil {
		return C.CString("")
	}
	defer client.Close()

	args := Delete_kv_rangeArgs{int(l), int(r)}
	reply := Delete_kv_rangeReply{}
	err := client.Call("KV_DB_Service.Delete_kv_range", &args, &reply)
	if err != nil {
		log.Println("error:", err)
		return C.CString("")
	}
	return C.CString(reply.Value)
}

//export Get_all_kv
func Get_all_kv() *C.char {
	client := connect()
	if client == nil {
		return C.CString("")
	}
	defer client.Close()

	args := Get_all_kv_Args{}
	reply := Get_all_kv_Reply{}
	err := client.Call("KV_DB_Service.Get_all_kv", &args, &reply)
	if err != nil {
		log.Println("error:", err)
		return C.CString("")
	}
	return C.CString(reply.Value)
}

//export Config_IP
func Config_IP(addr *C.char) {
	globalAddress = C.GoString(addr)
}

//export Put
func Put(key *C.char, value *C.char) {
	client := connect()
	if client == nil {
		return
	}
	defer client.Close()

	keyGo := C.GoString(key)
	valueGo := C.GoString(value)

	args := PutArgs{keyGo, valueGo}
	reply := PutReply{}
	err := client.Call("KV_DB_Service.Put", &args, &reply)
	if err != nil {
		log.Println("error:", err)
	}
}

//export Get
func Get(key *C.char) *C.char {
	client := connect()
	if client == nil {
		return C.CString("")
	}
	defer client.Close()

	keyGo := C.GoString(key)
	args := GetArgs{keyGo}
	reply := GetReply{}
	err := client.Call("KV_DB_Service.Get", &args, &reply)
	if err != nil {
		log.Println("error:", err)
		return C.CString("")
	}
	if reply.Error != "" {
		log.Println("KV_DB_Service.Get error:", reply.Error)
		return C.CString("")
	}
	return C.CString(reply.Value)
}

//export Delete
func Delete(key *C.char) {
	client := connect()
	if client == nil {
		return
	}
	defer client.Close()

	keyGo := C.GoString(key)
	args := DeleteArgs{keyGo}
	reply := DeleteReply{}
	err := client.Call("KV_DB_Service.Delete", &args, &reply)
	if err != nil {
		log.Println("error:", err)
	}
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
	err := client.Call("ShardingMasterService.RemoveServer", &args, &reply)
	if err != nil {
		log.Println("error:", err)
		return C.CString("")
	}
	if reply.Error != "" {
		log.Println("ShardingMasterService.RemoveServer error:", reply.Error)
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
	err := client.Call("ShardingMasterService.AddServer", &args, &reply)
	if err != nil {
		log.Println("error:", err)
		return C.CString("")
	}
	if reply.Error != "" {
		log.Println("ShardingMasterService.AddServer error:", reply.Error)
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
	err := client.Call("ShardingMasterService.GetConfig", &args, &reply)
	if err != nil {
		log.Println("error:", err)
		return C.CString("")
	}
	if reply.Error != "" {
		log.Println("ShardingMasterService.GetConfig error:", reply.Error)
		return C.CString("")
	}
	return C.CString(reply.Value)
}

//
// main
//

func main() {
}
