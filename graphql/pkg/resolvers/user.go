package resolvers

import (
	"context"

	"github.com/MemeLabs/Rustla2/graphql/pkg/models"
	"github.com/MemeLabs/Rustla2/graphql/pkg/types"
)

type userResolver struct{ *Resolver }

func (r *userResolver) ID(ctx context.Context, obj *models.User) (types.ID, error) {
	panic("not implemented")
}
func (r *userResolver) CreatedAt(ctx context.Context, obj *models.User) (*types.Timestamp, error) {
	panic("not implemented")
}
func (r *userResolver) LastSeenAt(ctx context.Context, obj *models.User) (*types.Timestamp, error) {
	panic("not implemented")
}
func (r *userResolver) Channels(ctx context.Context, obj *models.User) ([]models.Channel, error) {
	panic("not implemented")
}
func (r *userResolver) Room(ctx context.Context, obj *models.User) (*models.Room, error) {
	panic("not implemented")
}
func (r *userResolver) RoomsOccupied(ctx context.Context, obj *models.User, first *int, after *string, last *int, before *string) (*models.UserRoomsOccupiedConnection, error) {
	panic("not implemented")
}
