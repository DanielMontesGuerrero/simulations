package main

import (
	"flag"

	"github.com/DanielMontesGuerrero/simulations/gameoflifeServer/workers"
)

var rows int
var cols int
var host string
var port int
var protocol string

func init() {
	flag.IntVar(&rows, "rows", 5, "Number of rows for the worker")
	flag.IntVar(&cols, "cols", 5, "Number of cols for the worker")
	flag.StringVar(&host, "host", "localhost", "The host ip for the worker")
	flag.IntVar(&port, "port", 8080, "The port for the worker")
	flag.StringVar(&protocol, "protocol", "tcp", "The protocol for the worker")
	flag.Parse()
}

func main() {
	worker := workers.NewWorker(rows, cols, host, port, protocol)
	worker.ListenAndServe()
}
