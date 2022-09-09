package connect

import (
	"fmt"
	"net"
)

func BaseHandler(connection net.Conn) bool {
	buffer := ReadRaw(connection)
	messageType, event, data := ReadPacket(buffer, len(buffer))
	switch messageType {
	case MESSAGE_PING:
		return handlePing(connection, event, data)
	default:
		fmt.Printf("Error recieving response\n")
	}
	return false
}

func handlePing(connection net.Conn, event byte, data []int) bool {
	fmt.Println("Event: ", event)
	fmt.Println("Data:")
	for i := 0; i < len(data); i++ {
		fmt.Print(data[i], " ")
	}
	fmt.Println("")
	buffer := [3]int{1, 2, 3}
	response := CreateResponsePacket(buffer[:])
	WriteRaw(connection, response)
	return true
}
