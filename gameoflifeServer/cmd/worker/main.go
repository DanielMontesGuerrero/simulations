package main

import (
	"flag"
	"os"

	"github.com/DanielMontesGuerrero/simulations/gameoflifeServer/workers"
)

var rows int
var cols int
var host string
var port int
var protocol string
var debug bool

func init() {
	flag.IntVar(&rows, "rows", 5, "Number of rows for the worker")
	flag.IntVar(&cols, "cols", 5, "Number of cols for the worker")
	flag.StringVar(&host, "host", "localhost", "The host ip for the worker")
	flag.IntVar(&port, "port", 8080, "The port for the worker")
	flag.StringVar(&protocol, "protocol", "tcp", "The protocol for the worker")
	flag.BoolVar(&debug, "debug", false, "If set, doesn't print to Stdout")
	flag.Parse()
	if !debug {
		os.Stdout = nil
	}
}

func main() {
	worker := workers.NewWorker(rows, cols, host, port, protocol)
	worker.ListenAndServe()
}
