package main

import (
	"context"
	"log"
	"net/http"

	"github.com/99designs/gqlgen/graphql"
	"github.com/99designs/gqlgen/handler"
	"github.com/MemeLabs/Rustla2/graphql/api/gql"
	server "github.com/MemeLabs/Rustla2/graphql/pkg/server"
	"github.com/spf13/viper"
)

func main() {
	viper.SetConfigName("config")
	viper.AddConfigPath("/etc/rustla3/")
	viper.AddConfigPath(".")

	viper.SetDefault("port", "8080")

	err := viper.ReadInConfig()
	if err != nil {
		log.Fatal("error reading config\n", err)
	}

	config := gql.Config{Resolvers: &server.Resolver{}}

	config.Directives.HasRole = func(ctx context.Context, obj interface{}, next graphql.Resolver, role gql.Role) (res interface{}, err error) {
		// if !getCurrentUser(ctx).HasRole(role) {
		// 	// block calling the next resolver
		// 	return nil, fmt.Errorf("Access denied")
		// }

		// or let it pass through
		return next(ctx)
	}

	http.Handle("/", handler.Playground("GraphQL playground", "/query"))
	http.Handle("/query", handler.GraphQL(gql.NewExecutableSchema(config)))
	http.HandleFunc("/login", func(w http.ResponseWriter, r *http.Request) {
		http.Redirect(w, r, "https://", 301)
	})

	port := viper.GetString("port")
	log.Printf("connect to http://localhost:%s/ for GraphQL playground", port)
	log.Fatal(http.ListenAndServe(":"+port, nil))
}
