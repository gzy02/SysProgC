package main

//
// Common RPC request/reply definitions
//

type SquareArgs struct {
	Value int
}

type SquareReply struct {
	Result int
}
