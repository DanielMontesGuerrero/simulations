package connect

import (
	"fmt"
	"net"

	"github.com/DanielMontesGuerrero/simulations/gameoflifeServer/gameoflife"
)

func BaseHandler(connection net.Conn, manager *gameoflife.GameManager) bool {
	for {
		messageType, event, data := ReadPacket(connection)
		switch messageType {
		case MESSAGE_PING:
			handlePing(connection, event, data)
		case MESSAGE_EVENT:
			handleEvent(connection, event, data, manager)
		case MESSAGE_LOG:
			manager.Println()
			sendOkResponse(connection)
		case MESSAGE_CLOSE:
			sendOkResponse(connection)
			return true
		default:
			fmt.Printf("Error recieving response\n")
		}
	}
}

func sendOkResponse(connection net.Conn) {
	WriteRaw(connection, CreateResponsePacket([]int{}))
}

func sendBadResponse(connection net.Conn) {
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

func handleEvent(connection net.Conn, event byte, buffer []byte, manager *gameoflife.GameManager) bool {
	switch event {
	case EVENT_PAUSE:
		manager.TogglePause()
		sendOkResponse(connection)
	case EVENT_MOUSE_CLICK:
		data := BytesToInts(buffer)
		if len(data) >= 2 {
			manager.SetCell(data[0], data[1])
			sendOkResponse(connection)
		} else {
			sendBadResponse(connection)
		}
	case EVENT_UPDATE_RATE_INCREASE:
		manager.IncreaseUpdateRate()
		sendOkResponse(connection)
	case EVENT_UPDATE_RATE_DECREASE:
		manager.DecreaseUpdateRate()
		sendOkResponse(connection)
	case EVENT_GET:
		data := BytesToInts(buffer)
		if len(data) >= 4 {
			subgrid := manager.GetSubgrid(data[0], data[1], data[2], data[3])
			buffer := SerializeMatrix(subgrid)
			response := CreateResponsePacket(buffer)
			WriteRaw(connection, response)
		} else {
			sendBadResponse(connection)
		}
	case EVENT_SET_BORDERS:
		top, bottom, left, right := DeserializeBorders(buffer)
		manager.SetBorders(top, bottom, left, right)
		sendOkResponse(connection)
	case EVENT_UPDATE:
		manager.SingleUpdate()
		sendOkResponse(connection)
	}
	return true
}
