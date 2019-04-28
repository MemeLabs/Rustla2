package loaders

//go:generate dataloaden -keys null.Uint64 github.com/MemeLabs/Rustla2/graphql/pkg/models.Channel

import (
	"context"
	"log"
	"time"

	"github.com/MemeLabs/Rustla2/graphql/pkg/models"
	"github.com/volatiletech/null"
	"github.com/volatiletech/sqlboiler/queries/qm"
)

type channelLoaderKeyType struct{}

// ChannelLoaderKey ...
var ChannelLoaderKey channelLoaderKeyType

// NewChannelLoaderWithConfig returns a new channel loader.
func NewChannelLoaderWithConfig(ctx context.Context, cfg config) *ChannelLoader {
	fetch := func(ids []null.Uint64) ([]*models.Channel, []error) {
		log.Printf("Fetching channel ids %v", ids)
		channels, err := models.Channels(
			qm.WhereIn("id in ?", models.IDs(ids).Interfaces()...),
		).All(ctx, cfg.exec)

		if err != nil {
			return nil, []error{err}
		}

		lookup := make(map[null.Uint64]*models.Channel, len(ids))
		for _, channel := range channels {
			lookup[channel.ID] = channel
		}

		sorted := make([]*models.Channel, len(channels))
		for i, id := range ids {

			sorted = append(sorted, lookup[id])
		}

		return sorted, nil
	}

	return &ChannelLoader{
		maxBatch: 100,
		wait:     2 * time.Millisecond,
		fetch:    fetch,
	}
}
