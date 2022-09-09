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

func (client Client) Dial() net.Conn {
	conn, err := net.Dial(client.protocol, fmt.Sprintf("%s:%d", client.host, client.port))
	if err != nil {
		panic(err)
	}
	return conn
}

func (client Client) Send(messageType byte, event byte, data []int) ([]byte, int) {
	client.connection = client.Dial()
	defer client.connection.Close()
	packet := CreatePacket(messageType, event, data)
	WriteRaw(client.connection, packet)
	buffer := ReadRaw(client.connection)
	return ReadResponsePacket(buffer, len(buffer))
}
