package workers

import (
	"fmt"
	"net"
	"os"

	"github.com/DanielMontesGuerrero/simulations/gameoflifeServer/connect"
	"github.com/DanielMontesGuerrero/simulations/gameoflifeServer/gameoflife"
	"github.com/DanielMontesGuerrero/simulations/utilsgo"
)

type HostData struct {
	Host     string
	Port     int
	Protocol string
}

type Orchestrator struct {
	server     connect.Server
	clients    []connect.Client
	manager    gameoflife.GameManager
	shouldStop bool
}

func NewOrchestrator(rows, cols int, host string, port int, protocol string, hosts []HostData) *Orchestrator {
	orchestrator := new(Orchestrator)
	orchestrator.shouldStop = false
	orchestrator.clients = make([]connect.Client, len(hosts))
	orchestrator.server = *connect.NewServer(host, port, protocol)
	for i := 0; i < len(hosts); i++ {
		orchestrator.clients[i] = *connect.NewClient(hosts[i].Host, hosts[i].Port, hosts[i].Protocol)
	}
	orchestrator.manager = *gameoflife.NewGameManager(rows, cols, len(hosts))
	return orchestrator
}

func (orch *Orchestrator) Run() {
	fmt.Println("Running...")
	for {
		if orch.shouldStop {
			fmt.Println("Stopped updating")
			break
		}
		orch.Update()
	}
}

func (orch *Orchestrator) Update() {
	if orch.manager.ShouldUpdate() {
		orch.UpdateBorders()
		for cliendId := 0; cliendId < len(orch.clients); cliendId++ {
			orch.clients[cliendId].Send(connect.MESSAGE_EVENT, connect.EVENT_UPDATE, []byte{})
		}
	}
}

func (orch *Orchestrator) TogglePause() {
	orch.manager.TogglePause()
}

func (orch *Orchestrator) ToggleCell(i, j int) {
	cliendId := orch.manager.GetNodeIdByIndexes(i, j)
	dy, dx := orch.manager.GetIndexGapByNodeId(cliendId)
	fmt.Println("cliendId:", cliendId)
	orch.clients[cliendId].Send(connect.MESSAGE_EVENT, connect.EVENT_MOUSE_CLICK, connect.IntsToBytes([]int{i - dy, j - dx}))
}

func (orch *Orchestrator) ProcessQueuedEvents() {
	if len(orch.manager.QueuedRequests) > 0 {
		event := orch.manager.QueuedRequests[0]
		orch.manager.QueuedRequests = orch.manager.QueuedRequests[1:]
		switch event.Type {
		case gameoflife.EVENT_TOGGLE_CELL:
			orch.ToggleCell(event.RowUp, event.ColLeft)
		}
	}
}

func (orch *Orchestrator) UpdateBorders() {
	for clientId := 0; clientId < len(orch.clients); clientId++ {
		orch.updateBorder(clientId)
	}
}

func (orch *Orchestrator) IncreaseUpdateRate() {
	orch.manager.IncreaseUpdateRate()
}

func (orch *Orchestrator) DecreaseUpdateRate() {
	orch.manager.DecreaseUpdateRate()
}

func (orch *Orchestrator) updateBorder(clientId int) {
	borderIndexes := orch.manager.GetBorderIndexesOfNode(clientId)
	borders := make([]utilsgo.Vector, 4)
	for i := 0; i < 4; i++ {
		borders[i] = orch.requestBorder(borderIndexes[i])
	}
	fmt.Printf("Sending borders to node %d\n", clientId)
	for i := 0; i < 4; i++ {
		borders[i].Println()
	}
	orch.clients[clientId].Send(connect.MESSAGE_EVENT, connect.EVENT_SET_BORDERS, connect.SerializeBorders(&borders[0], &borders[1], &borders[2], &borders[3]))
}

func (orch *Orchestrator) requestBorder(borderIndexes [3][4]int) utilsgo.Vector {
	vecLen := 0
	for i := 0; i < len(borderIndexes); i++ {
		if borderIndexes[i][0] == borderIndexes[i][1] {
			vecLen += borderIndexes[i][3] - borderIndexes[i][2] + 1
		} else {
			vecLen += borderIndexes[i][1] - borderIndexes[i][0] + 1
		}
	}
	vec := *utilsgo.NewVector(vecLen)
	currSize := 0
	for i := 0; i < len(borderIndexes); i++ {
		var aux utilsgo.Vector
		auxLen := 0
		if borderIndexes[i][0] == borderIndexes[i][1] {
			auxLen = borderIndexes[i][3] - borderIndexes[i][2] + 1
		} else {
			auxLen = borderIndexes[i][1] - borderIndexes[i][0] + 1
		}
		requestClientId := orch.manager.GetNodeIdByIndexes(borderIndexes[i][0], borderIndexes[i][2])
		if requestClientId != -1 {
			fmt.Println("RequestClient:", requestClientId)
			fmt.Println("Requesting global submatrix:", borderIndexes[i])
			borderIndexes[i][0], borderIndexes[i][2] = orch.manager.TranslateGlobalIndexToLocalIndex(requestClientId, borderIndexes[i][0], borderIndexes[i][2])
			borderIndexes[i][1], borderIndexes[i][3] = orch.manager.TranslateGlobalIndexToLocalIndex(requestClientId, borderIndexes[i][1], borderIndexes[i][3])
			fmt.Println("Requesting local submatrix:", borderIndexes[i])
			buffer, _ := orch.clients[requestClientId].Send(connect.MESSAGE_EVENT, connect.EVENT_GET, connect.IntsToBytes(borderIndexes[i][:]))
			aux = utilsgo.MatrixToVector(connect.DeserializeMatrix(buffer))
			if aux.Size != auxLen {
				fmt.Printf("Expected vector size to be %d, received %d", auxLen, aux.Size)
				aux = *utilsgo.NewVector(auxLen)
			}
		} else {
			aux = *utilsgo.NewVector(auxLen)
		}
		fmt.Printf("Recieved vector: ")
		aux.Println()
		for j := 0; j < auxLen; j++ {
			vec.Set(currSize+j, aux.Get(j) == 1)
		}
		currSize += auxLen
	}
	return vec
}

