package util

//
// Common RPC request/reply definitions
//
type Delete_kv_rangeArgs struct {
	L int
	R int
}

type Delete_kv_rangeReply struct {
	Value string
}

type Get_all_kv_Args struct {
}

type Get_all_kv_Reply struct {
	Value string
}

// Put(string, string): void
type PutArgs struct {
	Key   string
	Value string
}

type PutReply struct {
}

// Get(string): string
type GetArgs struct {
	Key string
}

type GetReply struct {
	Value string
	Error string
}

// Delete(string): void
type DeleteArgs struct {
	Key string
}

type DeleteReply struct {
}
