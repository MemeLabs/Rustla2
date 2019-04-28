package resolvers

import (
	"context"

	"github.com/MemeLabs/Rustla2/graphql/pkg/models"
	"github.com/MemeLabs/Rustla2/graphql/pkg/types"
)

type userConfigResolver struct{ *Resolver }

func (r *userConfigResolver) ID(ctx context.Context, obj *models.UserConfig) (types.ID, error) {
	panic("not implemented")
}
func (r *userConfigResolver) Data(ctx context.Context, obj *models.UserConfig) (*types.UserConfigData, error) {
	panic("not implemented")
}