func (orch *Orchestrator) GetSubmatrix(ui, bi, lj, rj int) utilsgo.Matrix {
	rows := bi - ui + 1
	cols := rj - lj + 1
	matrix := utilsgo.New(rows, cols)
	for i := ui; i <= bi; {
		var currUi, currBi, currLj, currRj int
		for j := lj; j <= rj; {
			clientId := orch.manager.GetNodeIdByIndexes(i, j)
			_, clientBi, _, clientRj := orch.manager.GetIndexesOfNode(clientId)
			currUi = i
			currLj = j
			currBi = clientBi
			currRj = clientRj
			if bi < currBi {
				currBi = bi
			}
			if rj < currRj {
				currRj = rj
			}
			indexes := []int{currUi, currBi, currLj, currRj}
			fmt.Printf("Requesting to client %d\n", clientId)
			fmt.Println("global indexes:", indexes)
			localCurrUi, localCurrLj := orch.manager.TranslateGlobalIndexToLocalIndex(clientId, currUi, currLj)
			localCurrBi, localCurrRj := orch.manager.TranslateGlobalIndexToLocalIndex(clientId, currBi, currRj)
			indexes = []int{localCurrUi, localCurrBi, localCurrLj, localCurrRj}
			fmt.Println("local indexes:", indexes)
			buffer, _ := orch.clients[clientId].Send(connect.MESSAGE_EVENT, connect.EVENT_GET, connect.IntsToBytes(indexes))
			auxMatrix := connect.DeserializeMatrix(buffer)
			fmt.Println("Recieved matrix:")
			auxMatrix.Println()
			matrix.SetSubMatrix(currUi-ui, currLj-lj, &auxMatrix)
			j = currRj + 1
		}
		i = currBi + 1
	}
	fmt.Println("Built matrix")
	matrix.Println()
	return *matrix
}

func (orch *Orchestrator) SendLog() {
	for id := 0; id < len(orch.clients); id++ {
		orch.clients[id].Send(connect.MESSAGE_EVENT, connect.EVENT_LOG, []byte{})
	}
}

func (orch *Orchestrator) SendClose() {
	for id := 0; id < len(orch.clients); id++ {
		orch.clients[id].Send(connect.MESSAGE_CLOSE, 0, []byte{})
	}
}

func (orch *Orchestrator) CloseAllClients() {
	orch.SendClose()
}

func (orch *Orchestrator) Close() {
	orch.shouldStop = true
	orch.CloseAllClients()
	orch.server.ShouldStop = true
	fmt.Println("Closed orchestrator")
	os.Exit(0)
}

func (orch *Orchestrator) handleEvent(connection net.Conn, event byte, buffer []byte) bool {
	switch event {
	case connect.EVENT_MOUSE_CLICK:
		data := connect.BytesToInts(buffer)
		if len(data) >= 2 {
			orch.ToggleCell(data[0], data[1])
			connect.SendOkResponse(connection)
		} else {
			connect.SendBadResponse(connection)
		}
	case connect.EVENT_UPDATE_RATE_DECREASE:
		orch.DecreaseUpdateRate()
		connect.SendOkResponse(connection)
	case connect.EVENT_UPDATE_RATE_INCREASE:
		orch.IncreaseUpdateRate()
		connect.SendOkResponse(connection)
	case connect.EVENT_PAUSE:
		orch.TogglePause()
		connect.SendOkResponse(connection)
	case connect.EVENT_LOG:
		orch.manager.Println()
		orch.SendLog()
		connect.SendOkResponse(connection)
	case connect.MESSAGE_CLOSE:
		connect.SendOkResponse(connection)
		fmt.Println("Closing")
		orch.Close()
		fmt.Println("Closed")

	case connect.EVENT_GET:
		data := connect.BytesToInts(buffer)
		matrix := *utilsgo.New(0, 0)
		if len(data) >= 4 {
			matrix = orch.GetSubmatrix(data[0], data[1], data[2], data[3])
		}
		packet := connect.CreateResponsePacket(connect.SerializeMatrix(&matrix))
		connect.WriteRaw(connection, packet)
	default:
		fmt.Printf("Error recieving event\n")
		connect.SendBadResponse(connection)
	}
	return true

}

func (orch *Orchestrator) ListenAndServe() {
	orch.server.Listen()
	orch.server.Serve(orch.handleEvent)
}
