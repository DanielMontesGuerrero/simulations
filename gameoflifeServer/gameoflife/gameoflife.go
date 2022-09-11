package gameoflife

import (
	"fmt"
	"math/rand"

	"github.com/DanielMontesGuerrero/simulations/utilsgo"
)

type Rule struct {
	survivalMin int
	survivalMax int
	birthMin    int
	birthMax    int
}

type GameOfLife struct {
	matrix            utilsgo.Matrix
	rule              Rule
	neighborhoodDiffs [][2]int
}

func NewGame(rows int, cols int) *GameOfLife {
	game := new(GameOfLife)
	game.rule.survivalMin = 2
	game.rule.survivalMax = 3
	game.rule.birthMin = 3
	game.rule.birthMax = 3
	game.neighborhoodDiffs = [][2]int{
		{-1, -1},
		{-1, 0},
		{-1, 1},
		{0, -1},
		{0, 1},
		{1, -1},
		{1, 0},
		{1, 1},
	}
	game.matrix = *utilsgo.New(rows, cols)
	for i := 0; i < rows; i++ {
		for j := 0; j < cols; j++ {
			if rand.Intn(2) == 1 {
				game.matrix.Set(i, j, true)
			}
		}
	}
	return game
}

func (game *GameOfLife) Update() {
	prevMatrix := game.matrix
	for i := 0; i < game.matrix.Rows; i++ {
		for j := 0; j < game.matrix.Cols; j++ {
			cnt := game.GetNeighborhoodCount(i, j, prevMatrix)
			isAlive := prevMatrix.Get(i, j) == 1
			newState := false
			if isAlive && game.rule.survivalMin <= cnt && cnt <= game.rule.survivalMax {
				newState = true
			} else if !isAlive && game.rule.birthMin <= cnt && cnt <= game.rule.birthMax {
				newState = true
			}
			game.matrix.Set(i, j, newState)
		}
	}
}

func (game *GameOfLife) GetNeighborhoodCount(i int, j int, matrix utilsgo.Matrix) int {
	ans := 0
	for _, diff := range game.neighborhoodDiffs {
		y := i + diff[0]
		x := j + diff[1]
		if !matrix.AreValidCoords(y, x) {
			continue
		}
		ans += matrix.Get(y, x)
	}
	return ans
}

func (game *GameOfLife) Get(i int, j int) int {
	return game.matrix.Get(i, j)
}

func (game *GameOfLife) Println() {
	for i := 0; i < game.matrix.Rows; i++ {
		for j := 0; j < game.matrix.Cols; j++ {
			if game.Get(i, j) == 1 {
				fmt.Print("1")
			} else {
				fmt.Print("0")
			}
		}
		fmt.Print("\n")
	}
	fmt.Print("\n")
}
