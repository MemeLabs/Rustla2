vcl 4.0;

backend default {
    .host = "127.0.0.1";
    .port = "9000";
    .probe = {
        .url = "/";
        .timeout = 300ms;
        .interval = 10s;
     }
}

sub vcl_recv {
	# handle static content folders
	if (req.url ~ "^/(assets|js|css|image)/") {
		unset req.http.cookie;
	}

	# cache any other static asset
	if ( req.url ~ "(?i)\.(png|gif|jpeg|jpg|ico|swf|css|js|html|htm)(\?[a-z0-9]+)?$" ) {
		unset req.http.cookie;
	}

	# don't cache if there is a JWT token
	if ( req.http.cookie ~ "jwt" ) {
		return(pass);
	}

	# don't cache POSTs (do we have any)
	if (req.method == "POST") {
		return(pass);
	}

	# don't cache profile and login pages
	if (req.url == "/(profile|login|oauth)") {
		return(pass);
	}
}

sub vcl_backend_response {

    # For static content strip all backend cookies
    if (bereq.url ~ "\.(css|js|png|gif|jpg)") {
        unset beresp.http.cookie;
    }

    # let the content live for 30m in cache
    set beresp.ttl = 30m;

    # serve up old shit if we break things
    set beresp.grace = 24h;
}

sub vcl_deliver {
    # remove unwanted headers
    unset resp.http.Via;
    unset resp.http.X-Varnish;

    # add new headers
    set resp.http.Pepo = "Thinking";
}
