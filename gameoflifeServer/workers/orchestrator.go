package workers

import (
	"fmt"

	"github.com/DanielMontesGuerrero/simulations/gameoflifeServer/connect"
	"github.com/DanielMontesGuerrero/simulations/gameoflifeServer/gameoflife"
	"github.com/DanielMontesGuerrero/simulations/utilsgo"
)

type HostData struct {
	Host string
	Port int
	Protocol string
}

type Orchestrator struct {
	clients []connect.Client
	manager gameoflife.GameManager
}

func NewOrchestrator(rows, cols int, hosts []HostData) *Orchestrator {
	orchestrator := new(Orchestrator)
	orchestrator.clients = make([]connect.Client, len(hosts))
	for i := 0; i < len(hosts); i++ {
		orchestrator.clients[i] = *connect.NewClient(hosts[i].Host, hosts[i].Port, hosts[i].Protocol)
	}
	orchestrator.manager = *gameoflife.NewGameManager(rows, cols, len(hosts))
	return orchestrator
}

func (orch *Orchestrator) UpdateBorders() {
	for clientId := 0; clientId < len(orch.clients); clientId++ {
		orch.updateBorder(clientId)
	}
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
	orch.clients[clientId].Send(connect.MESSAGE_EVENT, connect.EVENT_SET_BORDERS, connect.SerializeBorders(borders[0], borders[1], borders[2], borders[3]))
}

func (orch *Orchestrator) requestBorder(borderIndexes [3][4]int) utilsgo.Vector {
	vecLen := 0
	for i := 0; i < len(borderIndexes); i++{
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
			dy, dx := orch.manager.GetIndexGapByNodeId(requestClientId)
			borderIndexes[i][0] -= dy
			borderIndexes[i][1] -= dy
			borderIndexes[i][2] -= dx
			borderIndexes[i][3] -= dx
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
			vec.Set(currSize + j, aux.Get(j) == 1)
		}
		currSize += auxLen
	}
	return vec
}

func (orch *Orchestrator) SendLog() {
	for id := 0; id < len(orch.clients); id++ {
		orch.clients[id].Send(connect.MESSAGE_LOG, 0, []byte{})
	}
}

func (orch *Orchestrator) SendClose() {
	for id := 0; id < len(orch.clients); id++ {
		orch.clients[id].Send(connect.MESSAGE_CLOSE, 0, []byte{})
	}
}

func (orch *Orchestrator) Close() {
	orch.SendClose()
}
