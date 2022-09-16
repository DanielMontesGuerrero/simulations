package connect

import (
	"fmt"
	"net"
)

func BaseHandler(connection net.Conn, eventHandler func(connection net.Conn, event byte, buffer []byte)bool) bool {
	defer wg.Done()
	messageType, event, data := ReadPacket(connection)
	switch messageType {
	case MESSAGE_PING:
		handlePing(connection, event, data)
	case MESSAGE_EVENT:
		eventHandler(connection, event, data)
	case MESSAGE_CLOSE:
		eventHandler(connection, MESSAGE_CLOSE, data)
	default:
		fmt.Printf("Error recieving message\n")
		SendBadResponse(connection)
		return false
	}
	return true
}

func SendOkResponse(connection net.Conn) {
	WriteRaw(connection, CreateResponsePacket([]int{}))
}

func SendBadResponse(connection net.Conn) {
	WriteRaw(connection, CreateResponsePacket([]int{}))
}

func handlePing(connection net.Conn, event byte, buffer []byte) bool {
	data := BytesToInts(buffer)
	fmt.Println("Event: ", event)
	fmt.Println("Data:")
	for i := 0; i < len(data); i++ {
		fmt.Print(data[i], " ")
	}
	fmt.Println("")
	aux := [3]int{1, 2, 3}
	response := CreateResponsePacket(aux[:])
	WriteRaw(connection, response)
	return true
}
