package main

// #cgo CFLAGS: -g -Wall
// #cgo LDFLAGS: -L${SRCDIR}/ -lmr-server
// #include "mr-server.h"
// #include <stdlib.h>
import "C"

import (
	"bufio"
	"fmt"
	"log"
	"net"
	"net/rpc"
	"os"
	"strconv"
	"strings"
	"unsafe"
)

const (
	OUTPUT_DIR = "./output"
	TMP_DIR    = "/tmp/distri_data"
	SEP        = "\n"
)

//
// Server
//

type MRService struct {
}

func server() {
	service := &MRService{}
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
		//fmt.Println("accept one... %d", i)
		if err == nil {
			go rpcs.ServeConn(conn)
		} else {
			break
		}
	}
	fmt.Println("finished")
}

func (mr *MRService) Task_get(args *Task_t, reply *Task_t) error {
	r := C.task_get()
	reply.Task_type = C.GoString(r)
	//fmt.Println("task type: " + reply.Task_type)
	if reply.Task_type == "FINISH" {
		fmt.Println("Task accomplished!!")
	}
	return nil
}

func (mr *MRService) Map_get(args *Map_t, reply *Map_t) error {
	r := C.map_get()
	if r == nil {
		return nil
	}
	reply.Name = C.GoString(r)
	fmt.Println("map " + C.GoString(r) + "...")
	return nil
}

func (mr *MRService) Map_finish(args *Map_finish_t, reply *Map_finish_t) error {
	name := C.CString(args.Name)

	file_names := args.File_names
	cStringArray := make([]*C.char, len(file_names))
	for i, str := range file_names {
		cStringArray[i] = C.CString(str)
		defer C.free(unsafe.Pointer(cStringArray[i]))
	}
	cArray := &cStringArray[0]

	C.map_finish(name, cArray)
	fmt.Println("map " + args.Name + " finish")
	return nil
}

func (mr *MRService) Reduce_get(args *Reduce_t, reply *Reduce_t) error {
	rdc := C.reduce_get()
	if rdc == nil {
		return nil
	}
	/*
		reply.Name = C.GoString(rdc.name)
		file_size := int(C.get_mapsize())
		goStrings := make([]string, 0)
		for i := 0; i < file_size; i++ {
			cString := *(**C.char)(unsafe.Pointer(uintptr(unsafe.Pointer(rdc.input_file)) + uintptr(i)*unsafe.Sizeof(rdc.input_file)))
			if cString == nil {
				break
			}
			goStrings = append(goStrings, C.GoString(cString))
		}
		reply.Input_files = goStrings
	*/
	// 传输过来的已经是序列化后的字符串了
	reply.Name = C.GoString(rdc)
	fmt.Println("reduce to " + strings.Split(reply.Name, SEP)[0] + " ...")
	return nil
}

func (mr *MRService) Reduce_finish(args *Reduce_t, reply *Reduce_t) error {
	name := C.CString(args.Name)
	C.reduce_finish(name)
	fmt.Println("reduce " + strings.Split(args.Name, SEP)[0] + " finish")
	return nil
}

func (mr *MRService) Get_mapsize(args *Int_t, reply *Int_t) error {
	r := C.get_mapsize()
	reply.Value = int(r)
	return nil
}

func (mr *MRService) Get_reducesize(args *Int_t, reply *Int_t) error {
	r := C.get_reducesize()
	reply.Value = int(r)
	return nil
}

func main() {
	os.MkdirAll(TMP_DIR, 0777)
	//os.Chmod(TMP_DIR, 0777)
	// 删除一个目录及其所有内容
	os.RemoveAll(OUTPUT_DIR)
	os.MkdirAll(OUTPUT_DIR, 0777)
	//os.Chmod(OUTPUT_DIR, 0777)

	var result_size int

	fmt.Println("<result_size> <file1> <file2> ... <fileN>")
	fmt.Println("example: 8 a.txt b.txt c.txt d.txt")
	reader := bufio.NewReader(os.Stdin)
	fmt.Println("Please input...")
	userInput, _ := reader.ReadString('\n')

	// Remove the line break at the end
	userInput = userInput[:len(userInput)-1]
	fmt.Println("Your input is: " + userInput)

	inputStrings := strings.Fields(userInput)
	result_size, _ = strconv.Atoi(inputStrings[0])
	file_size := len(inputStrings) - 1
	fmt.Println("result_size: ", result_size)
	fmt.Println("file_size: ", file_size)

	cStringArray := make([]*C.char, file_size)
	for i := 0; i < file_size; i++ {
		cStringArray[i] = C.CString(inputStrings[i+1])
		defer C.free(unsafe.Pointer(cStringArray[i]))
	}

	cArray := &cStringArray[0]
	fmt.Println("task init...")
	C.task_init(C.int(file_size), cArray, C.int(result_size))

	fmt.Println("server started...")
	server()
}
