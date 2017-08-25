#include "User.h"

User::User(SOCKET sock, SOCKET passiveSock, string id, string name, string connectionCodePassive, string connectionCodeActive, bool active_open, bool passive_open)
{
	_sock = sock;
	_passiveSock = passiveSock;
	_ID = id;
	_name = name;
	_connectionCodeForPassive = connectionCodePassive;
	_connectionCodeForActive = connectionCodeActive;
	_active_open = active_open;
	_passive_open = passive_open;
	_passiveSocketLocked = false;
	_mainPassiveSocketLocked = false;
	_mainPassiveSocketKilled = false;
	//helpPicInfo = new help_pic_info;
}

User::~User()
{}


SOCKET User::getSocket()
{
	return _sock;
}

SOCKET User::getPassiveSocket()
{
	return _passiveSock;
}

string User::getID()
{
	return _ID;
}

string User::getName()
{
	return _name;
}

string User::getPassiveConnectionCode()
{
	return _connectionCodeForPassive;
}

string User::getActiveConnectionCode()
{
	return _connectionCodeForActive;
}

string User::getUserBuffer()
{
	return _user_buffer;
}

bool User::getActiveOpen()
{
	return _active_open;
}

bool User::getPassiveOpen()
{
	return _passive_open;
}

void User::setSocket(SOCKET sock)
{
	_sock = sock;
}

void User::setPassiveSocket(SOCKET passiveSock)
{
	_passiveSock = passiveSock;
}

void User::setID(string ID)
{
	_ID = ID;
}

void User::setName(string name)
{
	_name = name;
}

void User::setPassiveConnectionCode(string connectionCode)
{
	_connectionCodeForPassive = connectionCode;
}

void User::setActiveConnectionCode(string connectionCode)
{
	_connectionCodeForActive = connectionCode;
}

void User::setUserBuffer(string buf)
{
	_user_buffer = buf;
}

void User::setActiveOpen(bool val)
{
	_active_open = val;
}

void User::setPassiveOpen(bool val)
{
	_passive_open = val;
}