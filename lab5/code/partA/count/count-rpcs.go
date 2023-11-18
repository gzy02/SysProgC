package main

type IncrementArgs struct{

}
type IncrementReply struct {
	Count int
}

type DecrementArgs struct{
	
}
type DecrementReply struct {
	Count int
}

type GetCountArgs struct {

}

type GetCountReply struct {
	Count int
}