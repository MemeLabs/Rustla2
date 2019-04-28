package loaders

import (
	"context"

	"github.com/volatiletech/sqlboiler/boil"
)

type config struct {
	exec boil.ContextExecutor
}

// With ...
func With(ctx context.Context, exec boil.ContextExecutor) context.Context {
	cfg := config{exec}
	ctx = context.WithValue(ctx, ChannelLoaderKey, NewChannelLoaderWithConfig(ctx, cfg))
	return ctx
}
