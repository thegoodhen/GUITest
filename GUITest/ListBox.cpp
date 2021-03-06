
#include "ListBox.h"
#include <functional>


	ListBox::ListBox(String _id, String _text)
	{
		id = _id;
		text = _text;
		elementType = "ListBox";
		this->isSynced = true;
	}


	void ListBox::sendHtml(ESPWebServer& server)
	{
		this->startElementContainer(server);

		server.sendContent("<select ");
		server.sendContent("id=\"");
		server.sendContent(this->id);
		server.sendContent("\" ");
		server.sendContent(this->getCallbackString());
		server.sendContent(">\n");
		sendItemsHtml(server);
		server.sendContent("</select>\n");

		this->endElementContainer(server);
	}

	void ListBox::sendItemsHtml(ESPWebServer& server)
	{
		for (std::vector<ListItem*>::size_type i = 0; i != theList.size(); i++) {
			{
				//Serial.println(i);
				theList[i]->sendHtml(server);
			}
		}

	}


	void ListBox::addItem(ListItem* li)//TODO: add api for removing items
	{
		theList.push_back(li);
	}

	/*update the list of items on client side (i.e. in the browser)*/
	void ListBox::updateItemsDisplay(int clientNo)
	{
		this->evalAndTell(clientNo, "clearListBox(\"" + this->getId() + "\");");//clears the listBox contents

		for (std::vector<ListItem*>::size_type i = 0; i != theList.size(); i++) {
			{
				//Serial.println(i);
				String theValue = "\""+theList[i]->getValue()+"\"";
				String theText= "\""+theList[i]->getText()+"\"";
				String theId = "\"" + this->getId() + "\"";
				this->evalAndTell(clientNo, "addOption("+theId+", "+theValue+", "+theText+");");//clears the listBox contents
			}
		}
	}

	void ListBox::clearList()
	{
		for (std::vector<ListItem*>::size_type i = 0; i != theList.size(); i++) 
			{
				delete(theList[i]);
			}
		theList.clear();

	}


	void ListBox::onChange(std::function<void(int, ListItem)> f)
	{

		String CBString = "sendJSON({type:\"event\", id: this.id, evType:\"onchange\", index: this.selectedIndex, value: this.options[this.selectedIndex].value, text: this.options[this.selectedIndex].text})";
		this->onChangeCB = f;
		this->addClientSideCallback("onchange", CBString);
	}

	int ListBox::handleEvent(int clientNum, JsonObject &obj) 
	{
		if (strcmp(obj["evType"], "onchange") == 0)
		{

			if (onChangeCB!= NULL)
			{
				int index = obj["index"];
				String value = obj["value"];
				String text = obj["text"];
				this->lastRetrievedIndex = index;
				//Serial.println("te�konc jsem tady");
				//Serial.println(value);

				ListItem li = ListItem(text, value, index);

				if (isSynced)
				{
					this->setPropertyOfAllBut(clientNum, "selectedIndex", (String)index);//synchronize the switch between clients
				}
				onChangeCB(clientNum, li);
				return 0;
			}
		}
		return 1;
	}


String ListBox::retrieveText(int clientNo)
{
	this->retrieveIntValue(clientNo);//necessary for updating the last value
	return this->evalAndTell(clientNo, "var theLb=document.getElementById(\"" + id + "\"); theLb.options[theLb.selectedIndex].value");
}

int ListBox::retrieveIntValue(int clientNo)
{
	String s=this->evalAndTell(clientNo, "var theLb=document.getElementById(\"" + id + "\"); theLb.selectedIndex");
	const char* str = s.c_str();
	char* ptr;
	int intVal=strtol(str, &ptr, 10);//yea, but handle some error checking as well please
	this->lastRetrievedIndex = intVal;
	return intVal;
}


	void ListBox::sendInitialization(int clientNo)
	{
		//clearList();
		//this->addItem(new ListItem("kokon gekon"));
		updateItemsDisplay(clientNo);//provide the fresh list of options
		if (lastRetrievedIndex != -1 && isSynced)
		{
			this->setProperty(clientNo, "selectedIndex", (String)lastRetrievedIndex);//synchronize the switch between clients
		}
		else
		{
			this->setProperty(clientNo, "selectedIndex", (String)this->defaultSelectedIndex);
		}
		//getGUI()->sendText(clientNo, "initialized"+(String)this->getId());
	}

	void ListBox::setDefaultIntValue(int _val)
	{
		Serial.println("pustila se ta spravna funkce to vypada!");
		this->defaultSelectedIndex = _val;
		this->defaultIntValue = _val;
	}

	/*
	String Button::getHTML() 
	{
		return "<button id=\"" + id + "\" onclick='sendJSON({type:\"event\", id: this.id, evType:\"click\"})'>" + text + "</button>\n";
	}
	*/
