/*
	Copyright 2015 Kirollos

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

var _config = require('./config.js').config;

var http = require((_config.usessl == true) ? 'https' : 'http');
var fs = require('fs');

//var port = process.argv[2]; // Had it for testing purposes, not needed anymore.

var httphandler = function(req, resp) {

	var f,t;

	if(typeof req.headers["authorization"] == 'undefined' || req.headers["authorization"].replace('Basic ', '') != new Buffer(_config.webhost_user+":"+_config.webhost_pass).toString('base64')) {
		resp.setHeader('WWW-Authenticate', 'Basic');
		resp.writeHead(401);
		resp.end("Unauthorized: pls");
		return;
	}

	if(req.url == "/") {
		f = "index.html";
		t = "text/html";
	}
	else if(req.url == "/CONFIG") {
		f = "../config.js";
		t = "application/javascript";
	}
	else {
		f = req.url;
		if(f.match('.html'))
			t = "text/html";
		if(f.match('.css'))
			t = "text/css";
		if(f.match('.js'))
			t = "application/javascript";
	}
	try {
		var fresp = fs.readFileSync("web/"+f);

		resp.writeHead(200, {"Content-type": t});
		resp.end(fresp);
	}
	catch(e) {
		resp.writeHead(404);
		resp.end("File not found.");
	}
};

var httpsvr;
if(_config.usessl) {
	httpsvr = http.createServer({
		key:	fs.readFileSync('key.pem'),
		cert:	fs.readFileSync('cert.pem')
	}, httphandler);
}
else
{
	httpsvr = http.createServer(httphandler);
}

var sock = require('socket.io')(httpsvr);

sock.on('connection', function(socket){
	try{
		var rcon = new require('net').Socket();
		console.log("New connection from " + socket.handshake.address);
		rcon.connect(_config.rcon_port, _config.rcon_ip);
	}
	catch(e) {
		console.log("Error while connecting to server: " + e);
		socket.emit('on-receive', {resp: "Error: Failed to connect to the server: " + e});
		socket.emit('rip');
		if(rcon != null) {
			rcon.destroy();
		}
		rcon = null;
		return;
	}
	rcon.on('data', function(data){
		socket.emit('on-receive', {resp: data.toString('utf8').trim()});
	});
	rcon.on('close', function(data){
		socket.emit('rip');
		if(rcon != null) {
			rcon.destroy();
		}
		rcon = null;
	});
	rcon.on('error', function(err){
		console.log("Error while connecting to server: " + err);
		socket.emit('on-receive', {resp: "Error: Failed to connect to the server: " + err});
	});
	socket.on('on-send', function(data){
		if(rcon == null) return;
		console.log("Client ("+socket.handshake.address+") has executed command '"+data.cmd+"'");
		rcon.write(data.cmd + "\r\n");
	});
	socket.on('disconnect', function(data){
		if(rcon != null) {
			rcon.destroy();
		}
		console.log("Disconnected from " + socket.handshake.address);
		rcon = null;
	});
});


httpsvr.listen(_config.webhost_port);