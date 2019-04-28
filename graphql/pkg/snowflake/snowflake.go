package snowflake

import (
	"time"

	"github.com/sony/sonyflake"
)

var flake *sonyflake.Sonyflake

func init() {
	flake = sonyflake.NewSonyflake(sonyflake.Settings{
		StartTime: time.Date(2019, 0, 0, 0, 0, 0, 0, time.UTC),
	})
}

// NextID returns unique id
func NextID() uint64 {
	id, err := flake.NextID()
	if err != nil {
		panic(err)
	}
	return id
}
