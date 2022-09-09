package main

import (
	"fmt"

	"github.com/DanielMontesGuerrero/simulations/gameoflifeServer/connect"
)

func main() {
	client := connect.NewClient("localhost", 8080, "tcp")
	event := byte(7)
	data := [6]int{1, 2, 3, 4, 5, 6}
	response, resLen := client.Send(connect.MESSAGE_PING, event, data[:])
	fmt.Println("Len: ", resLen)
	fmt.Println("Response: ", resLen)
	for i := 0; i < resLen; i++ {
		fmt.Print(response[i], " ")
	}
	fmt.Println()
}
