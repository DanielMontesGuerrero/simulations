package connect

import (
	"fmt"
	"net"
	"sync"
)

var wg sync.WaitGroup

type Server struct {
	listener net.Listener
	host     string
	port     int
	protocol string
	ShouldStop bool
}

func NewServer(host string, port int, protocol string) *Server {
	server := new(Server)
	server.host = host
	server.port = port
	server.protocol = protocol
	server.ShouldStop = false
	return server
}

func (server *Server) Listen() {
	var err error
	server.listener, err = net.Listen(server.protocol, fmt.Sprintf("%s:%d", server.host, server.port))
	if server.listener == nil || err != nil {
		fmt.Println("Listener is nil in Listen()")
		panic(err)
	} else {
		fmt.Printf("Listening on port: %d...", server.port)
	}
}

func (server *Server) Close() {
	wg.Wait()
	server.listener.Close()
	fmt.Println("Closed server")
}

func (server *Server) Serve(eventHandler func(connection net.Conn, event byte, buffer []byte)bool) {
	for {
		fmt.Println("ShouldStop:", server.ShouldStop)
		if server.ShouldStop {
			fmt.Println("Stopped serving")
			server.Close()
			break
		}
		if server.listener == nil {
			fmt.Println("Listener is nil in Serve()")
			panic(server.listener)
		}
		connection, err := server.listener.Accept()
		if err != nil {
			fmt.Printf("Error accepting connection: %s\n", err.Error())
			continue
		}
		wg.Add(1)
		go BaseHandler(connection, eventHandler)
	}
}
