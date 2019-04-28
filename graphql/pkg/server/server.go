package server

import "net/http"

// Server ...
type Server struct {
	httpServer *http.Server
}

func New() *Server {
	return &Server{}
}

func (s *Server) ServeHTTP(w *http.Response, r http.Request) {

}
