


#include "GUI.h"

ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

	GUI::GUI()
	{

	}
	void GUI::begin()
	{
		// start webSocket server
		webSocket.begin();
		//void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length);
		webSocket.onEvent(webSocketEvent);

		if (MDNS.begin("esp8266")) {
			USE_SERIAL.println("MDNS responder started");
		}

		// handle index
		//void sendHTML(void);

		/*
    server.on("/", []() {
        // send index.html
        server.send(200, "text/html", "<html><head><script>var connection = new WebSocket('ws://'+location.hostname+':81/', ['arduino']);connection.onopen = function () {  connection.send('Connect ' + new Date()); }; connection.onerror = function (error) {    console.log('WebSocket Error ', error);};connection.onmessage = function (e) {  console.log('Server: ', e.data);};function sendRGB() {  var r = parseInt(document.getElementById('r').value).toString(16);  var g = parseInt(document.getElementById('g').value).toString(16);  var b = parseInt(document.getElementById('b').value).toString(16);  if(r.length < 2) { r = '0' + r; }   if(g.length < 2) { g = '0' + g; }   if(b.length < 2) { b = '0' + b; }   var rgb = '#'+r+g+b;    console.log('RGB: ' + rgb); connection.send(rgb); }</script></head><body>LED Control:<br/><br/>R: <input id=\"r\" type=\"range\" min=\"0\" max=\"255\" step=\"1\" oninput=\"sendRGB();\" /><br/>G: <input id=\"g\" type=\"range\" min=\"0\" max=\"255\" step=\"1\" oninput=\"sendRGB();\" /><br/>B: <input id=\"b\" type=\"range\" min=\"0\" max=\"255\" step=\"1\" oninput=\"sendRGB();\" /><br/></body></html>");
    });
	*/
		//server.on("/", [](GUI* const thisPtr)->void {thisPtr->sendHTML();}(this));
		//void getHTML(void);
    //auto fcn = std::mem_fn(GUI::getHTML);
	std::function<void(void)> f = std::bind(&GUI::sendHTML, this);
	server.on("/", f);
	//this->getHTML();

		server.begin();

		// Add service to MDNS
		MDNS.addService("http", "tcp", 80);
		MDNS.addService("ws", "tcp", 81);

	}
	void GUI::loop()
	{
		webSocket.loop();
		server.handleClient();
	}


	void GUI::webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

		switch (type) {
		case WStype_DISCONNECTED:
			USE_SERIAL.printf("[%u] Disconnected!\n", num);
			break;
		case WStype_CONNECTED: {
			IPAddress ip = webSocket.remoteIP(num);
			USE_SERIAL.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

			// send message to client
			webSocket.sendTXT(num, "Connected");
		}
							   break;
		case WStype_TEXT:
			USE_SERIAL.printf("[%u] get Text: %s\n", num, payload);
			break;
		}

	}

	/**
	Send the text out using Websockets
	*/
	int GUI::sendText(String theText)
	{
		webSocket.sendTXT(0,theText);//TODO: make sure this also works for other user than just the 0th.
		//server.send(200, "text/html", "<html><head><script>var connection = new WebSocket('ws://'+location.hostname+':81/', ['arduino']);connection.onopen = function () {  connection.send('Connect ' + new Date()); }; connection.onerror = function (error) {    console.log('WebSocket Error ', error);};connection.onmessage = function (e) {  console.log('Server: ', e.data);};function sendRGB() {  var r = parseInt(document.getElementById('r').value).toString(16);  var g = parseInt(document.getElementById('g').value).toString(16);  var b = parseInt(document.getElementById('b').value).toString(16);  if(r.length < 2) { r = '0' + r; }   if(g.length < 2) { g = '0' + g; }   if(b.length < 2) { b = '0' + b; }   var rgb = '#'+r+g+b;    console.log('RGB: ' + rgb); connection.send(rgb); }</script></head><body>LED Control:<br/><br/>R: <input id=\"r\" type=\"range\" min=\"0\" max=\"255\" step=\"1\" oninput=\"sendRGB();\" /><br/>G: <input id=\"g\" type=\"range\" min=\"0\" max=\"255\" step=\"1\" oninput=\"sendRGB();\" /><br/>B: <input id=\"b\" type=\"range\" min=\"0\" max=\"255\" step=\"1\" oninput=\"sendRGB();\" /><br/></body></html>");
		//server.send(200, "text/html", theText);
		return 0;
	}
	String GUI::getHTML()
	{
		String returnString;
		returnString += getHeader();
		for (std::vector<int>::size_type i = 0; i != elements.size(); i++) {
			{
				returnString += elements[i].getHTML();
			}
		}
		returnString += getFooter();
		return returnString;
	}

	void GUI::sendHTML()
	{
		server.send(200,"text/html",getHTML());

	}
	String GUI::getHeader()
	{
		return "<!DOCTYPE html>\n\
<html>\n\
\n";
	}
	String GUI::getFooter()
	{
		return "</html> \n";
	}

	String GUI::getScript()
	{
		return " <head>\n\
<script>\n\
var theSocket = new WebSocket(\"ws://192.168.0.102:81\", [\'arduino\']);\n\
\n\
function sendJSON(theJSON)\n\
{\n\
  theSocket.send(JSON.stringify(theJSON));\n\
}\n\
\n\
theSocket.onmessage = function (event) {\n\
  console.log(event.data);\n\
}\n\
</script>\n\
</head>";
	}

	void GUI::add(GUIElement ge)
	{
		elements.push_back(ge);
		ge.setGUI(this);
	}
	GUIElement* GUI::find(String s)
	{
		for (std::vector<int>::size_type i = 0; i != elements.size(); i++) {
			if (elements[i].getId() == s)
			{
				return &elements[i];
			}
		}
		return NULL;
	}

	int GUI::handleRequest(JsonObject obj)
	{
		if (strcmp(obj["type"], "event"))
		{
			String id = obj["evType"];
			GUIElement* ge = find(id);
			if (ge != NULL)
			{
				return ge->handleEvent(obj);
			}
			else
			{
				return 1;
			}
		}
		return 0;
	}
