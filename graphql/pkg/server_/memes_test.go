package server

import (
	// "reflect"

	"context"
	"database/sql"
	"log"
	"testing"
	"time"

	// "github.com/MemeLabs/Rustla2/graphql/api/gql"

	"github.com/MemeLabs/Rustla2/graphql/pkg/models"
	"github.com/MemeLabs/Rustla2/graphql/pkg/snowflake"
	"github.com/go-redis/redis"
	_ "github.com/go-sql-driver/mysql"
	"github.com/sony/sonyflake"
	"github.com/volatiletech/null"
	"github.com/volatiletech/sqlboiler/boil"
)

func TestMemes(t *testing.T) {
	client := redis.NewClient(&redis.Options{
		Addr:     "localhost:6379",
		Password: "secretpassword", // no password set
		DB:       0,                // use default DB
	})
	// if err != nil {
	// 	t.Error("error creating redis client", err)
	// 	return
	// }

	if err := client.Set("key", "value", 0).Err(); err != nil {
		t.Error("error setting key", err)
	}

	key, err := client.Get("key").Result()
	log.Println(key, err)
}

func TestMemes2(t *testing.T) {
	ctx := context.Background()

	db, err := sql.Open("mysql", "root:secretpassword@/rustla3")
	if err != nil {
		t.Error("error connecting to db", err)
	}

	channel := models.Channel{
		ID:      snowflake.NextID(),
		Service: null.NewString("ANGELTHUMP", true),
		Channel: null.NewString("slugalisk", true),
	}

	err = channel.Insert(ctx, db, boil.Columns{})
	if err != nil {
		t.Error("error", err)
		return
	}

	room := models.Room{
		Path:         "slugalisk",
		OwnerUserID:  null.NewUint64(0, false),
		ChannelID:    null.NewUint64(channel.ID, true),
		Title:        null.NewString("", true),
		Description:  null.NewString("", true),
		ThumbnailURL: null.NewString("", true),
	}

	// room.Path = "slugalisk"
	// room.Title = null.NewString("test room", true)
	// room.Description = null.NewString("test room description...", true)

	err = room.Insert(ctx, db, boil.Columns{})
	if err != nil {
		t.Error("error upserting record", err)
		return
	}

	_ = db
	_ = ctx
	_ = room

	// models.
}

func TestMemes3(t *testing.T) {
	flake := sonyflake.NewSonyflake(sonyflake.Settings{
		StartTime: time.Date(2019, 03, 25, 0, 0, 0, 0, time.UTC),
	})

	id, _ := flake.NextID()
	log.Println(id)

	_ = flake
}

// func TestMemes(t *testing.T) {
// 	tests := []struct {
// 		name string
// 		want []gql.Channel
// 	}{
// 		// TODO: Add test cases.
// 	}
// 	for _, tt := range tests {
// 		t.Run(tt.name, func(t *testing.T) {
// 			if got := GetChannels(); !reflect.DeepEqual(got, tt.want) {
// 				t.Errorf("GetChannels() = %v, want %v", got, tt.want)
// 			}
// 		})
// 	}
// }
