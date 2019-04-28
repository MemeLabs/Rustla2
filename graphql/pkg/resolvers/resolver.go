package resolvers

import (
	"github.com/MemeLabs/Rustla2/graphql/pkg/models"
)

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
