package connect

import (
	"fmt"
	"net"

	"github.com/DanielMontesGuerrero/simulations/gameoflifeServer/gameoflife"
)

type Server struct {
	listener net.Listener
	host     string
	port     int
	protocol string
}

func NewServer(host string, port int, protocol string) *Server {
	server := new(Server)
	server.host = host
	server.port = port
	server.protocol = protocol
	return server
}

func (server *Server) Listen() {
	var err any
	server.listener, err = net.Listen(server.protocol, fmt.Sprintf("%s:%d", server.host, server.port))
	if server.listener == nil || err != nil {
		fmt.Println("Listener is nil in Listen()")
		panic(err)
	}
}

func (server *Server) Close() {
	server.listener.Close()
}

func (server *Server) Serve(manager *gameoflife.GameManager) {
	defer server.listener.Close()
	for {
		if server.listener == nil {
			fmt.Println("Listener is nil in Serve()")
			panic(server.listener)
		}
		connection, err := server.listener.Accept()
		if err != nil {
			panic(err)
		}
		go BaseHandler(connection, manager)
	}
}
