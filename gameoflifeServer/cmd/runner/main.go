package main

import (
	"fmt"

	"github.com/DanielMontesGuerrero/simulations/gameoflifeServer/gameoflife"
)

func main() {
	fmt.Println("Hello world ;)")
	game := gameoflife.New(10, 10)
	for k := 0; k < 10; k++ {
		for i := 0; i < 10; i++ {
			for j := 0; j < 10; j++ {
				if game.Get(i, j) == 1 {
					fmt.Print("1")
				} else {
					fmt.Print("0")
				}
			}
			fmt.Print("\n")
		}
		fmt.Print("\n")
		game.Update()
	}
}
