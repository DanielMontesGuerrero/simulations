package main

import (
	"fmt"

	"github.com/DanielMontesGuerrero/simulations/gameoflifeServer/gameoflife"
)

func main() {
	fmt.Println("Hello world ;)")
	game := gameoflife.New(10, 10)
	game.Update()
}
