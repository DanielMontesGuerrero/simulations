package utilsgo

import "fmt"

type Vector struct {
	Size int
	data []int
}

func NewVector(size int) *Vector {
	vector := new(Vector)
	vector.Size = size
	numOfInts := size / 32
	if size%32 > 0 {
		numOfInts++
	}
	vector.data = make([]int, numOfInts)
	return vector
}

func (vector *Vector) TurnOn(i int) {
	vector.data[i/32] |= (1 << (i % 32))
}

func (vector *Vector) TurnOff(i int) {
	vector.data[i/32] &= ^(1 << (i % 32))
}

func (vector *Vector) Get(i int) int {
	return (vector.data[i/32] >> (i % 32)) & 1
}

func (vector *Vector) Set(i int, val bool) {
	if val {
		vector.TurnOn(i)
	} else {
		vector.TurnOff(i)
	}
}

func (vector *Vector) Println() {
	for i := 0; i < vector.Size; i++ {
		fmt.Print(vector.Get(i))
	}
	fmt.Println()
}
