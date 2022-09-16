package workers

import (
	"fmt"
	"net"
	"os"

	"github.com/DanielMontesGuerrero/simulations/gameoflifeServer/connect"
	"github.com/DanielMontesGuerrero/simulations/gameoflifeServer/gameoflife"
)

type Worker struct {
	game   gameoflife.GameOfLife
	server connect.Server
}

func NewWorker(rows int, cols int, host string, port int, protocol string) *Worker {
	worker := new(Worker)
	worker.game = *gameoflife.NewGame(rows, cols)
	worker.server = *connect.NewServer(host, port, protocol)
	return worker
}

func (worker *Worker) Update() {
	worker.game.Update()
}

func (worker *Worker) ListenAndServe() {
	worker.server.Listen()
	worker.server.Serve(worker.handleEvent)
}

func (worker *Worker) Close() {
	worker.server.ShouldStop = true
	fmt.Println("Closed worker")
	os.Exit(0)
}

func (worker *Worker) handleEvent(connection net.Conn, event byte, buffer []byte) bool {
	switch event {
	case connect.EVENT_MOUSE_CLICK:
		data := connect.BytesToInts(buffer)
		if len(data) >= 2 {
			worker.game.ToggleCell(data[0]+1, data[1]+1)
			connect.SendOkResponse(connection)
		} else {
			connect.SendBadResponse(connection)
		}
	case connect.EVENT_GET:
		data := connect.BytesToInts(buffer)
		if len(data) >= 4 {
			subgrid := worker.game.GetSubgrid(data[0]+1, data[1]+1, data[2]+1, data[3]+1)
			fmt.Println("Sending submatrix:")
			subgrid.Println()
			buffer := connect.SerializeMatrix(&subgrid)
			response := connect.CreateResponsePacket(buffer)
			connect.WriteRaw(connection, response)
		} else {
			connect.SendBadResponse(connection)
		}
	case connect.EVENT_SET_BORDERS:
		top, bottom, left, right := connect.DeserializeBorders(buffer)
		worker.game.SetBorders(top, bottom, left, right)
		connect.SendOkResponse(connection)
	case connect.EVENT_UPDATE:
		worker.game.Update()
		connect.SendOkResponse(connection)
	case connect.EVENT_LOG:
		worker.game.Println()
		connect.SendOkResponse(connection)
	case connect.MESSAGE_CLOSE:
		connect.SendOkResponse(connection)
		worker.Close()
	default:
		fmt.Printf("Error recieving event\n")
		connect.SendBadResponse(connection)
	}
	return true
}
