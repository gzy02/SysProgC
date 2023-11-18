package main

// #include <stdlib.h>
import "C"
import (
	"fmt"
	"log"
	"net/rpc"
	"strings"
	"unsafe"
)

const (
	OUTPUT_DIR = "./output"
	TMP_DIR    = "/tmp/distri_data"
	SEP        = "\n"
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

//export Task_get
func Task_get() *C.char {
	client := connect()
	args := Task_t{}
	reply := Task_t{}
	err := client.Call("MRService.Task_get", &args, &reply)
	if err != nil {
		log.Fatal("error:", err)
	}
	client.Close()
	return C.CString(reply.Task_type)
}

//export Map_get
func Map_get() *C.char {
	client := connect()
	args := Map_t{}
	reply := Map_t{}
	err := client.Call("MRService.Map_get", &args, &reply)
	if err != nil {
		log.Fatal("error:", err)
	}
	client.Close()
	return C.CString(reply.Name)
}

//export Map_finish
func Map_finish(input_strings **C.char, file_size C.int) {
	client := connect()
	map_name := C.GoString(*(**C.char)(unsafe.Pointer(uintptr(unsafe.Pointer(input_strings)))))

	file_names := make([]string, 0)
	for i := 1; i < int(file_size)+1; i++ {
		cString := *(**C.char)(unsafe.Pointer(uintptr(unsafe.Pointer(input_strings)) + uintptr(i)*unsafe.Sizeof(input_strings)))
		if cString == nil {
			break
		}
		file_names = append(file_names, C.GoString(cString))
	}
	args := Map_finish_t{map_name, file_names}
	reply := Map_finish_t{}
	err := client.Call("MRService.Map_finish", &args, &reply)
	if err != nil {
		log.Fatal("error:", err)
	}
	fmt.Println("map " + args.Name + " finish")
	client.Close()
}

//export Reduce_get
func Reduce_get() *C.char {
	client := connect()
	args := Reduce_t{}
	reply := Reduce_t{}
	err := client.Call("MRService.Reduce_get", &args, &reply)
	if err != nil {
		log.Fatal("error:", err)
	}
	client.Close()
	/*
		files := strings.Join(reply.Input_files, " ")
		reduce_task := reply.Name + " " + files
		return C.CString(reduce_task)
	*/
	// 传输过来的已经是序列化后的字符串了，不需要再序列化
	return C.CString(reply.Name)
}

//export Reduce_finish
func Reduce_finish(reduce_name *C.char) {
	client := connect()
	args := Reduce_t{Name: C.GoString(reduce_name)}
	reply := Reduce_t{}
	err := client.Call("MRService.Reduce_finish", &args, &reply)
	if err != nil {
		log.Fatal("error:", err)
	}
	fmt.Println("reduce " + strings.Split(args.Name, SEP)[0] + " finish")
	client.Close()
}

//export Get_mapsize
func Get_mapsize() C.int {
	client := connect()
	args := Int_t{}
	reply := Int_t{}
	err := client.Call("MRService.Get_mapsize", &args, &reply)
	if err != nil {
		log.Fatal("error:", err)
	}
	client.Close()
	return C.int(reply.Value)
}

//export Get_reducesize
func Get_reducesize() C.int {
	client := connect()
	args := Int_t{}
	reply := Int_t{}
	err := client.Call("MRService.Get_reducesize", &args, &reply)
	if err != nil {
		log.Fatal("error:", err)
	}
	client.Close()
	return C.int(reply.Value)
}

//
// main
//

func main() {

}
