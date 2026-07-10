package main

import (
	"encoding/binary"
	"flag"
	"fmt"
	"io"
	"log"
	"net"
	"os"
	"time"

	"go.mongodb.org/mongo-driver/bson"
)

type Payload interface {
	isMessagePayload()
}

type PingPayload struct {
	Digest []byte `bson:"digest"`
}

func (PingPayload) isMessagePayload() {}

type PongPayload struct {
	Digest []byte `bson:"digest"`
}

func (PongPayload) isMessagePayload() {}

type EventPayload struct {
	Topic string `bson:"topic"`
}

func (EventPayload) isMessagePayload() {}

type Message struct {
	Kind      int32     `bson:"kind"`
	Timestamp time.Time `bson:"timestamp"`
	Data      bson.Raw  `bson:"data"`
}

type MessageKind int

const (
	Invaild MessageKind = iota
	Event
	Error
	Ping
	Pong
)

const DEFAULT_SOCKET_PATH = "/tmp/ipc_test.sock"

func sendPing(conn net.Conn) {
	payload := struct {
		Kind      int32       `bson:"kind"`
		Timestamp time.Time   `bson:"timestamp"`
		Data      PingPayload `bson:"data"`
	}{
		Kind:      int32(Ping),
		Timestamp: time.Now(),
		Data: PingPayload{
			Digest: []byte("Hello World"),
		},
	}

	bsonBytes, err := bson.Marshal(payload)
	if err != nil {
		log.Fatalf("Failed to marshal bson: %v", err)
	}

	_, err = conn.Write(bsonBytes)
	if err != nil {
		log.Fatalf("Failed to write to socket: %v", err)
	}

	fmt.Printf("Successfully sent %d bytes of bson data\n", len(bsonBytes))
}

var ValidCommands = []string{
	"ping",
}

func printHelp() {

}

func invalidCommand(cmd string) {
	fmt.Printf("invalid command: %s\n", cmd)
	fmt.Printf("valid commands are:\n")
	for _, item := range ValidCommands {
		fmt.Printf(" - %s\n", item)
	}
	os.Exit(1)
}

func readMessage(conn net.Conn) (*Message, error) {
	var lengthBytes [4]byte
	_, err := io.ReadFull(conn, lengthBytes[:])
	if err != nil {
		return nil, fmt.Errorf("invalid bson document: %v", err)
	}

	length := binary.LittleEndian.Uint32(lengthBytes[:])
	if length < 5 {
		return nil, fmt.Errorf("invalid bson document size: %d", length)
	}

	buffer := make([]byte, length)
	copy(buffer[0:4], lengthBytes[:])

	_, err = io.ReadFull(conn, buffer[4:])
	if err != nil {
		return nil, fmt.Errorf("failed to read full bson document: %v", err)
	}

	var m Message
	err = bson.Unmarshal(buffer, &m)
	if err != nil {
		return nil, fmt.Errorf("failed to unmarshal bson document: %v", err)
	}

	return &m, nil
}

func main() {
	pingCmd := flag.NewFlagSet("ping", flag.ExitOnError)

	getPingSocketPath := pingCmd.String("socket-path", "", "The unix socket path to connect to")

	if len(os.Args) < 2 {
		log.Fatalf("expected a command")
		os.Exit(1)
	}

	socketPath := DEFAULT_SOCKET_PATH
	cmd := os.Args[1]
	cmd_args := os.Args[2:]
	switch cmd {
	case "ping":
		pingCmd.Parse(cmd_args)
		if *getPingSocketPath != "" {
			socketPath = *getPingSocketPath
		}
	default:
		invalidCommand(cmd)
	}

	conn, err := net.Dial("unix", socketPath)
	if err != nil {
		log.Fatalf("failed to connect to hive at %s: %v", socketPath, err)
	}
	defer conn.Close()
	conn.SetDeadline(time.Now().Add(5 * time.Second))
	fmt.Printf("connected to hive at: %s\n", socketPath)

	switch cmd {
	case "ping":
		sendPing(conn)
		m, err := readMessage(conn)
		if err != nil {
			log.Fatalf("failed to read response: %v", err)
		}

		switch MessageKind(m.Kind) {
		case Pong:
			var payload PongPayload
			if err := bson.Unmarshal(m.Data, &payload); err != nil {
				log.Fatalf("failed to unmarhsal pong payload: %v", err)
			}
			fmt.Printf("pong: %s\n", string(payload.Digest))
		}

	default:
		invalidCommand(cmd)
	}
}
