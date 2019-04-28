package resolvers

import (
	"context"

	models1 "github.com/MemeLabs/Rustla2/graphql/pkg/models"
	"github.com/MemeLabs/Rustla2/graphql/pkg/pkg/models"
)

// THIS CODE IS A STARTING POINT ONLY. IT WILL NOT BE UPDATED WITH SCHEMA CHANGES.

type Resolver struct{}

func (r *Resolver) Channel() models.ChannelResolver {
	return &channelResolver{r}
}
func (r *Resolver) Mutation() models.MutationResolver {
	return &mutationResolver{r}
}
func (r *Resolver) Query() models.QueryResolver {
	return &queryResolver{r}
}
func (r *Resolver) Room() models.RoomResolver {
	return &roomResolver{r}
}
func (r *Resolver) Subscription() models.SubscriptionResolver {
	return &subscriptionResolver{r}
}
func (r *Resolver) User() models.UserResolver {
	return &userResolver{r}
}
func (r *Resolver) UserConfig() models.UserConfigResolver {
	return &userConfigResolver{r}
}

type channelResolver struct{ *Resolver }

func (r *channelResolver) ID(ctx context.Context, obj *models1.Channel) (models1.ID, error) {
	panic("not implemented")
}
func (r *channelResolver) UpdatedAt(ctx context.Context, obj *models1.Channel) (*models1.Timestamp, error) {
	panic("not implemented")
}
func (r *channelResolver) Service(ctx context.Context, obj *models1.Channel) (models.Service, error) {
	panic("not implemented")
}
func (r *channelResolver) Channel(ctx context.Context, obj *models1.Channel) (string, error) {
	panic("not implemented")
}
func (r *channelResolver) ViewerCount(ctx context.Context, obj *models1.Channel) (int, error) {
	panic("not implemented")
}
func (r *channelResolver) Tags(ctx context.Context, obj *models1.Channel) ([]string, error) {
	panic("not implemented")
}
func (r *channelResolver) ServiceInfo(ctx context.Context, obj *models1.Channel) (*models.ServiceInfo, error) {
	panic("not implemented")
}
func (r *channelResolver) Rooms(ctx context.Context, obj *models1.Channel) ([]models1.Room, error) {
	panic("not implemented")
}

type mutationResolver struct{ *Resolver }

func (r *mutationResolver) UpdateUserConfig(ctx context.Context, name string, data models1.UserConfigData) (*models1.UserConfigData, error) {
	panic("not implemented")
}
func (r *mutationResolver) GetOrCreateChannel(ctx context.Context, service models.Service, channel string, tags []string) (*models1.Channel, error) {
	panic("not implemented")
}
func (r *mutationResolver) UpdateChannel(ctx context.Context, id models1.ID, tags []string) (*models1.Channel, error) {
	panic("not implemented")
}
func (r *mutationResolver) UpdateRoom(ctx context.Context, id models1.ID, tags []string, path *string, title *string, description *string, thumbnailURL *string) (*models1.Room, error) {
	panic("not implemented")
}

type queryResolver struct{ *Resolver }

func (r *queryResolver) Viewer(ctx context.Context) (*models1.User, error) {
	panic("not implemented")
}
func (r *queryResolver) Channels(ctx context.Context) ([]models1.Channel, error) {
	panic("not implemented")
}

type roomResolver struct{ *Resolver }

func (r *roomResolver) ID(ctx context.Context, obj *models1.Room) (models1.ID, error) {
	panic("not implemented")
}
func (r *roomResolver) Owner(ctx context.Context, obj *models1.Room) (*models1.User, error) {
	panic("not implemented")
}
func (r *roomResolver) Editors(ctx context.Context, obj *models1.Room) ([]models1.User, error) {
	panic("not implemented")
}
func (r *roomResolver) Channel(ctx context.Context, obj *models1.Room) (*models1.Channel, error) {
	panic("not implemented")
}
func (r *roomResolver) ViewerCount(ctx context.Context, obj *models1.Room) (int, error) {
	panic("not implemented")
}
func (r *roomResolver) Tags(ctx context.Context, obj *models1.Room) ([]string, error) {
	panic("not implemented")
}
func (r *roomResolver) Occupants(ctx context.Context, obj *models1.Room, first *int, after *string, last *int, before *string) (*models.RoomOccupantsConnection, error) {
	panic("not implemented")
}
func (r *roomResolver) Title(ctx context.Context, obj *models1.Room) (string, error) {
	panic("not implemented")
}
func (r *roomResolver) Description(ctx context.Context, obj *models1.Room) (string, error) {
	panic("not implemented")
}
func (r *roomResolver) ThumbnailURL(ctx context.Context, obj *models1.Room) (string, error) {
	panic("not implemented")
}

type subscriptionResolver struct{ *Resolver }

func (r *subscriptionResolver) ChannelChanged(ctx context.Context) (<-chan *models1.Channel, error) {
	panic("not implemented")
}

type userResolver struct{ *Resolver }

func (r *userResolver) ID(ctx context.Context, obj *models1.User) (models1.ID, error) {
	panic("not implemented")
}
func (r *userResolver) CreatedAt(ctx context.Context, obj *models1.User) (*models1.Timestamp, error) {
	panic("not implemented")
}
func (r *userResolver) LastSeenAt(ctx context.Context, obj *models1.User) (*models1.Timestamp, error) {
	panic("not implemented")
}
func (r *userResolver) Channels(ctx context.Context, obj *models1.User) ([]models1.Channel, error) {
	panic("not implemented")
}
func (r *userResolver) Room(ctx context.Context, obj *models1.User) (*models1.Room, error) {
	panic("not implemented")
}
func (r *userResolver) RoomsOccupied(ctx context.Context, obj *models1.User, first *int, after *string, last *int, before *string) (*models.UserRoomsOccupiedConnection, error) {
	panic("not implemented")
}

type userConfigResolver struct{ *Resolver }

func (r *userConfigResolver) ID(ctx context.Context, obj *models1.UserConfig) (models1.ID, error) {
	panic("not implemented")
}
func (r *userConfigResolver) Data(ctx context.Context, obj *models1.UserConfig) (*models1.UserConfigData, error) {
	panic("not implemented")
}
