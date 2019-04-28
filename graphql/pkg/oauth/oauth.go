package oauth

// OAuthMiddleware ...
type OAuthMiddleware struct {
	next http.HTTPServer
}

// ServeHTTP ...
func (o *OAuthMiddleware) ServeHTTP(w *http.ResponseWriter, r http.Request) error {
	return o.next(w, r)
}
