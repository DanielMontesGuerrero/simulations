package gameoflife

import (
	"github.com/DanielMontesGuerrero/simulations/gameoflifeServer/connect"
	"github.com/DanielMontesGuerrero/simulations/gameoflifeServer/gameoflife"
)

type Worker struct {
	manager gameoflife.GameManager
	server  connect.Server
}

func NewWorker(rows int, cols int, host string, port int, protocol string) *Worker {
	worker := new(Worker)
	worker.manager = *gameoflife.NewGameManager(rows, cols)
	worker.server = *connect.NewServer(host, port, protocol)
	return worker
}

func (worker *Worker) Update() {
	worker.manager.Update()
}

func (worker *Worker) ListenAndServe() {
	worker.server.Listen()
	worker.server.Serve(&worker.manager)
}
