package main

//
// Common RPC request/reply definitions
//

// RemoveServer(string): string
type RemoveServerArgs struct {
	Key string
}

type RemoveServerReply struct {
	Value string
	Error string
}

// AddServer(string): string
type AddServerArgs struct {
	Key string
}

type AddServerReply struct {
	Value string
	Error string
}

// GetConfig(): string
type GetConfigArgs struct {
}

type GetConfigReply struct {
	Value string
	Error string
}
