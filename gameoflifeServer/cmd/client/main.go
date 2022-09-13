package main

import (
	"fmt"

	"github.com/DanielMontesGuerrero/simulations/gameoflifeServer/connect"
	"github.com/DanielMontesGuerrero/simulations/utilsgo"
)

func main() {
	client := connect.NewClient("localhost", 8080, "tcp")
	defer client.Close()
	event := byte(7)
	data := [6]int{1, 2, 3, 4, 5, 6}
	response, resLen := client.Send(connect.MESSAGE_PING, event, connect.IntsToBytes(data[:]))
	fmt.Println("Len: ", resLen)
	fmt.Println("Response: ", resLen)
	for i := 0; i < resLen; i++ {
		fmt.Print(response[i], " ")
	}
	fmt.Println()
	client.Send(connect.MESSAGE_LOG, 0, connect.IntsToBytes([]int{}))
	fmt.Println("Sent log")
	client.Send(connect.MESSAGE_EVENT, connect.EVENT_PAUSE, connect.IntsToBytes([]int{}))
	fmt.Println("Sent pause")
	client.Send(connect.MESSAGE_LOG, 0, connect.IntsToBytes([]int{}))
	fmt.Println("Sent log")

	client.Send(connect.MESSAGE_EVENT, connect.EVENT_PAUSE, connect.IntsToBytes([]int{}))
	client.Send(connect.MESSAGE_LOG, 0, connect.IntsToBytes([]int{}))
	fmt.Println("Sent log")
	client.Send(connect.MESSAGE_EVENT, connect.EVENT_MOUSE_CLICK, connect.IntsToBytes([]int{0, 0}))
	fmt.Println("Sent mouse_click")
	client.Send(connect.MESSAGE_LOG, 0, connect.IntsToBytes([]int{}))
	fmt.Println("Sent log")

	client.Send(connect.MESSAGE_EVENT, connect.EVENT_UPDATE_RATE_DECREASE, connect.IntsToBytes([]int{}))
	fmt.Println("Sent Update decrease")
	client.Send(connect.MESSAGE_LOG, 0, connect.IntsToBytes([]int{}))
	fmt.Println("Sent log")
	client.Send(connect.MESSAGE_EVENT, connect.EVENT_UPDATE_RATE_INCREASE, connect.IntsToBytes([]int{}))
	fmt.Println("Sent Update increase")
	client.Send(connect.MESSAGE_LOG, 0, connect.IntsToBytes([]int{}))
	fmt.Println("Sent log")

	response, _ = client.Send(connect.MESSAGE_EVENT, connect.EVENT_GET, connect.IntsToBytes([]int{0, 5, 0, 5}))
	fmt.Println("Sent get")
	matrix := connect.DeserializeMatrix(response)
	matrix.Println()

	vec := *utilsgo.NewVector(12)
	vec.Set(0, true)
	vec.Set(11, true)
	packet := connect.SerializeBorders(vec, vec, vec, vec)
	client.Send(connect.MESSAGE_EVENT, connect.EVENT_SET_BORDERS, packet)
	fmt.Println("Sent borders")
	client.Send(connect.MESSAGE_LOG, 0, connect.IntsToBytes([]int{}))
	fmt.Println("Sent log")

	client.Send(connect.MESSAGE_EVENT, connect.EVENT_UPDATE, []byte{})
	fmt.Println("Sent single update")
	client.Send(connect.MESSAGE_LOG, 0, connect.IntsToBytes([]int{}))
	fmt.Println("Sent log")

	client.Send(connect.MESSAGE_CLOSE, 0, packet)
}
