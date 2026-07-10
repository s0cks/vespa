package ipc

import (
	"go.mongodb.org/mongo-driver/bson"
	"time"
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
