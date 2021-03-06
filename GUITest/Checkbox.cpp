
#include "Checkbox.h"
#include <functional>

	Checkbox::Checkbox(String _id, String _text, std::function<void(int, boolean)> clickCallback)
	{

		id = _id;
		text = _text;
		onClickCB = clickCallback;
		elementType = "checkbox";
		isSynced = true;

		if(clickCallback!=NULL)
		{
		String CBString = "sendJSON({type:\"event\", id: this.id, evType:\"onClick\", value: this.checked})";
		addClientSideCallback("onclick", CBString);
		}
		//callbacksString = "onclick='sendJSON({type:\"event\", id: this.id, evType:\"click\"})';";
	}

	int Checkbox::handleEvent(int clientNum, JsonObject &obj) 
	{
		if (strcmp(obj["evType"], "onClick") == 0)
		{

			if (onClickCB != NULL)
			{
				if (isSynced)
				{
					String v = obj["value"];
					tghDbg("checked:");
					tghDbg(v);
					this->setPropertyOfAllBut(clientNum, "checked", v);//synchronize the switch between clients
				}
				boolean checked = (strcmp(obj["value"], "true") == 0);
				onClickCB(clientNum, checked);
				return 0;
			}
		}
		return 1;
	}


	String Checkbox::getHTML() 
	{
		char tempStr[400];//But of course, what if the id is too long? Well, then we are f***ed!
		const char* lineBreakStr = this->lineBreak ? "<br>" : "";
		sprintf(tempStr, "%s<input type=\"checkbox\", id=\"%s\", %s>%s",this->text.c_str(), id.c_str(), this->getCallbackString().c_str(),lineBreakStr);
		return tempStr;
	}

	void Checkbox::sendHtml(ESPWebServer& server)
	{
		this->startElementContainer(server);

		server.sendContent("<div class=\"switch\">\n");
		server.sendContent("<label>\n");
		server.sendContent("<input type=\"checkbox\", id=\"");
		server.sendContent(this->id);
		server.sendContent("\", ");
		server.sendContent(this->getCallbackString());
		server.sendContent(">\n");
		server.sendContent("<span class=\"lever\"></span>");
		server.sendContent("</label>\n");
		server.sendContent("</div>\n");

		this->endElementContainer(server);
	}

	void Checkbox::onClick(std::function<void(int, boolean)> f)
	{

		String CBString = "sendJSON({type:\"event\", id: this.id, evType:\"onClick\", value: this.checked})";
		this->onClickCB = f;
		this->addClientSideCallback("onclick", CBString);
	}


	void Checkbox::sendInitialization(int clientNo)
	{
		if (this->lastRetrievedIntValue == -1)
		{
		   this->setProperty(clientNo, "checked", this->defaultIntValue?"true":"false");
		}
		else
		{
			this->setProperty(clientNo, "checked", this->lastRetrievedIntValue ? "true" : "false");
		}
		Serial.println("lastRetrievedIntValue");
		Serial.println(lastRetrievedIntValue);
		//getGUI()->sendText(clientNo, "initialized"+(String)this->getId());
	}



int Checkbox::retrieveIntValue(int clientNo)
{
	String s = this->retrieveProperty(clientNo, "checked");
	const char* str = s.c_str();
	int returnVal = false;
	if (strcmp(str, "true") == 0)
	{
		returnVal = true;
	}
	else
	{
		returnVal = false;
	}
	if (isSynced)
	{
		lastRetrievedIntValue = returnVal;
	}
	return returnVal;
}

	/*
	String Button::getHTML() 
	{
		return "<button id=\"" + id + "\" onclick='sendJSON({type:\"event\", id: this.id, evType:\"click\"})'>" + text + "</button>\n";
	}
*/
