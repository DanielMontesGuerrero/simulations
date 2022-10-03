package connect

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"net"

	"github.com/DanielMontesGuerrero/simulations/utilsgo"
)

func CreatePacket(messageType byte, event byte, data []byte) []byte {
	packet := []byte{messageType, event}
	packet = appendInt(packet, len(data))
	packet = append(packet, data...)
	return packet
}

func ReadPacket(conn net.Conn) (byte, byte, []byte) {
	buffer := ReadRaw(conn, 6)
	if len(buffer) < 6 {
		fmt.Printf("Expected readLen (%d) >= 6\n", len(buffer))
		return MESSAGE_ERROR, 0, []byte{}
	}
	messageType := buffer[0]
	event := buffer[1]
	dataLen := int(binary.LittleEndian.Uint32(buffer[2:6]))
	buffer = ReadRaw(conn, dataLen)
	if len(buffer) < dataLen {
		fmt.Printf("Expected readLen (%d) >= %d\n", len(buffer), dataLen)
		return MESSAGE_ERROR, 0, []byte{}
	}
	return messageType, event, buffer
}

func BytesToInts(buffer []byte) []int {
	if len(buffer)%4 > 0 {
		aux := make([]byte, 4-(len(buffer)%4))
		buffer = append(buffer, aux...)
	}
	data := make([]int, 0)
	for i := 0; i < len(buffer); i += 4 {
		val := int(binary.LittleEndian.Uint32(buffer[i : i+4]))
		data = append(data, val)
	}
	return data
}

func IntsToBytes(buffer []int) []byte {
	data := make([]byte, 0)
	for i := 0; i < len(buffer); i++ {
		data = appendInt(data, buffer[i])
	}
	return data
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
		fmt.Printf("Expected readLen (%d) >= %d\n", len(buffer), 4)
		return []byte{}, 0
	}
	packetLen := int(binary.LittleEndian.Uint32(buffer))
	buffer = ReadRaw(conn, packetLen)
	if len(buffer) < packetLen {
		fmt.Printf("Expected readLen (%d) >= %d\n", len(buffer), packetLen)
		return []byte{}, 0
	}
	return buffer, packetLen
}

func appendInt(buffer []byte, val int) []byte {
	aux := new(bytes.Buffer)
	err := binary.Write(aux, binary.LittleEndian, int32(val))
	if err != nil {
		fmt.Printf("Error appending int: %s\n", err.Error())
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
	for i := 0; i < MAX_RETRIES; i++ {
		upperLimit := totalSentLen + MAX_PACKET_LEN
		if upperLimit > len(buffer) {
			upperLimit = len(buffer)
		}
		sentLen, err := connection.Write(buffer[totalSentLen:upperLimit])
		if err != nil {
			fmt.Println("Error writing data:", err.Error())
			break
		}
		totalSentLen += sentLen
		if totalSentLen == len(buffer) {
			break
		}
	}
	if len(buffer) != totalSentLen {
		fmt.Printf("Did not send whole packet. Send len: %d\n", totalSentLen)
	}
}

func ReadRaw(connection net.Conn, size int) []byte {
	if size == 0 {
		return []byte{}
	}
	response := make([]byte, 0)
	totalReadLen := 0
	for i := 0; i < MAX_RETRIES; i++ {
		lenToRead := size - totalReadLen
		if lenToRead > MAX_PACKET_LEN {
			lenToRead = MAX_PACKET_LEN
		}
		buffer := make([]byte, lenToRead)
		readLen, err := connection.Read(buffer)
		if err != nil {
			fmt.Printf("Error reading bytes: %s\n", err.Error())
			return []byte{}
		}
		totalReadLen += readLen
		response = append(response, buffer[:readLen]...)
		if totalReadLen == size {
			return response
		}
	}
	fmt.Printf("Did not read whole packet. Read len: %d\n", totalReadLen)
	return response
}

func SerializeMatrix(matrix *utilsgo.Matrix) []int {
	packet := make([]int, 0)
	packet = append(packet, matrix.Rows)
	packet = append(packet, matrix.Cols)
	for i := 0; i < matrix.Rows; i++ {
		for j := 0; j < matrix.Cols; j += 32 {
			val := 0
			for k := 0; k < 32; k++ {
				if matrix.Get(i, j+k) == 1 {
					val |= (1 << k)
				}
			}
			packet = append(packet, val)
		}
	}
	return packet
}

func DeserializeMatrix(packet []byte) *utilsgo.Matrix {
	if len(packet) < 8 {
		return utilsgo.New(0, 0)
	}
	rows := int(binary.LittleEndian.Uint32(packet[:4]))
	cols := int(binary.LittleEndian.Uint32(packet[4:8]))
	matrix := utilsgo.New(rows, cols)
	numOfInts := cols / 32
	if cols%32 > 0 {
		numOfInts++
	}
	packet = packet[8:]
	for i := 0; i < rows; i++ {
		for j := 0; j < cols; j += 32 {
			val := 0
			if len(packet) >= (i*numOfInts*4 + j/32*4 + 4) {
				val = int(binary.LittleEndian.Uint32(packet[i*numOfInts*4+j/32*4 : i*numOfInts*4+j/32*4+4]))
			} else {
				fmt.Printf("Packet smaller than expected, size: %d\n", len(packet))
			}
			for k := 0; k < 32; k++ {
				matrix.Set(i, j+k, ((val>>k)&1) != 0)
			}
		}
	}
	return matrix
}

func SerializeVector(vector *utilsgo.Vector) []byte {
	data := make([]byte, 0)
	data = appendInt(data, vector.Size)
	numOfInts := vector.Size / 32
	if vector.Size%32 > 0 {
		numOfInts++
	}
	for i := 0; i < numOfInts; i++ {
		val := 0
		for j := 0; j < 32; j++ {
			if vector.Get(i*32+j) == 1 {
				val |= (1 << j)
			}
		}
		data = appendInt(data, val)
	}
	return data
}

func DeserializeVector(packet []byte) *utilsgo.Vector {
	if len(packet) < 4 {
		return utilsgo.NewVector(0)
	}
	size := int(binary.LittleEndian.Uint32(packet[:4]))
	vector := utilsgo.NewVector(size)
	packet = packet[4:]
	for i := 0; i < size; i += 32 {
		val := 0
		if len(packet) >= (i/32*4 + 4) {
			val = int(binary.LittleEndian.Uint32(packet[i/32*4 : i/32*4+4]))
		}
		for j := 0; j < 32; j++ {
			vector.Set(i+j, ((val>>j)&1) != 0)
		}
	}
	return vector
}

func SerializeBorders(top, bottom, left, right *utilsgo.Vector) []byte {
	data := SerializeVector(top)
	data = append(data, SerializeVector(bottom)...)
	data = append(data, SerializeVector(left)...)
	data = append(data, SerializeVector(right)...)
	return data
}

func DeserializeBorders(data []byte) (*utilsgo.Vector, *utilsgo.Vector, *utilsgo.Vector, *utilsgo.Vector) {
	vectors := make([]utilsgo.Vector, 4)
	for i := 0; i < 4; i++ {
		vec := DeserializeVector(data)
		numOfInts := vec.Size / 32
		if vec.Size%32 > 0 {
			numOfInts++
		}
		data = data[(numOfInts+1)*4:]
		vectors[i] = *vec
	}
	return &vectors[0], &vectors[1], &vectors[2], &vectors[3]
}
