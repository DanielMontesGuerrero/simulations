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