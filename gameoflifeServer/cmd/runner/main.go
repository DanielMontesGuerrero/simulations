package main

import (
	gameoflife "github.com/DanielMontesGuerrero/simulations/gameoflifeServer/workers"
)

func main() {
	worker := gameoflife.NewWorker(10, 10, "localhost", 8080, "tcp")
	go worker.ListenAndServe()
	for {
		worker.Update()
	}
}
