package connect

import (
	"fmt"
	"net"

	"github.com/DanielMontesGuerrero/simulations/gameoflifeServer/gameoflife"
)

func BaseHandler(connection net.Conn, game *gameoflife.GameOfLife) bool {
	for {
		messageType, event, data := ReadPacket(connection)
		switch messageType {
		case MESSAGE_PING:
			handlePing(connection, event, data)
		case MESSAGE_EVENT:
			handleEvent(connection, event, data, game)
		case MESSAGE_LOG:
			game.Println()
			sendOkResponse(connection)
		case MESSAGE_CLOSE:
			sendOkResponse(connection)
			return true
		default:
			fmt.Printf("Error recieving message\n")
			sendBadResponse(connection)
			return false
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

func handleEvent(connection net.Conn, event byte, buffer []byte, game *gameoflife.GameOfLife) bool {
	switch event {
	case EVENT_MOUSE_CLICK:
		data := BytesToInts(buffer)
		if len(data) >= 2 {
			game.ToggleCell(data[0]+1, data[1]+1)
			sendOkResponse(connection)
		} else {
			sendBadResponse(connection)
		}
	case EVENT_GET:
		data := BytesToInts(buffer)
		if len(data) >= 4 {
			subgrid := game.GetSubgrid(data[0]+1, data[1]+1, data[2]+1, data[3]+1)
			fmt.Println("Sending submatrix:")
			subgrid.Println()
			buffer := SerializeMatrix(subgrid)
			response := CreateResponsePacket(buffer)
			WriteRaw(connection, response)
		} else {
			sendBadResponse(connection)
		}
	case EVENT_SET_BORDERS:
		top, bottom, left, right := DeserializeBorders(buffer)
		game.SetBorders(top, bottom, left, right)
		sendOkResponse(connection)
	case EVENT_UPDATE:
		game.Update()
		sendOkResponse(connection)
	default:
		fmt.Printf("Error recieving event\n")
		sendBadResponse(connection)
	}
	return true
}
