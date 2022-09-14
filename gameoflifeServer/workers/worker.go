package workers

import (
	"github.com/DanielMontesGuerrero/simulations/gameoflifeServer/connect"
	"github.com/DanielMontesGuerrero/simulations/gameoflifeServer/gameoflife"
)

type Worker struct {
	game gameoflife.GameOfLife
	server  connect.Server
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
	worker.server.Serve(&worker.game)
}
