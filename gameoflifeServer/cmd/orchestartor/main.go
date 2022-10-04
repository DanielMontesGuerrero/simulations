package main

import (
	"flag"
	"os"
	"strconv"
	"strings"

	"github.com/DanielMontesGuerrero/simulations/gameoflifeServer/workers"
)

var hostsData []workers.HostData
var rows int
var cols int
var host string
var port int
var protocol string
var debug bool
var standAlone bool
var shouldModuleIndexes bool
var help bool

func init() {
	var hostsRaw string
	var portsRaw string
	flag.BoolVar(&help, "help", false, "Show usage")
	flag.IntVar(&rows, "rows", 10, "Number of rows for the orchestrator")
	flag.IntVar(&cols, "cols", 10, "Number of cols for the orchestrator")
	flag.StringVar(&host, "host", "localhost", "The host ip for the orchestrator")
	flag.IntVar(&port, "port", 3000, "The port for the orchestrator")
	flag.StringVar(&hostsRaw, "hosts", "localhost,localhost,localhost,localhost", "The host ip list for the workers")
	flag.StringVar(&portsRaw, "ports", "8080,8081,8082,8083", "The ports list for the workers")
	flag.StringVar(&protocol, "protocol", "tcp", "The protocol to use")
	flag.BoolVar(&debug, "debug", false, "If set, doesn't print to Stdout")
	flag.BoolVar(&shouldModuleIndexes, "shouldModule", false, "If set, will module cell indexes when updating")
	flag.BoolVar(&standAlone, "standalone", false, "If set, the orchestrator will update workers periodically without waiting for GameHandler")
	flag.Parse()
	hosts := strings.Split(hostsRaw, ",")
	ports := strings.Split(portsRaw, ",")
	hostsData = make([]workers.HostData, len(hosts))
	if help {
		flag.Usage()
	}
	if !debug {
		os.Stdout = nil
	}
	for i := 0; i < len(hosts); i++ {
		port, _ := strconv.Atoi(ports[i])
		hostsData[i] = workers.HostData{Host: hosts[i], Port: port, Protocol: protocol}
	}

}

func main() {
	orch := workers.NewOrchestrator(rows, cols, host, port, protocol, hostsData, shouldModuleIndexes)
	if standAlone {
		go orch.Run()
	}
	orch.ListenAndServe()
}
