package gameoflife

import (
	"time"

	"github.com/DanielMontesGuerrero/simulations/utilsgo"
)

type GameManager struct {
	rows int
	cols int
	numOfNodes int
	numDivsY int
	numDivsX int
	isPaused            bool
	updateRateMs        int64
	lastUpdateTimestamp int64
	shouldModuleIndexes bool
}

type UpdateRequest struct {
	nodeId int
}

func NewGameManager(rows, cols, numOfNodes int) *GameManager {
	manager := new(GameManager)
	manager.rows = rows
	manager.cols = cols
	manager.numOfNodes = numOfNodes
	for i := 1; i * i <= numOfNodes; i++ {
		if numOfNodes % i == 0 {
			manager.numDivsY = i
			manager.numDivsX = numOfNodes / i
		}
	}
	manager.isPaused = true
	manager.updateRateMs = 500
	manager.lastUpdateTimestamp = 0
	manager.shouldModuleIndexes = false
	return manager
}

func (manager *GameManager) Update() []UpdateRequest {
	timeSinceLastUpdate := time.Now().Unix() - manager.lastUpdateTimestamp
	requests := make([]UpdateRequest, manager.numOfNodes)
	if !manager.isPaused && timeSinceLastUpdate >= manager.updateRateMs {
		manager.lastUpdateTimestamp = time.Now().Unix()
		for id := 0; id < manager.numOfNodes; id++ {
			requests = append(requests, UpdateRequest{id})
		}
	}
	return requests
}

func (manager *GameManager) GetIndexesOfNode(nodeId int) (int, int, int, int) {
	rowIndex := nodeId / manager.numDivsY
	colIndex := nodeId % manager.numDivsX
	ui := (manager.rows / manager.numDivsY) * rowIndex
	bi := ui + (manager.rows / manager.numDivsY) - 1
	lj := (manager.cols / manager.numDivsX) * colIndex
	rj := lj + (manager.cols / manager.numDivsX) - 1
	return ui, bi, lj, rj
}

func (manager *GameManager) GetBorderIndexesOfNode(nodeId int) [4][3][4]int {
	ui, bi, lj, rj := manager.GetIndexesOfNode(nodeId)
	upperLine := ui - 1
	bottomLine := bi + 1
	leftLine := lj - 1
	rightLine := rj + 1
	auxlj := lj - 1
	auxrj := rj + 1
	auxui := ui - 1
	auxbi := bi + 1
	if manager.shouldModuleIndexes {
		upperLine = (upperLine + manager.rows) % manager.rows;
		bottomLine = (bottomLine + manager.rows) % manager.rows
		leftLine = (leftLine + manager.cols) % manager.cols
		rightLine = (rightLine + manager.cols) % manager.cols
		auxui = (auxui + manager.rows) % manager.rows
		auxbi = (auxbi + manager.rows) % manager.rows
		auxlj = (auxlj + manager.cols) % manager.cols
		auxrj = (auxrj + manager.cols) % manager.cols
	}
	return [4][3][4]int{
		{
			{upperLine, upperLine, auxlj, auxlj},
			{upperLine, upperLine, lj, rj},
			{upperLine, upperLine, auxrj, auxrj},
		},
		{
			{bottomLine, bottomLine, auxlj, auxlj},
			{bottomLine, bottomLine, lj, rj},
			{bottomLine, bottomLine, auxrj, auxrj},
		},
		{
			{auxui, auxui, leftLine, leftLine},
			{ui, bi, leftLine, leftLine},
			{auxbi, auxbi, leftLine, leftLine},
		},
		{
			{auxui, auxui, rightLine, rightLine},
			{ui, bi, rightLine, rightLine},
			{auxbi, auxbi, rightLine, rightLine},
		},
	}
}

func (manager *GameManager) GetNodeIdByIndexes(i, j int) int {
	if i < 0 || i >= manager.rows || j < 0 || j >= manager.cols {
		return -1
	}
	rowIndex := i / (manager.rows /  manager.numDivsY)
	colIndex := j / (manager.cols / manager.numDivsX)
	return rowIndex * manager.numDivsX + colIndex
}

func (manager *GameManager) GetIndexGapByNodeId(nodeId int) (int, int) {
	ui, _, lj, _ := manager.GetIndexesOfNode(nodeId)
	return ui, lj
}

func (manager *GameManager) TogglePause() {
	manager.isPaused = !manager.isPaused
}

func (manager *GameManager) Println() {
	println("Paused: ", manager.isPaused)
	println("UpdateRateMs: ", manager.updateRateMs)
}

func (manager *GameManager) IncreaseUpdateRate() {
	manager.updateRateMs = utilsgo.Max(MIN_UPDATE_RATE_MS, manager.updateRateMs-SPEED_FACTOR)
}

func (manager *GameManager) DecreaseUpdateRate() {
	manager.updateRateMs = utilsgo.Min(MAX_UPDATE_RATE_MS, manager.updateRateMs+SPEED_FACTOR)
}
