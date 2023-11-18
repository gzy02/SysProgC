package main

//
// Common RPC request/reply definitions
//

type StringArgs struct {
	Str string
}

type StringReply struct {
	Result int
}