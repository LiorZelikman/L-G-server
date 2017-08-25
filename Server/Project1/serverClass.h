#pragma once

#include <thread>
#include <exception>
#include <map>
#include <WinSock2.h>
#include <Windows.h>
#include <iostream>
#include <string>
#include <queue>
#include <string.h>
#include <cstdlib>
#include <ctime>
#include <time.h>
#include <stdio.h>
#include "DataBase.h"
#include "User.h"


using namespace std;

class ServerClass
{

public:
	ServerClass();
	~ServerClass();
	void serve();

private:
	//Variables:
	SOCKET connectSocket, clientSocket, passiveSocket;
	struct sockaddr_in clientService, clientServicePassive;
	vector<User*> _connectedUsers;//list of user conncted
	unordered_multimap<string, string> text_messages; //username, info.
	unordered_multimap<string, string> post_links; //username, info.
	unordered_multimap<string, string> profile_links; //username, info
	unordered_multimap<string, string> standby_pics_messages; //username, info.


	DataBase _db;
	bool connectedUsersLocked = false;
	bool email_locked = false;
	bool didSrand = false;
	long long timeRangeList[5];
	//Main functions:
	void bindAndListen();
	void _accept();
	bool connectPassiveSocket(char*); //true = success, false = failure
	void clientHandler(User* user);
	void passiveClientHandler(User* user);
	void connectionHandler(SOCKET& clientSock, string data);
	bool handleSignIn(User* user, string data, bool passive_or_active); //true - active, false - passive
	void handleSignUp(User* user, string data);
	void handleForgotPassword(User* user, string data);
	void handleCreateNewPost(User* user, string data);
	void handleAddNewCommentToPost(User* user, string data);
	void handlePostRequest(User* user, string data);
	void handleUpdateComments(User* user, string data);
	void handleRefreshRequest(User* user, string data);
	void handleVote(User* user, string data);
	void handleSavedPosts(User* user, string data);
	void handleDeleteAllSavedPosts(User* user, string data);
	void handleDeleteSavedPosts(User* user, string data);
	void handleAddReadLaterPost(User* user, string data);
	void handlePostSearch(User* user, string data);
	void handleStatusUpdate(User* user, string data);
	void handlePersonalInfoUpdate(User* user, string data);
	void handleProfilePicUpdate(User* user, string data);
	void handleProfileReq(User* user, string data);
	void handleProfilePicReq(User* user, string data);
	void handleUserDataUpdateReq(User* user, string data);
	void handleBackgroundUpdateReq(User* user, string data);
	void handleBlockReq(User* user, string data);
	void handleUserSearch(User* user, string data);

	//Sending functions:
	//---------------------------------------------------------------------------
	void handleGotMessageFromSender(User* user, string data);
	void sendText(User* user, string data);
	void handleSendPostLink(User* user, string data);
	void sendPost(User* user, string data);
	void handleSendProfileLink(User* user, string data);
	void sendProfile(User* user, string data);
	void handleSendPic(User* user, string data);
	void sendPic(User* user, string data);
	//---------------------------------------------------------------------------

	void handleLogOut(User* user);
	void handleExitApp(User* user);
	void handleNotInProtocol(SOCKET& clientSock, string data);

	void checkForMessages(SOCKET clientSock, string username, string userID);
	bool sendStandbyTextMessages(SOCKET& passiveClientSock, string userID, string data); //returns "true" in case of success, "false" in case of failure.
	bool sendStandbyPostLinks(SOCKET& passiveClientSock, string userID, string data); //returns "true" in case of success, "false" in case of failure.
	bool sendStandbyProfileLinks(SOCKET& passiveClientSock, string userID, string data); //returns "true" in case of success, "false" in case of failure.
	bool sendStandbyPics(SOCKET& passiveClientSock, string userID, string data); //returns "true" in case of success, "false" in case of failure.
	//Help functions:
	bool Send(SOCKET& sock, const char data[], int size);
	bool bigDataRecv(SOCKET& sock, char msg_c[], int size);
	string rndStr();
	void sendEmail(string clientEmail, string new_password);
	bool isUserConnectedBySock(SOCKET& sock);
	bool isUserConnectedByID(string ID);
	bool isUserConnectedByName(string name);
	void addMessageLength(char dest[], const char data[], int size);
	void clientDisconnection(User* user, int option);
	string padd_zeroes(string str, int final_size)
	{
		string help = str;
		str = "";
		while ((signed)(str.length() + help.length()) < final_size)
			str += "0";
		str += help;
		return str;
	}
	long long time_now()
	{
		chrono::milliseconds ms = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());
		long long time_now_ms = ms.count();
		return time_now_ms;
	}
	void emptyString(char str[], int startIndex, int endIndex)
	{
		int i;
		for (i = startIndex; i <= endIndex; i++)
		{
			str[i] = NULL;
		}
	}
	string to_lower_case(string str)
	{
		int i, size = str.length();
		for (i = 0; i < size; i++)
		{
			if (str[i] >= 65 && str[i] <= 90)
				str[i] += 32;
		}
		return str;
	}

};