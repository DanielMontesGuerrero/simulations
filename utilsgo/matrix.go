package utilsgo

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
