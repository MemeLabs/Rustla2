package resolvers

import (
	"context"

	"github.com/MemeLabs/Rustla2/graphql/pkg/models"
)

type queryResolver struct{ *Resolver }

func (r *queryResolver) Viewer(ctx context.Context) (*models.User, error) {
	panic("not implemented")
}
func (r *queryResolver) Channels(ctx context.Context) ([]models.Channel, error) {
	panic("not implemented")
}
