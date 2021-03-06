

/*
 * WebSocketServer_LEDcontrol.ino
 *
 *  Created on: 26.11.2015
 *
 */

#include <Arduino.h>

#include "TGH_GUI.h"//We include the library



GUI gui;


#ifdef ESP8266
	#include <ESP8266WiFi.h>
	#include <ESP8266WiFiMulti.h>
typedef ESP8266WiFiMulti WiFiMulti ;
#else
	#include <WiFi.h>
	#include <WiFiMulti.h>
#endif
#include "TGH_GUI.h"//We include the library

WiFiMulti wifiMulti;

void setup() {
	//USE_SERIAL.begin(921600);
	USE_SERIAL.begin(115200);//For debug output

	//USE_SERIAL.setDebugOutput(true);

	USE_SERIAL.println();
	USE_SERIAL.println();
	USE_SERIAL.println();

	for (uint8_t t = 3; t > 0; t--) {
		USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);//Taken from the WebSockets library, nice to properly connect to the serial port
		USE_SERIAL.flush();
		delay(1000);
	}

	wifiMulti.addAP("TGH_network", "r0ut3rp@$$");

	while (wifiMulti.run() != WL_CONNECTED) {//We wait till we connect to our router
		delay(100);
	}
	USE_SERIAL.println("WiFi.localIP()");//then we print out the IP of this device; paste it into the browser window to connect to it; alternatively, you could
	//also use an AP mode, where the device has its own SSID and password!
	USE_SERIAL.println(WiFi.localIP());
	gui.begin();//this is a necessary call! You need to call this in setup() to properly initialize the GUI!!




	//TabbedPane* tp = new TabbedPane("tp1");//We first need to create a tabbed pane in order to add tabs!
	//hBox* hb = new hBox("hb");
	//gui.add(hb);//We need to attach it to the GUI
	//Tab* tab1 = new Tab("Overview");//We create the first tab
	//tp->addTab(tab1);//We add the tab to the tabPane

	//Tab* tab2 = new Tab("Tests");
	//tp->addTab(tab2);//We add the tab to the tabPane

	//Tab* tab3 = new Tab("Settings");
	//tp->addTab(tab3);//We add the tab to the tabPane

	//vBox* vb = new vBox("vBox");//we create a new vertical box - the things in this box will go one under another
	//tab2->add(vb);//we add the vertical box inside the horizontal box we created



	//Button* btnRecallSettings= new Button("btnRecallSettings", "Recall stored settings", NULL);
	//vb->add(btnRecallSettings);


	vBox* vb = new vBox("vb");
	gui.add(vb);
	Chart* ch = new Chart("ch","some chart",true,"cas","slepice");
	vb->add(ch);
	ch->setPersistency(true);
	ch->clear();
}


void loop() {
	static unsigned long lastMillis;
	if (millis() - lastMillis > 250)
	{
		lastMillis = millis();
		Chart* ch = (Chart*) gui.find("ch");
		double pts[2];
		pts[0] = (double)1554797625UL + (double)millis() / 1000;
		Serial.println(pts[0], 10);
		pts[1] = random(10);
		ch->addPoint(ALL_CLIENTS, pts,2);
	}
	gui.loop();//you have to call this function in loop() for this library to work!
}

void buttonCB(int user)
{
	
	Serial.println("Saving communicator settings");

	String _smtpServer = gui.find("tiPort")->retrieveText(user);
	String slepice = gui.find("lb1")->retrieveText(user);
	Serial.println("smtpServer:");
	Serial.println(_smtpServer);
	gui.showInfo(user, "KOKOKODAAAAK");
	/*
	USE_SERIAL.println("User clicked the button! User number: ");
	USE_SERIAL.println(user);
	USE_SERIAL.println("First text input:");
	USE_SERIAL.println(gui.find("tiLoadCurrent")->retrieveText(user));
	USE_SERIAL.println(gui.find("cbIncludeResult")->retrieveIntValue(user));
	*/
	/*
	USE_SERIAL.println("Second text input:");
	USE_SERIAL.println(gui.find("ti2")->retrieveText(user));
	USE_SERIAL.println("Checkbox:");
	USE_SERIAL.println(gui.find("cb1")->retrieveIntValue(user));
	USE_SERIAL.println("Slider:");
	USE_SERIAL.println(gui.find("sl1")->retrieveIntValue(user));
	USE_SERIAL.println("ListBox");
	USE_SERIAL.println(gui.find("lb1")->retrieveText(user));
	USE_SERIAL.println(gui.find("lb1")->retrieveIntValue(user));
*/
}
void lbCb(int user, ListItem li)
{
	USE_SERIAL.println(li.getValue());
}
