package resolvers

import (
	"context"

	"github.com/MemeLabs/Rustla2/graphql/pkg/models"
	"github.com/MemeLabs/Rustla2/graphql/pkg/types"
)

type mutationResolver struct{ *Resolver }

func (r *mutationResolver) UpdateUserConfig(ctx context.Context, name string, data types.UserConfigData) (*types.UserConfigData, error) {
	panic("not implemented")
}
func (r *mutationResolver) GetOrCreateChannel(ctx context.Context, service models.Service, channel string, tags []string) (*models.Channel, error) {
	panic("not implemented")
}
func (r *mutationResolver) UpdateChannel(ctx context.Context, id types.ID, tags []string) (*models.Channel, error) {
	panic("not implemented")
}
func (r *mutationResolver) UpdateRoom(ctx context.Context, id types.ID, tags []string, path *string, title *string, description *string, thumbnailURL *string) (*models.Room, error) {
	panic("not implemented")
}
