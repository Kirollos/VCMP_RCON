var RCON_Config = 
{
	webhost_ip:		"127.0.0.1",	// The IP of the server hosting the node.js script (Don't set it to localhost if you are hosting it on a public server)
	webhost_port:	1338,			// The Port the webserver will be running on
	webhost_user:	"user",			// The username required to access the terminal
	webhost_pass:	"pass",			// The password required to access the terminal
	rcon_ip:		"127.0.0.1",	// The IP of the RCON server
	rcon_port:		1337,			// The port of the RCON server
	usessl:			false			// SSL or nope? (Requires both key.pem and cert.pem in the same directory)
};


// Don't touch..
if(typeof module != 'undefined')
{
	module.exports.config = RCON_Config;
}