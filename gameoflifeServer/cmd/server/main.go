package main

import (
	"fmt"

	"github.com/DanielMontesGuerrero/simulations/gameoflifeServer/connect"
)

func main() {
	server := connect.NewServer("localhost", 8080, "tcp")
	fmt.Printf("Listening on port %d...", 8080)
	server.ListenAndServe(connect.BaseHandler)
}
