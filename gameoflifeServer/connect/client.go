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
	conn, err := net.DialTimeout(client.protocol, fmt.Sprintf("%s:%d", client.host, client.port), MAX_TIMEOUT)
	if err == nil {
		client.connection = conn
		return conn
	}
	return nil
}

func (client *Client) Send(messageType byte, event byte, data []byte) ([]byte, int) {
	connection := client.Dial()
	if connection == nil {
		fmt.Println("error sending message")
		return []byte{}, 0
	}
	defer connection.Close()
	packet := CreatePacket(messageType, event, data)
	WriteRaw(connection, packet)
	response, resLen := ReadResponsePacket(connection)
	return response, resLen
}

func (client *Client) Close() {
	client.connection.Close()
}
