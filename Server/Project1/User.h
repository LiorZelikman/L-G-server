#pragma once

#include <iostream>
#include <string>
#include <WinSock2.h>
#include <Windows.h>

using namespace std;

class User
{
private:
	SOCKET _sock, _passiveSock;
	string _ID;
	string _name;
	string _connectionCodeForPassive;
	string _connectionCodeForActive;
	string _user_buffer;
	bool _active_open;
	bool _passive_open;
public:
	User(SOCKET sock, SOCKET passiveSock, string id, string name, string connectionCodePassive, string connectionCodeActive, bool active_open, bool passive_open);
	~User();
	SOCKET getSocket();
	SOCKET getPassiveSocket();
	string getID();
	string getName();
	string getPassiveConnectionCode();
	string getActiveConnectionCode();
	string getUserBuffer();
	fstream* getImage();
	bool getActiveOpen();
	bool getPassiveOpen();
	void setSocket(SOCKET sock);
	void setPassiveSocket(SOCKET passiveSock);
	void setID(string ID);
	void setName(string name);
	void setPassiveConnectionCode(string connectionCode);
	void setActiveConnectionCode(string connectionCode);
	void setActiveOpen(bool val);
	void setPassiveOpen(bool val);
	void setUserBuffer(string buf);
	bool _passiveSocketLocked;
	bool _mainPassiveSocketLocked;
	bool _mainPassiveSocketKilled;

};