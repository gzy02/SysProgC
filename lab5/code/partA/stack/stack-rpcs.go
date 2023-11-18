package main

//
// Common RPC request/reply definitions
//

type StackArgs struct {
	Value int
}

type StackReply struct {
	Result int
}
