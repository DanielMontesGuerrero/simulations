package utilsgo

func Max(a int64, b int64) int64 {
	if a > b {
		return a
	}
	return b
}

func Min(a int64, b int64) int64 {
	if a < b {
		return a
	}
	return b
}

func MatrixToVector(matrix *Matrix) *Vector {
	vecLen := int(Max(int64(matrix.Rows), int64(matrix.Cols)))
	vec := NewVector(vecLen)
	for i := 0; i < vecLen; i++ {
		if matrix.Rows == vecLen {
			vec.Set(i, matrix.Get(i, 0) == 1)
		} else {
			vec.Set(i, matrix.Get(0, i) == 1)
		}
	}
	return vec
}
