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

func ReadPacket(buffer []byte, readLen int) (byte, byte, []int) {
	if readLen < 6 {
		fmt.Printf("Expected readLen (%d) >= 6\n", readLen)
		return MESSAGE_ERROR, 0, []int{}
	}
	messageType := buffer[0]
	event := buffer[1]
	dataLen := int(binary.LittleEndian.Uint32(buffer[2:6]))
	if readLen < 6+dataLen {
		fmt.Printf("Expected readLen (%d) >= %d", readLen, 6+dataLen)
		return MESSAGE_ERROR, 0, []int{}
	}
	data := make([]int, dataLen)
	for i := 0; i < dataLen; i++ {
		data[i] = int(binary.LittleEndian.Uint32(buffer[6+4*i : 6+4*i+4]))
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

func ReadResponsePacket(buffer []byte, readLen int) ([]byte, int) {
	packetLen := binary.LittleEndian.Uint32(buffer[:4])
	return buffer[4 : packetLen+4], int(packetLen)
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
	totalSentLen := 0
	for {
		sentLen, err := connection.Write(buffer[totalSentLen:])
		if err != nil {
			fmt.Println("Error writing data", err)
		}
		fmt.Printf("Sent %d bytes\n", sentLen)
		totalSentLen += sentLen
		if totalSentLen == len(buffer) {
			break
		}
	}
}

func ReadRaw(connection net.Conn) []byte {
	buffer := make([]byte, MAX_PACKET_LEN)
	readLen, err := connection.Read(buffer)
	if err != nil {
		fmt.Printf("Error reading packet: %s\n", err.Error())
		return []byte{}
	}
	return buffer[:readLen]
}
