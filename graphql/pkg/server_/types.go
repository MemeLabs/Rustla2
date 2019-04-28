package server

import "time"

// JWTConfig ...
type JWTConfig struct {
	TTL    time.Duration
	Secure bool
	Secret string
	Name   string
	Domain string
}

// Config ...
type Config struct {
	DBURI string
	JWT   JWTConfig

	Emotes                         []string
	EmoteSimilarityMinLength       int
	EmoteSimilarityPrefixCheckSize int
	EmoteSimilarityMinEditDistance int
	EmoteSimilarityMinlength       int

	TwitchClientID     string
	TwitchClientSecret string
	TwitchRedirectURL  string

	GooglePublicAPIKey string
	FacebookAPIKey     string

	IPAddressHeader string

	LiveCheckInterval        time.Duration
	StreamBroadcastInterval  time.Duration
	RustlerBroadcastInterval time.Duration
}
