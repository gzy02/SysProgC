package main

//
// Common RPC request/reply definitions
//

type Map_t struct {
	Name string
}

type Map_finish_t struct {
	Name       string
	File_names []string
}

type Reduce_t struct {
	Name        string
	Input_files []string
}

type Task_t struct {
	Task_type string
}

type Int_t struct {
	Value int
}
