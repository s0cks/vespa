package main

import (
	"bytes"
	"encoding/binary"
	"flag"
	"fmt"
	"io"
	"log"
	"net"
	"os"
	"time"

	"go.mongodb.org/mongo-driver/bson"
	"vespa/internal/ipc"
)

const DEFAULT_SOCKET_PATH = "/tmp/ipc_test.sock"

func sendPing(conn net.Conn, digest []byte) {
	payload := struct {
		Kind      int32           `bson:"kind"`
		Timestamp time.Time       `bson:"timestamp"`
		Data      ipc.PingPayload `bson:"data"`
	}{
		Kind:      int32(ipc.Ping),
		Timestamp: time.Now(),
		Data: ipc.PingPayload{
			Digest: digest,
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
}

func sendEvent(conn net.Conn, topic string) (*ipc.EventPayload, error) {
	event := ipc.EventPayload{
		Topic: topic,
	}

	payload := struct {
		Kind      int32            `bson:"kind"`
		Timestamp time.Time        `bson:"timestamp"`
		Data      ipc.EventPayload `bson:"data"`
	}{
		Kind:      int32(ipc.Event),
		Timestamp: time.Now(),
		Data:      event,
	}

	bsonBytes, err := bson.Marshal(payload)
	if err != nil {
		return nil, fmt.Errorf("failed to marshal bson: %v", err)
	}

	_, err = conn.Write(bsonBytes)
	if err != nil {
		return nil, fmt.Errorf("failed to write to socket: %v", err)
	}

	return &event, nil
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

func readMessage(conn net.Conn) (*ipc.Message, error) {
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

	var m ipc.Message
	err = bson.Unmarshal(buffer, &m)
	if err != nil {
		return nil, fmt.Errorf("failed to unmarshal bson document: %v", err)
	}

	return &m, nil
}

func decodePong(conn net.Conn) (*ipc.PongPayload, error) {
	m, err := readMessage(conn)
	if err != nil {
		return nil, fmt.Errorf("failed to read response: %v", err)
	}

	if m.Kind != int32(ipc.Pong) {
		return nil, fmt.Errorf("invalid response message: %d", ipc.MessageKind(m.Kind))
	}

	var payload ipc.PongPayload
	if err := bson.Unmarshal(m.Data, &payload); err != nil {
		return nil, fmt.Errorf("failed to unmarshal pong: %v", err)
	}

	return &payload, nil
}

func ping(conn net.Conn, digest []byte) ([]byte, error) {
	sendPing(conn, []byte(digest))
	pong, err := decodePong(conn)
	if err != nil {
		return nil, err
	}

	if !bytes.Equal(digest, pong.Digest) {
		return nil, fmt.Errorf("pong digest does not equal ping: %s != %s", string(digest), string(pong.Digest))
	}

	return pong.Digest, nil
}

func publish(conn net.Conn, topic string) (*ipc.EventPayload, error) {
	return sendEvent(conn, topic)
}

func main() {
	getSocketPath := flag.String("socket-path", "", "The unix socket path to connect to")

	pingCmd := flag.NewFlagSet("ping", flag.ExitOnError)
	pingDigest := pingCmd.String("digest", "", "The digest for the ping")

	applyCmd := flag.NewFlagSet("apply", flag.ExitOnError)

	publishCmd := flag.NewFlagSet("publish", flag.ExitOnError)
	getPublishTopic := publishCmd.String("topic", "", "The topic to publish to")

	if len(os.Args) < 2 {
		log.Fatalf("expected a command")
		os.Exit(1)
	}

	socketPath := DEFAULT_SOCKET_PATH
	if *getSocketPath != "" {
		socketPath = *getSocketPath
	}

	cmd := os.Args[1]
	cmd_args := os.Args[2:]
	switch cmd {
	case "ping":
		pingCmd.Parse(cmd_args)
	case "apply":
		applyCmd.Parse(cmd_args)
	case "publish":
		publishCmd.Parse(cmd_args)
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
		digest := "Hello World"
		if *pingDigest != "" {
			digest = *pingDigest
		}

		fmt.Printf("ping >> %s\n", string(digest))
		pong, err := ping(conn, []byte(digest))
		if err != nil {
			log.Fatalf("failed to pong server: %v", err)
		}

		fmt.Printf("pong << %s\n", string(pong))

	case "publish":
		topic := *getPublishTopic
		if topic == "" {
			fmt.Printf("cannot publish to an empty topic, use --topic\n")
			os.Exit(1)
		}

		_, err := publish(conn, topic)
		if err != nil {
			log.Fatalf("failed to publish event: %v", err)
		}

		fmt.Printf("published event to %s\n", topic)

	default:
		invalidCommand(cmd)
	}
}
