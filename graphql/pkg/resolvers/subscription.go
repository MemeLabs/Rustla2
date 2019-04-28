package resolvers

import (
	"context"

	"github.com/MemeLabs/Rustla2/graphql/pkg/models"
)

type subscriptionResolver struct{ *Resolver }

func (r *subscriptionResolver) ChannelChanged(ctx context.Context) (<-chan *models.Channel, error) {
	panic("not implemented")
}
