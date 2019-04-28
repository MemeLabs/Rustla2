//go:generate go run github.com/99designs/gqlgen

package models

import (
	"encoding/json"
	"time"

	"github.com/sony/sonyflake"
	"github.com/volatiletech/null"
)

var flake *sonyflake.Sonyflake

func init() {
	flake = sonyflake.NewSonyflake(sonyflake.Settings{
		StartTime: time.Date(2019, 0, 0, 0, 0, 0, 0, time.UTC),
	})
}

// NewID ...
func NewID() null.Uint64 {
	id, err := flake.NextID()
	if err != nil {
		panic(err)
	}
	return null.Uint64From(id)
}

// IDs ...
type IDs []null.Uint64

// Interfaces ...
func (e IDs) Interfaces() []interface{} {
	var translatedIds []interface{}
	for _, id := range e {
		translatedIds = append(translatedIds, id)
	}
	return translatedIds
}

// Timestamp ...
type Timestamp time.Time

// Cursor ...
type Cursor json.RawMessage

// UserConfigData ...
type UserConfigData json.RawMessage
