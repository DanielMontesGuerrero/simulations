package connect

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"net"
)

func CreatePacket(messageType byte, event byte, data []int) []byte {
	packet := []byte{messageType, event}
	packet = appendInt(packet, len(data))
	for i := 0; i < len(data); i++ {
		packet = appendInt(packet, data[i])
	}
	return packet
}

func ReadPacket(conn net.Conn) (byte, byte, []int) {
	buffer := ReadRaw(conn, 6)
	if len(buffer) < 6 {
		fmt.Printf("Expected readLen (%d) >= 6\n", len(buffer))
		return MESSAGE_ERROR, 0, []int{}
	}
	messageType := buffer[0]
	event := buffer[1]
	dataLen := int(binary.LittleEndian.Uint32(buffer[2:6]))
	buffer = ReadRaw(conn, dataLen*4)
	if len(buffer) < 4*dataLen {
		fmt.Printf("Expected readLen (%d) >= %d", len(buffer), 4*dataLen)
		return MESSAGE_ERROR, 0, []int{}
	}
	data := make([]int, dataLen)
	for i := 0; i < dataLen; i++ {
		data[i] = int(binary.LittleEndian.Uint32(buffer[4*i : 4*i+4]))
	}
	return messageType, event, data
}

func CreateResponsePacket(data []int) []byte {
	packet := []byte{}
	packet = appendInt(packet, 4*len(data))
	for i := 0; i < len(data); i++ {
		packet = appendInt(packet, data[i])
	}
	return packet
}

func ReadResponsePacket(conn net.Conn) ([]byte, int) {
	buffer := ReadRaw(conn, 4)
	if len(buffer) < 4 {
		fmt.Printf("Expected readLen (%d) >= %d", len(buffer), 4)
		return []byte{}, 0
	}
	packetLen := int(binary.LittleEndian.Uint32(buffer))
	buffer = ReadRaw(conn, packetLen)
	if len(buffer) < packetLen {
		fmt.Printf("Expected readLen (%d) >= %d", len(buffer), packetLen)
		return []byte{}, 0
	}
	return buffer, packetLen
}

func appendInt(buffer []byte, val int) []byte {
	aux := new(bytes.Buffer)
	err := binary.Write(aux, binary.LittleEndian, int32(val))
	if err != nil {
		fmt.Printf("Error appending int: %s", err.Error())
		return []byte{}
	}
	buffer = append(buffer, aux.Bytes()[:]...)
	return buffer
}

func WriteRaw(connection net.Conn, buffer []byte) {
	if len(buffer) == 0 {
		return
	}
	totalSentLen := 0
	for {
		sentLen, err := connection.Write(buffer[totalSentLen:])
		if err != nil {
			fmt.Println("Error writing data", err)
		}
		totalSentLen += sentLen
		if totalSentLen == len(buffer) {
			fmt.Printf("Sent %d total bytes\n", len(buffer))
			break
		}
	}
}

func ReadRaw(connection net.Conn, size int) []byte {
	if size == 0 {
		return []byte{}
	}
	response := make([]byte, 0)
	totalReadLen := 0
	for {
		buffer := make([]byte, size-totalReadLen)
		readLen, err := connection.Read(buffer)
		if err != nil {
			fmt.Printf("Error reading bytes: %s\n", err.Error())
			return []byte{}
		}
		totalReadLen += readLen
		response = append(response, buffer...)
		if totalReadLen == size {
			fmt.Printf("Read %d total bytes\n", size)
			return response
		}
	}
}
