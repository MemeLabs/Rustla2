package resolvers

import (
	"context"

	"github.com/MemeLabs/Rustla2/graphql/pkg/models"
	"github.com/MemeLabs/Rustla2/graphql/pkg/types"
)

type roomResolver struct{ *Resolver }

func (r *roomResolver) ID(ctx context.Context, obj *models.Room) (types.ID, error) {
	panic("not implemented")
}
func (r *roomResolver) Owner(ctx context.Context, obj *models.Room) (*models.User, error) {
	panic("not implemented")
}
func (r *roomResolver) Editors(ctx context.Context, obj *models.Room) ([]models.User, error) {
	panic("not implemented")
}
func (r *roomResolver) Channel(ctx context.Context, obj *models.Room) (*models.Channel, error) {
	panic("not implemented")
}
func (r *roomResolver) ViewerCount(ctx context.Context, obj *models.Room) (int, error) {
	panic("not implemented")
}
func (r *roomResolver) Tags(ctx context.Context, obj *models.Room) ([]string, error) {
	panic("not implemented")
}
func (r *roomResolver) Occupants(ctx context.Context, obj *models.Room, first *int, after *string, last *int, before *string) (*models.RoomOccupantsConnection, error) {
	panic("not implemented")
}
func (r *roomResolver) Title(ctx context.Context, obj *models.Room) (string, error) {
	panic("not implemented")
}
func (r *roomResolver) Description(ctx context.Context, obj *models.Room) (string, error) {
	panic("not implemented")
}
func (r *roomResolver) ThumbnailURL(ctx context.Context, obj *models.Room) (string, error) {
	panic("not implemented")
}
