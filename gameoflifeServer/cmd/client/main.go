package main

import (
	"flag"
	"fmt"

	"github.com/DanielMontesGuerrero/simulations/gameoflifeServer/connect"
)

var shouldStopOrchestrator bool

func init(){
	flag.BoolVar(&shouldStopOrchestrator, "stop", false, "If true, stops the orchestrator and the workers")
	flag.Parse()
}

func sendLog(client connect.Client){
	client.Send(connect.MESSAGE_EVENT, connect.EVENT_LOG, []byte{})
	fmt.Println("Sent log")
}

func sendPause(client connect.Client){
	client.Send(connect.MESSAGE_EVENT, connect.EVENT_PAUSE, []byte{})
	fmt.Println("Sent pause")
}
func sendMouseClick(client connect.Client, i, j int){
	client.Send(connect.MESSAGE_EVENT, connect.EVENT_MOUSE_CLICK, connect.IntsToBytes([]int{i, j}))
	fmt.Println("Sent mouse_click")
}
func sendUpdateDecrease(client connect.Client){
	client.Send(connect.MESSAGE_EVENT, connect.EVENT_UPDATE_RATE_DECREASE, []byte{})
	fmt.Println("Sent Update decrease")
}
func sendUpdateIncrease(client connect.Client){
	client.Send(connect.MESSAGE_EVENT, connect.EVENT_UPDATE_RATE_INCREASE, []byte{})
	fmt.Println("Sent Update increase")
}

func main() {
	client := *connect.NewClient("localhost", 3000, "tcp")

	sendLog(client)
	sendMouseClick(client, 6, 6)
	sendLog(client)
	sendPause(client)
	sendLog(client)
	sendUpdateDecrease(client)
	sendLog(client)
	sendUpdateIncrease(client)
	sendLog(client)

	if shouldStopOrchestrator {
		client.Send(connect.MESSAGE_CLOSE, 0, []byte{})
	}
}
