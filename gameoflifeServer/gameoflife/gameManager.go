package gameoflife

import (
	"time"

	"github.com/DanielMontesGuerrero/simulations/utilsgo"
)

type GameManager struct {
	game                GameOfLife
	isPaused            bool
	updateRateMs        int64
	lastUpdateTimestamp int64
}

func NewGameManager(rows int, cols int) *GameManager {
	manager := new(GameManager)
	manager.game = *NewGame(rows, cols)
	manager.isPaused = true
	manager.updateRateMs = 500
	manager.lastUpdateTimestamp = 0
	return manager
}

func (manager *GameManager) Update() {
	timeSinceLastUpdate := time.Now().Unix() - manager.lastUpdateTimestamp
	if !manager.isPaused && timeSinceLastUpdate >= manager.updateRateMs {
		manager.game.Update()
		manager.lastUpdateTimestamp = time.Now().Unix()
	}
}

func (manager *GameManager) TogglePause() {
	manager.isPaused = !manager.isPaused
}

func (manager *GameManager) Println() {
	println("Paused: ", manager.isPaused)
	println("UpdateRateMs: ", manager.updateRateMs)
	manager.game.Println()
}

func (manager *GameManager) SetCell(i int, j int) {
	manager.game.matrix.Toggle(i, j)
}

func (manager *GameManager) IncreaseUpdateRate() {
	manager.updateRateMs = utilsgo.Max(MIN_UPDATE_RATE_MS, manager.updateRateMs-SPEED_FACTOR)
}

func (manager *GameManager) DecreaseUpdateRate() {
	manager.updateRateMs = utilsgo.Min(MAX_UPDATE_RATE_MS, manager.updateRateMs+SPEED_FACTOR)
}

func (manager *GameManager) GetSubgrid(ui, di, lj, rj int) utilsgo.Matrix {
	return *manager.game.matrix.SubMatrix(ui, di, lj, rj)
}
