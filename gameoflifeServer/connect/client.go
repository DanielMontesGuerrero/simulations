package connect

import (
	"fmt"
	"net"
)

type Client struct {
	connection net.Conn
	host       string
	port       int
	protocol   string
}

func NewClient(host string, port int, protocol string) *Client {
	client := new(Client)
	client.host = host
	client.port = port
	client.protocol = protocol
	return client
}

func (client *Client) Dial() net.Conn {
	for {
		conn, err := net.Dial(client.protocol, fmt.Sprintf("%s:%d", client.host, client.port))
		if err == nil {
			client.connection = conn
			return conn
		}
	}
}

func (client *Client) Send(messageType byte, event byte, data []int) ([]byte, int) {
	if client.connection == nil {
		client.connection = client.Dial()
	}
	packet := CreatePacket(messageType, event, data)
	WriteRaw(client.connection, packet)
	return ReadResponsePacket(client.connection)
}

func (client *Client) Close() {
	client.connection.Close()
}
