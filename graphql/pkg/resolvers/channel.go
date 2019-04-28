package resolvers

import (
	"context"

	"github.com/MemeLabs/Rustla2/graphql/pkg/models"
	"github.com/MemeLabs/Rustla2/graphql/pkg/types"
)

type channelResolver struct{ *Resolver }

func (r *channelResolver) ID(ctx context.Context, obj *models.Channel) (types.ID, error) {
	panic("not implemented")
}
func (r *channelResolver) UpdatedAt(ctx context.Context, obj *models.Channel) (*types.Timestamp, error) {
	panic("not implemented")
}
func (r *channelResolver) Service(ctx context.Context, obj *models.Channel) (models.Service, error) {
	panic("not implemented")
}
func (r *channelResolver) Channel(ctx context.Context, obj *models.Channel) (string, error) {
	panic("not implemented")
}
func (r *channelResolver) ViewerCount(ctx context.Context, obj *models.Channel) (int, error) {
	panic("not implemented")
}
func (r *channelResolver) Tags(ctx context.Context, obj *models.Channel) ([]string, error) {
	panic("not implemented")
}
func (r *channelResolver) ServiceInfo(ctx context.Context, obj *models.Channel) (*models.ServiceInfo, error) {
	panic("not implemented")
}
func (r *channelResolver) Rooms(ctx context.Context, obj *models.Channel) ([]models.Room, error) {
	panic("not implemented")
}
