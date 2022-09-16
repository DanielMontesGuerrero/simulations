package utilsgo

import "fmt"

type Matrix struct {
	Rows int
	Cols int
	grid [][]int
}

func New(rows int, cols int) *Matrix {
	matrix := new(Matrix)
	matrix.Rows = rows
	matrix.Cols = cols
	numOfInts := cols / 32
	if cols%32 > 0 {
		numOfInts++
	}
	matrix.grid = make([][]int, rows)
	for i := 0; i < rows; i++ {
		matrix.grid[i] = make([]int, numOfInts)
	}
	return matrix
}

func (matrix *Matrix) Get(i int, j int) int {
	return (matrix.grid[i][j/32] >> (j % 32)) & 1
}

func (matrix *Matrix) TurnOn(i int, j int) {
	matrix.grid[i][j/32] |= (1 << (j % 32))
}

func (matrix *Matrix) TurnOff(i int, j int) {
	matrix.grid[i][j/32] &= ^(1 << (j % 32))
}

func (matrix *Matrix) Set(i int, j int, val bool) {
	if val {
		matrix.TurnOn(i, j)
	} else {
		matrix.TurnOff(i, j)
	}
}

func (matrix *Matrix) Toggle(i int, j int) {
	matrix.grid[i][j/32] ^= (1 << (j % 32))
}

func (matrix *Matrix) AreValidCoords(i int, j int) bool {
	return !(i < 0 || i >= matrix.Rows || j < 0 || j >= matrix.Cols)
}

func (matrix *Matrix) SubMatrix(ui, di, lj, rj int) *Matrix {
	nMatrix := New(di-ui+1, rj-lj+1)
	for i := 0; i < nMatrix.Rows; i++ {
		for j := 0; j < nMatrix.Cols; j++ {
			nMatrix.Set(i, j, matrix.Get(i+ui, j+lj) == 1)
		}
	}
	return nMatrix
}

func (matrix *Matrix) Println() {
	for i := 0; i < matrix.Rows; i++ {
		for j := 0; j < matrix.Cols; j++ {
			if matrix.Get(i, j) == 1 {
				fmt.Print("1")
			} else {
				fmt.Print("0")
			}
		}
		fmt.Print("\n")
	}
	fmt.Print("\n")
}

func (matrix *Matrix) SetSubMatrix(offsetI, offsetJ int, auxMatrix *Matrix) {
	for i := 0; i < auxMatrix.Rows; i++ {
		if offsetI+i >= matrix.Rows {
			continue
		}
		for j := 0; j < auxMatrix.Cols; j++ {
			if offsetJ+j < matrix.Cols {
				matrix.Set(offsetI+i, offsetJ+j, auxMatrix.Get(i, j) == 1)
			}
		}
	}
}
