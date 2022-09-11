package main

import (
	"fmt"

	"github.com/DanielMontesGuerrero/simulations/gameoflifeServer/connect"
)

func main() {
	client := connect.NewClient("localhost", 8080, "tcp")
	defer client.Close()
	event := byte(7)
	data := [6]int{1, 2, 3, 4, 5, 6}
	response, resLen := client.Send(connect.MESSAGE_PING, event, data[:])
	fmt.Println("Len: ", resLen)
	fmt.Println("Response: ", resLen)
	for i := 0; i < resLen; i++ {
		fmt.Print(response[i], " ")
	}
	fmt.Println()
	client.Send(connect.MESSAGE_LOG, 0, []int{})
	fmt.Println("Sent log")
	client.Send(connect.MESSAGE_EVENT, connect.EVENT_PAUSE, []int{})
	fmt.Println("Sent pause")
	client.Send(connect.MESSAGE_LOG, 0, []int{})
	fmt.Println("Sent log")

	client.Send(connect.MESSAGE_EVENT, connect.EVENT_PAUSE, []int{})
	client.Send(connect.MESSAGE_LOG, 0, []int{})
	fmt.Println("Sent log")
	client.Send(connect.MESSAGE_EVENT, connect.EVENT_MOUSE_CLICK, []int{0, 0})
	fmt.Println("Sent mouse_click")
	client.Send(connect.MESSAGE_LOG, 0, []int{})
	fmt.Println("Sent log")

	client.Send(connect.MESSAGE_CLOSE, 0, []int{})
}
