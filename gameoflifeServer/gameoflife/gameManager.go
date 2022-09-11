package gameoflife

type GameManager struct {
	game     GameOfLife
	isPaused bool
}

func NewGameManager(rows int, cols int) *GameManager {
	manager := new(GameManager)
	manager.game = *NewGame(rows, cols)
	manager.isPaused = true
	return manager
}

func (manager *GameManager) Update() {
	if !manager.isPaused {
		manager.game.Update()
	}
}

func (manager *GameManager) TogglePause() {
	manager.isPaused = !manager.isPaused
}

func (manager *GameManager) Println() {
	println("Paused: ", manager.isPaused)
	manager.game.Println()
}

func (manager *GameManager) SetCell(i int, j int) {
	manager.game.matrix.Toggle(i, j)
}
