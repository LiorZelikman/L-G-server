
#include "serverClass.h"

#define BUFF_SIZE 25600
#define MTU 1460
#define IP "192.168.1.26"
#define PORT 8888
#define EMAIL_PROGRAM_PATH "C:\\Users\\User\\Desktop\\magshimim\\12th_grade\\Project_DG_LZ\\Project\\Server\\Send Mail\\bin\\Debug\\ConsoleApplication1.exe"
#define ALL_OUT_CLOSURE 0
#define JUST_ACTIVE 1
#define JUST_PASSIVE 2

//Protocol:
#define CONNECTION 19903
#define CONNECTION_RES 29903
#define SEND_CONNECTION_CODE 29904
#define NOT_IN_PROTOCOL_FORM_CLIENT 10000
#define NOT_IN_PROTOCOL_FROM_SERVER 20000
#define SIGN_IN 10101
#define SIGN_IN_RES 20101
#define SIGN_UP 10201
#define SIGN_UP_RES 20201
#define FORGOT_PASSWORD 11001
#define FORGOT_PASSWORD_RES 21001
#define CLIENT_LOG_OUT 19999
#define CLIENT_EXIT_APPLICATION 19998
#define MESSAGE_FROM_SENDER 10901
#define MESSAGE_FROM_SENDER_RES 20901
#define PIC_FROM_SENDER 10902
#define PIC_FROM_SENDER_RES 20902
#define SEND_MESSAGE_TO_RECIEVER 20903
#define SEND_MESSAGE_TO_RECIEVER_RES 10903
#define SEND_PIC_TO_RECIEVER 20904
#define SEND_PIC_TO_RECIEVER_RES 10904
#define CREATE_NEW_POST 11201
#define CREATE_NEW_POST_RES 21201
#define ADD_COMMENT_TO_POST 11301
#define ADD_COMMENT_TO_POST_RES 21301
#define POST_REQUEST 10502
#define POST_REQUEST_RES 20502
#define REFRESH_REQUEST 10501
#define REFRESH_REQUEST_RES 20501
#define COMMENTS_UPDATE 10801
#define COMMENTS_UPDATE_RES 20801
#define VOTE 10802
#define VOTE_RES 20802
#define SEND_LINK_FROM_CLIENT 11501
#define SEND_LINK_FROM_CLIENT_RES 21501
#define SEND_POST_LINK_TO_CLIENT 20905
#define	SEND_POST_LINK_TO_CLIENT_RES 10905
#define SEND_PROFILE_LINK_TO_CLIENT 20906
#define SEND_PROFILE_LINK_TO_CLIENT_RES 10906
#define SAVED_POSTS_REQ 11601
#define SAVED_POSTS_REQ_RES 21601
#define DELETE_ALL_SAVED_POSTS 11602
#define DELETE_ALL_SAVED_POSTS_RES 21602
#define DELETE_SAVED_POSTS 11603
#define DELETE_SAVED_POSTS_RES 21603
#define ADD_POST_TO_READ_LATER 11604
#define ADD_POST_TO_READ_LATER_RES 21604
#define PROFILE_REQUEST 19901
#define PROFILE_REQUEST_RES 29901
#define PROFILE_PIC_REQUEST 19902
#define PROFILE_PIC_REQUEST_RES 29902
#define POST_SEARCH_REQ 11101
#define POST_SEARCH_REQ_RES 21101
#define PROFILE_PICTURE_UPDATE_REQ 10601
#define PROFILE_PICTURE_UPDATE_REQ_RES 20601
#define STATUS_UPDATE_REQ 10602
#define STATUS_UPDATE_REQ_RES 20602
#define PERSONAL_INFO_UPDATE_REQ 10603
#define PERSONAL_INFO_UPDATE_REQ_RES 20603
#define USER_DATA_UPDATE_REQ 11701
#define USER_DATA_UPDATE_REQ_RES 21701
#define BACKGROUND_UPDATE_REQ 11702
#define BACKGROUND_UPDATE_REQ_RES 21702
#define BLOCK_REQ 11703
#define BLOCK_REQ_RES 21703
#define USER_SEARCH_REQ 11401
#define USER_SEARCH_REQ_RES 21401
#define KEEP_ALIVE 19997
#define KEEP_ALIVE_SERVER 29997
#define KILL_MAIN_PASSIVE_READ 19996
#define NTF_TEXT 10001
#define NTF_POST 10002
#define NTF_PROFILE 10003
#define NTF_IMAGE 10004
using namespace std;

//Calls for functions in DataBase that check if the DBs can be opened and run the background functions.
ServerClass::ServerClass()
{
	DataBase::DataBase();
	//activating the background functions.
	thread tr = thread(&DataBase::background_posts_updater, &_db);
	tr.detach();
	tr = thread(&DataBase::background_comments_updater, &_db);
	tr.detach();
}

ServerClass::~ServerClass()
{

}

//calls for bindAndListen and infinitely calls for _accept().
void ServerClass::serve()
{
	bindAndListen();
	//Accepting clients's connections
	while (true)
		_accept();
}

//executes the bind and listen functions.
void ServerClass::bindAndListen()
{
	WSADATA info;
	int checker = WSAStartup(MAKEWORD(2, 0), &info), endConnection = 0;
	if (checker)
		cout << "problem with WSAStartup" << endl;
	connectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (connectSocket == INVALID_SOCKET)
		cout << "problem with socket function: " << WSAGetLastError() << endl;
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(IP);
	clientService.sin_port = htons(PORT);
	checker = ::bind(connectSocket, (struct sockaddr *) &clientService, sizeof(clientService));
	if (checker == SOCKET_ERROR)
		cout << "problem with bind: " << WSAGetLastError() << endl;
	else
		cout << "Server socket succesfuly formed: " << connectSocket << endl;
	checker = listen(connectSocket, SOMAXCONN);
	if (checker == SOCKET_ERROR)
		cout << "problem with listen: " << WSAGetLastError() << endl;

}

//gets connection requests from clients.
void ServerClass::_accept()
{
	char buf[BUFF_SIZE] = "";
	string msg;
	int addrlen = sizeof(clientService);
	timeval tmv;
	tmv.tv_sec = 1;
	tmv.tv_usec = 1000000;
	fd_set sockHelp;
	sockHelp.fd_count = 1;
	clientSocket = accept(connectSocket, (sockaddr*)&clientService, &addrlen);
	if (clientSocket == INVALID_SOCKET)
	{
		cout << "accept failed" << endl;
		return;
	}
	int check = 0, buffEmptyCheck;
	sockHelp.fd_array[0] = clientSocket;
	buffEmptyCheck = select(1, &sockHelp, NULL, NULL, &tmv);
	if (buffEmptyCheck > 0)
		check = recv(clientSocket, buf, BUFF_SIZE, 0);
	msg = buf;
	if (check <= 0 || buffEmptyCheck <= 0)
	{
		closesocket(clientSocket);
		return;
	}
	else if (stoi(msg.substr(0, 5)) == CONNECTION)
	{
		connectionHandler(clientSocket, msg.substr(5));
	}
	else
		return; //This isn't suppose to happen.
}

//listens to the active socket and calls the suitable functions for the user's request.
void ServerClass::clientHandler(User* user)
{
	while (connectedUsersLocked);
	connectedUsersLocked = true;
	SOCKET clientSock = user->getSocket();
	connectedUsersLocked = false;
	timeval tmv;
	tmv.tv_sec = 2;
	tmv.tv_usec = 2000000;
	fd_set sockHelp;
	sockHelp.fd_count = 1;
	sockHelp.fd_array[0] = clientSock;
	string msg = "", data = "";
	int code;
	char msg_c[BUFF_SIZE] = "";
	int check, buffEmptyCheck;
	while (true)
	{
		emptyString(msg_c, 0, BUFF_SIZE - 1);
		sockHelp.fd_count = 1;
		buffEmptyCheck = select(1, &sockHelp, NULL, NULL, &tmv);
		if (buffEmptyCheck > 0)
			check = recv(clientSock, msg_c, BUFF_SIZE, 0);
		if (buffEmptyCheck <= 0 || check <= 0)
		{
			while (connectedUsersLocked);
			connectedUsersLocked = true;
			try
			{
				if (user->getActiveOpen())
					clientDisconnection(user, JUST_ACTIVE);
			}
			catch (...){}
			connectedUsersLocked = false;
			break;
		}
		try
		{
			msg = msg_c;
			code = stoi(msg.substr(0, 5));
			data = msg.substr(5);
		}
		catch (...)
		{
			cout << "ServerClass::clientHandler - exception. msg_c = " << msg_c << endl;
			continue;
		}
		switch (code)
		{
		case KEEP_ALIVE:
			break;
		case SIGN_IN:
			handleSignIn(user, data, true);
			break;
		case SIGN_UP:
			handleSignUp(user, data);
			break;
		case FORGOT_PASSWORD:
			handleForgotPassword(user, data);
			break;
		case CREATE_NEW_POST:
			handleCreateNewPost(user, data);
			break;
		case ADD_COMMENT_TO_POST:
			handleAddNewCommentToPost(user, data);
			break;
		case POST_REQUEST:
			handlePostRequest(user, data);
			break;
		case COMMENTS_UPDATE:
			handleUpdateComments(user, data);
			break;
		case REFRESH_REQUEST:
			handleRefreshRequest(user, data);
			break;
		case VOTE:
			handleVote(user, data);
			break;
		case SAVED_POSTS_REQ:
			handleSavedPosts(user, data);
			break;
		case DELETE_ALL_SAVED_POSTS:
			handleDeleteAllSavedPosts(user, data);
			break;
		case DELETE_SAVED_POSTS:
			handleDeleteSavedPosts(user, data);
			break;
		case ADD_POST_TO_READ_LATER:
			handleAddReadLaterPost(user, data);
			break;
		case POST_SEARCH_REQ:
			handlePostSearch(user, data);
			break;
		case STATUS_UPDATE_REQ:
			handleStatusUpdate(user, data);
			break;
		case PERSONAL_INFO_UPDATE_REQ:
			handlePersonalInfoUpdate(user, data);
			break;
		case PROFILE_PICTURE_UPDATE_REQ:
			handleProfilePicUpdate(user, data);
			break;
		case PROFILE_REQUEST:
			handleProfileReq(user, data);
			break;
		case PROFILE_PIC_REQUEST:
			handleProfilePicReq(user, data);
			break;
		case USER_DATA_UPDATE_REQ:
			handleUserDataUpdateReq(user, data);
			break;
		case BACKGROUND_UPDATE_REQ:
			handleBackgroundUpdateReq(user, data);
			break;
		case BLOCK_REQ:
			handleBlockReq(user, data);
			break;
		case USER_SEARCH_REQ:
			handleUserSearch(user, data);
			break;
		case CLIENT_LOG_OUT:
			handleLogOut(user);
			break;
		default:
			handleNotInProtocol(clientSock, msg);
		}

	}
}

//listens to the passive socket and calls the suitable functions for the user's request.
void ServerClass::passiveClientHandler(User* user)
{
	while (connectedUsersLocked);
	connectedUsersLocked = true;
	SOCKET clientSock = user->getPassiveSocket();
	connectedUsersLocked = false;
	timeval tmv;
	tmv.tv_sec = 2;
	tmv.tv_usec = 2000000;
	fd_set sockHelp;
	sockHelp.fd_count = 1;
	sockHelp.fd_array[0] = clientSock;
	string msg = "", data = "";
	int code;
	thread tr;
	char msg_c[BUFF_SIZE] = "";
	int check, buffEmptyCheck;
	while (true)
	{
		//while (user->_mainPassiveSocketKilled == true);
		emptyString(msg_c, 0, BUFF_SIZE - 1);
		string userBuff = user->getUserBuffer();
		if (user->getUserBuffer().empty() == false)
		{
			strcpy(msg_c, userBuff.c_str());
			user->setUserBuffer("");
		}
		else
		{

			sockHelp.fd_count = 1;
			buffEmptyCheck = select(1, &sockHelp, NULL, NULL, &tmv);
			if (buffEmptyCheck > 0)
				check = recv(clientSock, msg_c, BUFF_SIZE, 0);

			if (buffEmptyCheck <= 0 || check <= 0)
			{
				while (connectedUsersLocked);
				connectedUsersLocked = true;
				try
				{

					if (user->getPassiveOpen())
						clientDisconnection(user, ALL_OUT_CLOSURE);
				}
				catch (...) {}
				connectedUsersLocked = false;
				break;
			}
		}
		try
		{
			msg = msg_c;
			code = stoi(msg.substr(0, 5));
			data = msg.substr(5);
		}
		catch (...)
		{
			cout << "ServerClass::passiveClientHandler - exception. msg_c = " << msg_c << endl;
			continue;
		}

		if (code == KEEP_ALIVE)
		{
			if (msg.length() > 5)
			{
				try
				{
					code = stoi(msg.substr(5, 5));
					data = msg.substr(10);
				}
				catch (...)
				{
					cout << "ServerClass::passiveClientHandler - exception. msg_c = " << msg_c << endl;
					continue;
				}
			}
			else
				continue;
		}
		switch (code)
		{
			case NTF_TEXT:
				sendText(user, data);
				break;
			case NTF_POST:
				sendPost(user, data);
				break;
			case NTF_PROFILE:
				sendProfile(user, data);
				break;
			case NTF_IMAGE:
				sendPic(user, data);
				break;
			case KILL_MAIN_PASSIVE_READ:
				cout << "vut ze fuk?" << endl;
				user->_mainPassiveSocketKilled = true;
				break;
			case MESSAGE_FROM_SENDER:
				handleGotMessageFromSender(user, data);
				break;
			case SEND_LINK_FROM_CLIENT:
				if (data[0] == '0')
				{	
					handleSendProfileLink(user, data);
				}
				else if (data[0] == '1')
				{
					handleSendPostLink(user, data);
				}
				else
					handleNotInProtocol(clientSock, msg);
				break;
			case PIC_FROM_SENDER:
				handleSendPic(user, data);
				break;
			case CLIENT_EXIT_APPLICATION:
				while (connectedUsersLocked);
				connectedUsersLocked = true;
				clientDisconnection(user, JUST_ACTIVE);
				connectedUsersLocked = false;
				break;
			default:
				handleNotInProtocol(clientSock, msg);

		}
	}
}

//Responsible to get the message.
void ServerClass::handleGotMessageFromSender(User* user, string data)
{
	while(connectedUsersLocked);
	connectedUsersLocked = true;
	SOCKET clientSock = user->getPassiveSocket();
	int numOfConnectedUsers = _connectedUsers.size();
	string userID = user->getID();
	connectedUsersLocked = false;
	timeval tmv;
	tmv.tv_sec = 3;
	tmv.tv_usec = 3000000;
	char recv_buff[6];
	fd_set sockHelp;
	string msg = to_string(SEND_MESSAGE_TO_RECIEVER);
	int i, reciever_name_len;
	SOCKET recieverSocket;
	string reciever_name, sending_time, message_len, message, creation_time, sender_name, sender_name_len_str, reciever_id;
	try
	{
		reciever_name_len = stoi(data.substr(0, 2));
		reciever_name = data.substr(2, reciever_name_len);
		sending_time = data.substr(2 + reciever_name_len, 13);
		message_len = data.substr(2 + reciever_name_len + 13, 3);
		message = data.substr(2 + reciever_name_len + 13 + 3, stoi(message_len));
		creation_time = padd_zeroes(to_string(time_now() - stoll(sending_time)), 13);
		sender_name = user->getName();
		reciever_id = _db.getIDByName(reciever_name);
		if (reciever_id == "0")
		{
			//user->_mainPassiveSocketLocked = false;
			return;
		}
		sender_name_len_str = padd_zeroes(to_string(sender_name.length()), 2);
	}
	catch (...)
	{
		//user->_mainPassiveSocketLocked = false;
		return;
	}
	if (_db.checkIfBlocked(userID, reciever_id))
	{
		//user->_mainPassiveSocketLocked = false;
		return;
	}
	bool userConnected = false, *passive_sock_lock = NULL, *mainPassiveSocketKilled = NULL;
	while (connectedUsersLocked);
	connectedUsersLocked = true;
	for (i = 0; i < numOfConnectedUsers; i++)
	{
		if (_connectedUsers[i]->getName() == reciever_name)
		{
			recieverSocket = _connectedUsers[i]->getPassiveSocket();
			passive_sock_lock = &_connectedUsers[i]->_passiveSocketLocked;
			if (recieverSocket != 0)
				userConnected = true;
			break;
		}
	}
	connectedUsersLocked = false;
	if (!userConnected)
	{
		pair<string, string> p;
		p.first = reciever_name;
		p.second = creation_time + sender_name_len_str + sender_name + message_len + message;
		text_messages.insert(p);
		
	}
	else
	{
		msg += sender_name_len_str;
		msg += sender_name;
		msg += sending_time;
		msg += message_len;
		msg += message;
		msg += padd_zeroes(userID, 8);
		_connectedUsers[i]->setUserBuffer(to_string(NTF_TEXT) + msg);

		/*
		bool needToAdd = false;
		while (*passive_sock_lock);
		*passive_sock_lock = true;
		Send(recieverSocket, msg.c_str(), msg.size());
		*passive_sock_lock = false;
		while (!*mainPassiveSocketKilled);
		sockHelp.fd_array[0] = recieverSocket;
		sockHelp.fd_count = 1;
		int check = select(1, &sockHelp, NULL, NULL, &tmv);
		if (check > 0)
		{
			recv(recieverSocket, recv_buff, 6, 0);
			try
			{
				if (atoi(recv_buff) != stoi(to_string(SEND_MESSAGE_TO_RECIEVER_RES) + "0"))
				{
					needToAdd = true;
				}
			}
			catch (...)
			{
				needToAdd = true;
			}
		}
		else
		{
			needToAdd = true;
		}
		*passive_sock_lock = false;
		if (needToAdd)
		{
			pair<string, string> p;
			p.first = reciever_name;
			p.second = creation_time + sender_name_len_str + sender_name + message_len + message;
			text_messages.insert(p);
			_db.insertNewMessage(message, "text", reciever_id, userID, "", creation_time, "sender");
		}
		else
			_db.insertNewMessage(message, "text", reciever_id, userID, "", creation_time, "both");
		*mainPassiveSocketKilled = false;
		*/
	}
	_db.insertNewMessage(message, "text", reciever_id, userID, "", creation_time, "sender");
	//user->_mainPassiveSocketLocked = false;

}

//sends a message to its reciever.
void ServerClass::sendText(User* user, string data)
{
	while (connectedUsersLocked);
	connectedUsersLocked = true;
	SOCKET recieverSocket = user->getPassiveSocket();
	string reciever_id = user->getID();
	string reciever_name = user->getName();
	connectedUsersLocked = false;
	
	char recv_buff[6];
	bool needToAdd = false;
	bool firstTime = true;
	int str_size = data.size(), check;
	string sender_id = data.substr(str_size - 8, 8);
	int sender_name_len = stoi(data.substr(5, 2));
	string sender_name = data.substr(5 + 2, sender_name_len);
	string creation_time = data.substr(5 + 2 + sender_name_len, 13);
	int message_len = stoi(data.substr(5 + 2 + sender_name_len + 13, 3));
	string message = data.substr(5 + 2 + sender_name_len + 13 + 3, message_len);
	fd_set sockHelp;
	timeval tmv;
	tmv.tv_sec = 3;
	tmv.tv_usec = 3000000;
	
	while (user->_passiveSocketLocked);
	user->_passiveSocketLocked = true;
	//canceling the KEEP_ALIVE:
	string KP_status_msg = to_string(KEEP_ALIVE_SERVER) + "1";
	Send(recieverSocket, KP_status_msg.c_str(), KP_status_msg.size());
	sockHelp.fd_array[0] = recieverSocket;
	sockHelp.fd_count = 1;
	char buff[BUFF_SIZE];
	/*int check = select(1, &sockHelp, NULL, NULL, &tmv);
	if (check > 0)
		recv(recieverSocket, buff, BUFF_SIZE, 0);*/

	Send(recieverSocket, data.substr(0, str_size - 8).c_str(), str_size - 8);
	sockHelp.fd_array[0] = recieverSocket;
	int i;
	for (i = 0; true; i++)
	{
		sockHelp.fd_count = 1;
		check = select(1, &sockHelp, NULL, NULL, &tmv);
		if (check > 0)
		{
			recv(recieverSocket, recv_buff, 5, 0);
			try
			{
				if (atoi(recv_buff) == stoi(to_string(KEEP_ALIVE)) && i < 2)
				{
					cout << "got keep alive" << endl;
				}
				else
				{
					sockHelp.fd_count = 1;
					int check = select(1, &sockHelp, NULL, NULL, &tmv);
					if (check > 0)
						recv(recieverSocket, recv_buff + 5, 1, 0);
					else
					{
						needToAdd = true;
						break;
					}
					if (atoi(recv_buff) == stoi(to_string(SEND_MESSAGE_TO_RECIEVER_RES) + "0"))
					{
						cout << "good" << endl;
						break;
					}
					else
					{
						needToAdd = true;
						break;
					}
				}
			}
			catch (...)
			{
				needToAdd = true;
				break;
			}
		}
		else
		{
			needToAdd = true;
			break;
		}
	}
	KP_status_msg[5] = '0';
	Send(recieverSocket, KP_status_msg.c_str(), KP_status_msg.size());
	user->_passiveSocketLocked = false;
	if (needToAdd)
	{
		pair<string, string> p;
		p.first = reciever_name;
		p.second = creation_time + to_string(sender_name_len) + sender_name + to_string(message_len) + message;
		text_messages.insert(p);
	}
	if (!needToAdd)
		_db.insertNewMessage(message, "text", reciever_id, sender_id, "", creation_time, "reciever");
}

//handles sign in requests. Returns true upon successful sign in.
bool ServerClass::handleSignIn(User* user, string data, bool passive_or_active) //true - active, false - passive
{
	while (connectedUsersLocked);
	connectedUsersLocked = true;
	SOCKET clientSock;
	if (passive_or_active == true)
		clientSock = user->getSocket();
	else
		clientSock = user->getPassiveSocket();
	connectedUsersLocked = false;
	try
	{
		string msg = to_string(SIGN_IN_RES);
		int name_len = stoi(data.substr(0, 2));
		string name = data.substr(2, name_len);
		int pass_len = stoi(data.substr(2 + name_len, 2));
		string pass = data.substr(2 + name_len + 2, pass_len);
		string user_id = _db.getIDByName(name);

		bool alreadyConnected = false;
		if (user_id == "0")
		{
			msg += "1";
			Send(clientSock, msg.c_str(), msg.size());
			return false;
		}
		else if (_db.isNameAndPassMatch(name, pass) == false) //if user and password DON'T match
		{
			msg += "1";
			Send(clientSock, msg.c_str(), msg.size());
			return false;
		}
		else //if user and password DO match
		{
			while (connectedUsersLocked);
			connectedUsersLocked = true;
			if (_connectedUsers.empty() == false) //checking if user is already connected.
			{
				int i = 0;
				for (i = 0; i < (signed)_connectedUsers.size(); i++)
				{
					if (_connectedUsers[i]->getID() == user_id)
					{
						alreadyConnected = true;
						break;
					}
				}
			}
			connectedUsersLocked = false;

			if (alreadyConnected) //user already connected.
			{
				msg += "2";
				Send(clientSock, msg.c_str(), msg.size());
				return false;
			}

			else //success
			{
				while (connectedUsersLocked);
				connectedUsersLocked = true;
				user->setID(user_id);
				user->setName(name);
				connectedUsersLocked = false;
				msg += "0";
				Send(clientSock, msg.c_str(), msg.size());
				return true;
			}
		}
	}
	catch (...)
	{
		return false;
	}
}

//hanldes sign up requests.
void ServerClass::handleSignUp(User* user, string data)
{
	while (connectedUsersLocked);
	connectedUsersLocked = true;
	SOCKET clientSock = user->getSocket();
	connectedUsersLocked = false;
	string msg = to_string(SIGN_UP_RES);
	int name_len, pass_len, email_len;
	string name, pass, email;
	try
	{
		name_len = stoi(data.substr(0, 2));
		name = data.substr(2, name_len);
		pass_len = stoi(data.substr(2 + name_len, 2));
		pass = data.substr(2 + name_len + 2, pass_len);
		email_len = stoi(data.substr(2 + name_len + 2 + pass_len, 2));
		email = to_lower_case(data.substr(2 + name_len + 2 + pass_len + 2, email_len));
	}
	catch (...)
	{
		return;
	}
	if (_db.isUserAlreadyExist(name) == true)
	{
		msg += "1";		
		Send(clientSock, msg.c_str(), msg.size());
	}
	else if (_db.isEmailAlreadyExist(email) == true)
	{
		msg += "2";
		Send(clientSock, msg.c_str(), msg.size());
	}
	else
	{
		if (_db.insertNewUser(name, pass, email) == true)
		{
			msg += "0";
			Send(clientSock, msg.c_str(), msg.size());
		}
		else
			cout << endl << "ServerClass::handleSignUp - _db.insertNewUser failed." << endl;
	}
}

//handles forgot password requests.
void ServerClass::handleForgotPassword(User* user, string data)
{
	while (connectedUsersLocked);
	connectedUsersLocked = true;
	SOCKET clientSock = user->getSocket();
	connectedUsersLocked = false;
	string msg = to_string(FORGOT_PASSWORD_RES);
	int name_len, email_len;
	string name, email;
	try
	{
		name_len = stoi(data.substr(0, 2));
		name = data.substr(2, name_len);
		email_len = stoi(data.substr(2 + name_len, 2));
		email = to_lower_case(data.substr(2 + name_len + 2, email_len));
	}
	catch (...)
	{
		msg += "3";
		Send(clientSock, msg.c_str(), msg.size());
		return;
	}
	if (_db.isUserAlreadyExist(name) == false)
	{
		//user doesn't exist.
		msg += "2";
		
		Send(clientSock, msg.c_str(), msg.size());
	}
	else if (_db.IsNameAndEmailMatch(name, email) == false)
	{
		//name and email don't match.
		msg += "1";
		
		Send(clientSock, msg.c_str(), msg.size());
	}
	else
	{
		string new_password = rndStr();
		sendEmail(email, new_password); //send email to user.
		if (_db.updatePassword(name, new_password))
			msg += "0";
		else
			msg += "3";
		Send(clientSock, msg.c_str(), msg.size());
	}
}

//handles a case when a client sent something that's not in protocol.
void ServerClass::handleNotInProtocol(SOCKET& clientSock, string data)
{
	string msg = to_string(NOT_IN_PROTOCOL_FROM_SERVER);
	msg += data;
	
	Send(clientSock, msg.c_str(), msg.size());
}

//handles a post creation request.
void ServerClass::handleCreateNewPost(User* user, string data)
{
	while (connectedUsersLocked);
	connectedUsersLocked = true;
	SOCKET clientSock = user->getSocket();
	connectedUsersLocked = false;
	string msg = to_string(CREATE_NEW_POST_RES);
	bool unanimus;
	int content_len;
	string content;
	try
	{
		content_len = stoi(data.substr(0, 3));
		content = data.substr(3, content_len);
		unanimus = data[3 + content_len] == '0' ? false : (data[3 + content_len] == '1' ? true : throw("not in protocol"));
	}
	catch (...)
	{
		msg += "1";
		Send(clientSock, msg.c_str(), msg.size());
		return;
	}
	try
	{
		string categoryID = data.substr(3 + content_len + 1, 2);
		string min_age = data.substr(3 + content_len + 1 + 2, 2);
		string max_age = data.substr(3 + content_len + 1 + 2 + 2, 2);
		string hobbies = data.substr(3 + content_len + 1 + 2 + 2 + 2, 2);
		string occupaiton = data.substr(3 + content_len + 1 + 2 + 2 + 2 + 2, 2);
		string interests = data.substr(3 + content_len + 1 + 2 + 2 + 2 + 2 + 2, 2);
		if (_db.insertNewPost((unanimus ? "" : user->getName()), content, categoryID, user->getID(), min_age, max_age, hobbies, occupaiton, interests))
			msg += "0";
		else
			msg += "1";
		Send(clientSock, msg.c_str(), msg.size());
	}
	catch (...)
	{
		msg += "1";
		Send(clientSock, msg.c_str(), msg.size());
	}
}

//handles a request to add a comment to a post.
void ServerClass::handleAddNewCommentToPost(User* user, string data)
{
	while (connectedUsersLocked);
	connectedUsersLocked = true;
	SOCKET clientSock = user->getSocket();
	connectedUsersLocked = false;
	string msg = to_string(ADD_COMMENT_TO_POST_RES);
	string post_id;
	bool unanimus;
	try
	{
		post_id = data.substr(0, 8);
		unanimus = data[8] == '1' ? false : (data[8] == '0' ? true : throw("not in protocol"));
	}
	catch (...)
	{
		msg += "1";
		Send(clientSock, msg.c_str(), msg.size());
		return;
	}
	int comment_len = stoi(data.substr(8 + 1, 3));
	string comment = data.substr(8 + 1 + 3, comment_len);
	string comment_id = _db.insertNewComment(to_string(stoi(post_id)), comment, (unanimus == true ? "" : user->getName()), user->getID());
	
	if (comment_id == "0")
		msg += "1";
	else
		msg += "0" + comment_id;
	Send(clientSock, msg.c_str(), msg.size());
}

//handles a post request.
void ServerClass::handlePostRequest(User* user, string data)
{
	while (connectedUsersLocked);
	connectedUsersLocked = true;
	SOCKET clientSock = user->getSocket();
	connectedUsersLocked = false;
	string msg = to_string(POST_REQUEST_RES);
	try
	{
		string post_id = data.substr(0, 8);
		string info = _db.postInfo(post_id, user->getID());
		msg += info;
	}
	catch (...)
	{
		msg += "0";
		Send(clientSock, msg.c_str(), msg.size());
		return;
	}
	Send(clientSock, msg.c_str(), msg.size());
}

//handles a comments update request.
void ServerClass::handleUpdateComments(User* user, string data)
{
	while (connectedUsersLocked);
	connectedUsersLocked = true;
	SOCKET clientSock = user->getSocket();
	string userID = user->getID();
	connectedUsersLocked = false;
	string msg;
	try
	{
		string postID = data.substr(0, 8);
		int numOfCurrentComments = stoi(data.substr(8, 3));
		msg = to_string(COMMENTS_UPDATE_RES);
		msg += _db.updateCommentsOnPost(postID, numOfCurrentComments, userID);
	}
	catch (...)
	{
		msg += "1";
		Send(clientSock, msg.c_str(), msg.size());
	}
	Send(clientSock, msg.c_str(), msg.size());
}

//handles a refresh request.
void ServerClass::handleRefreshRequest(User* user, string data)
{
	while (connectedUsersLocked);
	connectedUsersLocked = true;
	SOCKET clientSock = user->getSocket();
	string userID = user->getID();
	connectedUsersLocked = false;
	string msg = to_string(REFRESH_REQUEST_RES);
	string newest_OR_hottest, category, num_of_current_posts;
	try
	{
		category = data.substr(1, 2);
		num_of_current_posts = data.substr(3, 4);
		newest_OR_hottest = data[0] == '1' ? "newest" : (data[0] == '2' ? "hottest" : throw("error"));
	}
	catch (string errstr)
	{
		cout << "ServerClass::handleRefreshRequest - " << errstr << endl;
		return;
	}
	string info;
	if (newest_OR_hottest == "newest")
	{
		info = _db.getNewestPosts(category, num_of_current_posts, userID);
	}
	else if (newest_OR_hottest == "hottest")
	{
		info = _db.getHottestPosts(category, num_of_current_posts, userID);
	}
	msg += info;
	Send(clientSock, msg.c_str(), msg.size());
}

//handles a vote request.
void ServerClass::handleVote(User* user, string data)
{
	while (connectedUsersLocked);
	connectedUsersLocked = true;
	SOCKET clientSock = user->getSocket();
	connectedUsersLocked = false;
	string msg = to_string(VOTE_RES);
	string post_id, comment_num, growth_or_decline_size;
	int up_or_down;
	try
	{
		post_id = data.substr(0, 8);
		comment_num = data.substr(8, 8);
		up_or_down = data[16] == '0' ? 1 : (data[16] == '1' ? -1 : throw("up_or_down value isn't good"));
		growth_or_decline_size = data.substr(17, 1);
	}
	catch (...)
	{
		msg += "1";
		Send(clientSock, msg.c_str(), msg.size());
		return;
	}
	if (growth_or_decline_size != "0" && growth_or_decline_size != "1" && growth_or_decline_size != "2")
	{
		cout << "ServerClass::handleVote - growth_or_decline_size value isn't good" << endl;
		return;
	}
	if (_db.updateVotes(post_id, comment_num, up_or_down, growth_or_decline_size, user->getID()))
		msg += "0";
	else
		msg += "1";
	Send(clientSock, msg.c_str(), msg.size());
}

//handles a request to send a link to a post.
void ServerClass::handleSendPostLink(User* user, string data)
{
	while (connectedUsersLocked);
	connectedUsersLocked = true;
	SOCKET clientSock = user->getSocket();
	string username = user->getName();
	string userID = user->getID();
	connectedUsersLocked = false;
	timeval tmv;
	tmv.tv_sec = 3;
	tmv.tv_usec = 3000000;
	char recv_buff[6];
	SOCKET recieverSocket;
	string message1, message2, message;
	int reciepients_count = stoi(data.substr(1, 2));
	string sending_time, the_link, reciever_name, reciever_id;
	try
	{
		sending_time = to_string(time_now() - stoll(data.substr(3, 13)));
		the_link = data.substr(16, 8); //post ID
	}
	catch (...)
	{
		//user->_mainPassiveSocketLocked = false;
		return;
	}
	message1 = to_string(SEND_POST_LINK_TO_CLIENT);
	message1 += padd_zeroes(to_string(username.length()), 2);
	message1 += username;
	message2 = the_link; //post ID.
	int i, index, numOfConnectedUsers, name_len;
	string post_data = _db.shortenedPostInfo(the_link);
	message2 += post_data;
	bool userConnected = false, *passive_sock_lock = NULL, needToAdd;
	for (index = 0, i = 0; i < reciepients_count; i++)
	{
		try
		{
			needToAdd = false;
			name_len = stoi(data.substr(24 + index, 2));
			reciever_name = data.substr(24 + index + 2, name_len);
			reciever_id = _db.getIDByName(reciever_name);
			if (reciever_id == "0")
				continue;
			if (_db.checkIfBlocked(userID, reciever_id))
				continue;
			if (_db.checkRequirementsMatch(the_link, reciever_id) == false)
				continue;
			numOfConnectedUsers = _connectedUsers.size();
			userConnected = false;
			passive_sock_lock = NULL;
			while (connectedUsersLocked);
			connectedUsersLocked = true;
			for (i = 0; i < numOfConnectedUsers; i++)
			{
				if (_connectedUsers[i]->getName() == reciever_name)
				{
					recieverSocket = _connectedUsers[i]->getPassiveSocket();
					passive_sock_lock = &_connectedUsers[i]->_passiveSocketLocked;
					userConnected = true;
					connectedUsersLocked = false;
					break;
				}
			}
			connectedUsersLocked = false;

			if (!userConnected)
			{
				needToAdd = true;
			}

			if (userConnected)
			{
				message = message1 + sending_time + message2 + padd_zeroes(to_string(time_now() - stoll(userID)), 8);
				_connectedUsers[i]->setUserBuffer(to_string(NTF_POST) + message);
				/*while (*passive_sock_lock);
				*passive_sock_lock = true;
				Send(recieverSocket, message.c_str(), message.size());
				fd_set sockHelp;
				sockHelp.fd_count = 1;
				sockHelp.fd_array[0] = recieverSocket;
				while (!*mainPassiveSocketKilled);
				int check = select(1, &sockHelp, NULL, NULL, &tmv);
				if (check != SOCKET_ERROR && check != 0)
				{
					try
					{
						if (atoi(recv_buff) != stoi(to_string(SEND_POST_LINK_TO_CLIENT_RES) + "0"))
							needToAdd = true;
					}
					catch (...)
					{
						needToAdd = true;
					}
				}
				else
				{
					needToAdd = true;
				}
				*passive_sock_lock = false;
				index += 2 + name_len;
				_db.insertNewMessage(the_link, "post_link", reciever_id, userID, "", sending_time, "both");
				*mainPassiveSocketKilled = false;*/
				_db.insertNewMessage(the_link, "post_link", reciever_id, userID, "", sending_time, "sender");

			}

			if (needToAdd)
			{
				pair<string, string> p;
				p.first = reciever_name;
				int help_size = message2.size();
				string post_creation_time = padd_zeroes(to_string(time_now() - stoll(message2.substr(help_size - 13, 13))), 13);
				message2.replace(help_size - 13, 13, post_creation_time);
				p.second = message1 + padd_zeroes(sending_time, 13) + message2;
				post_links.insert(p);
				_db.insertNewMessage(the_link, "post_link", reciever_id, userID, "", sending_time, "sender");
			}
		}
		catch (...)
		{

		}
	}
	//user->_mainPassiveSocketLocked = false;
}

void ServerClass::sendPost(User* user, string data)
{
	while (connectedUsersLocked);
	connectedUsersLocked = true;
	SOCKET recieverSocket = user->getPassiveSocket();
	string reciever_id = user->getID();
	connectedUsersLocked = false;

	char recv_buff[6];
	fd_set sockHelp;
	timeval tmv;
	tmv.tv_sec = 1;
	tmv.tv_usec = 1000000;
	bool needToAdd = false;
	bool firstTime = true;
	int str_size = data.size();
	string sender_id = data.substr(str_size - 8, 8);
	int sender_name_len = stoi(data.substr(5, 2));
	string sending_time = data.substr(5 + 2 + sender_name_len, 13);
	string the_link = data.substr(5 + 2 + sender_name_len + 13, 8);
	data.replace(5 + 2 + sender_name_len, 13, padd_zeroes(to_string(time_now() - stoll(sending_time)), 13));
	while (user->_passiveSocketLocked);
	user->_passiveSocketLocked = true;
	//canceling the KEEP_ALIVE:
	string KP_status_msg = to_string(KEEP_ALIVE_SERVER) + "1";
	Send(recieverSocket, KP_status_msg.c_str(), KP_status_msg.size());
	sockHelp.fd_array[0] = recieverSocket;
	sockHelp.fd_count = 1;
	char buff[BUFF_SIZE];
	int check = select(1, &sockHelp, NULL, NULL, &tmv);
	if (check > 0)
		recv(recieverSocket, buff, BUFF_SIZE, 0);


	Send(recieverSocket, data.substr(0, str_size - 8).c_str(), str_size - 8);
	int i;
	//SEND_POST_LINK_TO_CLIENT_RES
	for (i = 0; true; i++)
	{
		sockHelp.fd_count = 1;
		int check = select(1, &sockHelp, NULL, NULL, &tmv);
		if (check > 0)
		{
			recv(recieverSocket, recv_buff, 5, 0);
			try
			{
				if (atoi(recv_buff) == stoi(to_string(KEEP_ALIVE)) && i < 2)
				{
					cout << "got keep alive" << endl;
				}
				else
				{
					sockHelp.fd_count = 1;
					int check = select(1, &sockHelp, NULL, NULL, &tmv);
					if (check > 0)
						recv(recieverSocket, recv_buff + 5, 1, 0);
					else
					{
						needToAdd = true;
						break;
					}
					if (atoi(recv_buff) == stoi(to_string(SEND_POST_LINK_TO_CLIENT_RES) + "0"))
					{
						cout << "good" << endl;
						break;
					}
					else
					{
						needToAdd = true;
						break;
					}
				}
			}
			catch (...)
			{
				needToAdd = true;
				break;
			}
		}
		else
		{
			needToAdd = true;
			break;
		}
	}
	KP_status_msg[5] = '0';
	Send(recieverSocket, KP_status_msg.c_str(), KP_status_msg.size());
	user->_passiveSocketLocked = false;
	if (!needToAdd)
		_db.insertNewMessage(the_link, "post_link", reciever_id, sender_id, "", sending_time, "reciever");
}

//handles a request to send a link to a profile.
void ServerClass::handleSendProfileLink(User* user, string data)
{
	while (connectedUsersLocked);
	connectedUsersLocked = true;
	SOCKET clientSock = user->getSocket();
	string username = user->getName();
	string userID = user->getID();
	int numOfConnectedUsers = _connectedUsers.size();
	connectedUsersLocked = false;
	timeval tmv;
	tmv.tv_sec = 3;
	tmv.tv_usec = 3000000;
	fd_set sockHelp;
	string message1, message2, message, sending_time_from_client, the_link;
	int reciepients_count;
	try
	{
		reciepients_count = stoi(data.substr(1, 2));
		sending_time_from_client = data.substr(3, 13);// to_string(time_now() - stoll(data.substr(3, 13)));
		the_link = data.substr(16, 8); //profile ID
		message1 = to_string(SEND_PROFILE_LINK_TO_CLIENT);
		message1 += "0";
		message1 += padd_zeroes(to_string(username.length()), 2);
		message1 += username;
	}
	catch (...)
	{
		//user->_mainPassiveSocketLocked = false;
		return;
	}
	string profile_data = _db.shortenedProfileInfo(the_link);
	bool is_pic, *passive_sock_lock = NULL, needToAdd = false, *mainPassiveSocketKilled = NULL;
	int pic_link_len;
	string pic_link;
	ifstream* profile_pic = NULL;
	if (profile_data[0] == '0')
	{
		message2 = profile_data.substr(1);
		is_pic = false;
	}
	else
	{
		pic_link_len = stoi(profile_data.substr(0, 2));
		pic_link = profile_data.substr(2, pic_link_len);
		message2 += profile_data.substr(2 + pic_link_len);
		profile_pic = _db.getProfilePic(username, to_string(username.length()));
		is_pic = true;
	}
	int totalReadChars = 0, currChars, size, i, index;
	if (is_pic)
	{
		profile_pic->seekg(0, profile_pic->end);
		size = profile_pic->tellg();
		profile_pic->seekg(0, profile_pic->beg);
		profile_pic->close();
	}
	else 
		size = 0;
	for (index = 0, i = 0; i < reciepients_count; i++)
	{
		needToAdd = false;
		int name_len;
		string reciever_name, reciever_id;
		try
		{
			name_len = stoi(data.substr(24 + index, 2));
			reciever_name = data.substr(24 + index + 2, name_len);
			reciever_id = _db.getIDByName(reciever_name);
			if (reciever_id == "0")
				continue;
			if (_db.checkIfBlocked(userID, reciever_id))
				continue;
		}
		catch (...)
		{
			continue;
		}
		SOCKET recieverSocket;
		bool userConnected = false;
		while (connectedUsersLocked);
		connectedUsersLocked = true;
		for (i = 0; i < numOfConnectedUsers; i++)
		{
			if (_connectedUsers[i]->getName() == reciever_name)
			{
				recieverSocket = _connectedUsers[i]->getPassiveSocket();
				passive_sock_lock = &_connectedUsers[i]->_passiveSocketLocked;
				mainPassiveSocketKilled = &_connectedUsers[i]->_mainPassiveSocketKilled;
				sockHelp.fd_array[0] = recieverSocket;
				sockHelp.fd_count = 1;
				userConnected = true;
				break;
			}
		}
		connectedUsersLocked = false;
		if (!userConnected)
		{
			needToAdd = true;
		}

		if (needToAdd)
		{
			pair<string, string> p;
			p.first = reciever_name;
			p.second = message1 + sending_time_from_client + message2 + padd_zeroes(to_string(size), 8) + padd_zeroes(to_string(BUFF_SIZE), 7);
			if (is_pic)
				p.second += padd_zeroes(to_string(pic_link_len), 2) + pic_link;
			profile_links.insert(p);
		}
		_db.insertNewMessage(the_link, "post_link", reciever_id, userID, "", sending_time_from_client, "sender");



		if (userConnected)
		{
			message = to_string(NTF_PROFILE) + message1 + sending_time_from_client + message2 + padd_zeroes(to_string(size), 8) + padd_zeroes(to_string(BUFF_SIZE), 7) + padd_zeroes(userID, 8);
			if (is_pic)
				message += padd_zeroes(to_string(pic_link_len), 2) + pic_link;
			_connectedUsers[i]->setUserBuffer(message);
			







			/*
			char final_msg[BUFF_SIZE];
			char buff[BUFF_SIZE];
			char recv_buff[7] = "";
			for (i = 0; i < 5; i++)
				final_msg[i] = to_string(SEND_PROFILE_LINK_TO_CLIENT)[i];
			final_msg[5] = '1';


			while (*passive_sock_lock);
			*passive_sock_lock = true;

			if (is_pic == false)
			{
				Send(recieverSocket, message.c_str(), message.size());
				int check = select(1, &sockHelp, NULL, NULL, &tmv);
				if (check > 0)
				{
					recv(clientSock, recv_buff, 6, 0);
					try
					{
						if (atoi(recv_buff) != stoi(to_string(SEND_PROFILE_LINK_TO_CLIENT_RES) + "0"))
							needToAdd = true;
					}
					catch (...)
					{
						needToAdd = true;
					}
				}
			}
			else
			{
				//First Message
				message = message1 + padd_zeroes(to_string(time_now() - stoll(sending_time_from_client)), 13) + message2 + padd_zeroes(to_string(size), 8) + padd_zeroes(to_string(BUFF_SIZE), 7);
				Send(recieverSocket, message.c_str(), message.size());
				int check = select(1, &sockHelp, NULL, NULL, &tmv);
				if (check > 0)
				{
					recv(recieverSocket, recv_buff, 6, 0);
					try
					{
						if (atoi(recv_buff) != stoi(to_string(SEND_PROFILE_LINK_TO_CLIENT_RES) + "0"))
							needToAdd = true;
					}
					catch (...)
					{
						needToAdd = true;
					}
				}
				while (totalReadChars < size && !needToAdd)
				{
					//emptyString(final_msg, 6, BUFF_SIZE - 1);
					//emptyString(buff, 0, BUFF_SIZE - 1);
					if (totalReadChars + (BUFF_SIZE - 13) < size)
						currChars = BUFF_SIZE - 13;
					else
						currChars = size - totalReadChars;
					totalReadChars += currChars;

					profile_pic->read(buff, currChars);
					for (i = 6; i < 6 + currChars; i++)
						final_msg[i] = buff[i - 6];

					Send(recieverSocket, final_msg, 6 + currChars);
					int check = select(1, &sockHelp, NULL, NULL, &tmv);
					if (check > 0)
					{
						recv(recieverSocket, recv_buff, 6, 0);
						try
						{
							if (atoi(recv_buff) != stoi(to_string(SEND_PROFILE_LINK_TO_CLIENT_RES) + "0"))
								needToAdd = true;
						}
						catch (...)
						{
							needToAdd = true;
						}
					}
					else
						needToAdd = true;
				}				
			}
			*passive_sock_lock = false;
			*/
		}
		index += 2 + name_len;
	}
	if (is_pic)
		delete profile_pic;
	//user->_mainPassiveSocketLocked = false;

}


void ServerClass::sendProfile(User* user, string data)
{
	while (connectedUsersLocked);
	connectedUsersLocked = true;
	SOCKET recieverSocket = user->getPassiveSocket();
	string reciever_id = user->getID();
	string reciever_name = user->getName();
	connectedUsersLocked = false;
	timeval tmv;
	tmv.tv_sec = 1;
	tmv.tv_usec = 1000000;
	fd_set sockHelp;
	sockHelp.fd_array[0] = recieverSocket;
	while (user->_passiveSocketLocked);
	user->_passiveSocketLocked = true;
	//canceling the KEEP_ALIVE:
	string KP_status_msg = to_string(KEEP_ALIVE_SERVER) + "1";
	Send(recieverSocket, KP_status_msg.c_str(), KP_status_msg.size());
	char recv_buff[6];
	int pic_link_len;
	string pic_link;
	int sender_name_len, name_len, pic_size;
	string sender_name, sending_time_from_client, time_since_sending, name, senderID;
	try
	{
		sender_name_len = stoi(data.substr(6, 2));
		sender_name = data.substr(6 + 2, sender_name_len);
		sending_time_from_client = data.substr(6 + 2 + sender_name_len, 13);
		time_since_sending = to_string(time_now() - stoll(sending_time_from_client));
		name_len = stoi(data.substr(6 + 2 + sender_name_len + 13, 2)); //the name of the person whose profile is sent.
		name = data.substr(6 + 2 + sender_name_len + 13 + 2, name_len);
		pic_size = stoi(data.substr(6 + 2 + sender_name_len + 13 + 2 + name_len, 8));
		senderID = data.substr(6 + 2 + sender_name_len + 13 + 2 + name_len + 8 + 7, 8);
	}
	catch (...)
	{
		cout << "problem with the data in ServerClass::sendProfile" << endl;
		return;
	}
	bool needToAdd = false;
	if (pic_size == 0)
	{
		data.replace(6 + 2 + sender_name_len, 13, time_since_sending);
		sockHelp.fd_array[0] = recieverSocket;
		int size = data.size();
		Send(recieverSocket, data.substr(0, size - 8).c_str(), size - 8);
		for (int i = 0; true; i++)
		{
			sockHelp.fd_count = 1;
			int check = select(1, &sockHelp, NULL, NULL, &tmv);
			if (check > 0)
			{
				recv(recieverSocket, recv_buff, 5, 0);
				try
				{
					if (atoi(recv_buff) == stoi(to_string(KEEP_ALIVE)) && i < 2)
					{
						cout << "got keep alive" << endl;
					}
					else
					{
						sockHelp.fd_count = 1;
						int check = select(1, &sockHelp, NULL, NULL, &tmv);
						if (check > 0)
							recv(recieverSocket, recv_buff + 5, 1, 0);
						else
						{
							needToAdd = true;
							break;
						}
						if (atoi(recv_buff) == stoi(to_string(SEND_PROFILE_LINK_TO_CLIENT_RES) + "0"))
						{
							cout << "good" << endl;
							break;
						}
						else
						{
							needToAdd = true;
							break;
						}
					}
				}
				catch (...)
				{
					needToAdd = true;
					break;
				}
			}
			else
			{
				needToAdd = true;
				break;
			}
		}
	}

	else
	{
		int totalReadChars = 0, currChars, i;
		char final_msg[BUFF_SIZE];
		char buff[BUFF_SIZE];
		char recv_buff[7] = "";
		for (i = 0; i < 5; i++)
			final_msg[i] = to_string(SEND_PROFILE_LINK_TO_CLIENT)[i];
		final_msg[5] = '1';
		ifstream profile_pic;
		try
		{
			pic_link_len = stoi(data.substr(6 + 2 + sender_name_len + 13 + 2 + name_len + 8 + 7 + 8, 2));
			pic_link = data.substr(6 + 2 + sender_name_len + 13 + 2 + name_len + 8 + 7 + 8 + 2, pic_link_len);
			profile_pic.open(pic_link, ios::in | ios::binary);
			data.replace(6 + 2 + sender_name_len, 13, time_since_sending);
		}
		catch (...)
		{
			cout << "problem with the data in ServerClass::sendProfile" << endl;
			return;
		}
		sockHelp.fd_array[0] = recieverSocket;
		int size = data.size();
		Send(recieverSocket, data.substr(0, size - pic_link_len - 2 - 8).c_str(), size - pic_link_len - 2 - 8);
		for (int i = 0; true; i++)
		{
			sockHelp.fd_count = 1;
			int check = select(1, &sockHelp, NULL, NULL, &tmv);
			if (check > 0)
			{
				recv(recieverSocket, recv_buff, 5, 0);
				try
				{
					if (atoi(recv_buff) == stoi(to_string(KEEP_ALIVE)) && i < 2)
					{
						cout << "got keep alive" << endl;
					}
					else
					{
						sockHelp.fd_count = 1;
						int check = select(1, &sockHelp, NULL, NULL, &tmv);
						if (check > 0)
							recv(recieverSocket, recv_buff + 5, 1, 0);
						else
						{
							needToAdd = true;
							break;
						}
						if (atoi(recv_buff) == stoi(to_string(SEND_PROFILE_LINK_TO_CLIENT_RES) + "0"))
						{
							cout << "good" << endl;
							break;
						}
						else
						{
							needToAdd = true;
							break;
						}
					}
				}
				catch (...)
				{
					needToAdd = true;
					break;
				}
			}
			else
			{
				needToAdd = true;
				break;
			}
		}


		while (totalReadChars < pic_size)
		{
			//emptyString(final_msg, 6, BUFF_SIZE - 1);
			//emptyString(buff, 0, BUFF_SIZE - 1);
			sockHelp.fd_count = 1;
			if (totalReadChars + (BUFF_SIZE - 13) < pic_size)
				currChars = BUFF_SIZE - 13;
			else
				currChars = pic_size - totalReadChars;
			totalReadChars += currChars;

			profile_pic.read(buff, currChars);
			for (i = 6; i < 6 + currChars; i++)
				final_msg[i] = buff[i - 6];

			Send(recieverSocket, final_msg, 6 + currChars);

			for (i = 0; true; i++) //this loop is responsible for checking if the client recieved the header message.
			{
				sockHelp.fd_count = 1;
				int check = select(1, &sockHelp, NULL, NULL, &tmv);
				if (check > 0)
				{
					emptyString(recv_buff, 0, 5);
					recv(recieverSocket, recv_buff, 5, 0);
					try
					{
						if (atoi(recv_buff) == stoi(to_string(KEEP_ALIVE)) && i < 2)
						{
							cout << "got keep alive" << endl;
						}
						else
						{
							sockHelp.fd_count = 1;
							int check = select(1, &sockHelp, NULL, NULL, &tmv);
							if (check > 0)
								recv(recieverSocket, recv_buff + 5, 1, 0);
							else
							{
								needToAdd = true;
								break;
							}
							if (atoi(recv_buff) == stoi(to_string(SEND_PIC_TO_RECIEVER_RES) + "0"))
							{
								cout << "good" << endl;
								break;
							}
							else
							{
								needToAdd = true;
								break;
							}
						}
					}
					catch (...)
					{
						needToAdd = true;
						break;
					}
				}
				else
				{
					needToAdd = true;
					break;
				}
			}
		}
	}
	
	if (needToAdd)
	{

		pair<string, string> p;
		p.first = reciever_name;
		int size = data.size();
		p.second = data.substr();
		if (pic_size != 0)
			p.second += padd_zeroes(to_string(pic_link_len), 2) + pic_link;
		profile_links.insert(p);
	}
	else
		_db.insertNewMessage(senderID, "post_link", reciever_id, senderID, "", sending_time_from_client, "reciever");
	KP_status_msg[5] = '0';
	Send(recieverSocket, KP_status_msg.c_str(), KP_status_msg.size());
	user->_passiveSocketLocked = false;
}


//handles a request to send an image.
void ServerClass::handleSendPic(User* user, string data)
{
	SOCKET clientSock;
	int numOfConnectedUsers, reciever_name_len, currChars, check, image_size, bytesRead = 0, part_size, i, totalReadChars = 0;
	string username, userID, reciever_name, recieverID, sending_time_from_client;
	fd_set sockHelp;
	timeval tmv;
	char buff[BUFF_SIZE], recv_buff[6];
	try
	{
		while (connectedUsersLocked);
		connectedUsersLocked = true;
		clientSock = user->getPassiveSocket();
		numOfConnectedUsers = _connectedUsers.size();
		username = user->getName();
		userID = user->getID();
		connectedUsersLocked = false;
		tmv.tv_sec = 3;
		tmv.tv_usec = 3000000;
		reciever_name_len = stoi(data.substr(0, 2));
		reciever_name = data.substr(2, reciever_name_len);
		recieverID = _db.getIDByName(reciever_name);
		if (recieverID == "0")
		{
			//user->_mainPassiveSocketLocked = false;
			return;
		}
		if (_db.checkIfBlocked(userID, recieverID))
		{
			//user->_mainPassiveSocketLocked = false;
			return;
		}
		sending_time_from_client = padd_zeroes(to_string(time_now() - stoll(data.substr(2 + reciever_name_len, 13))), 13);
		image_size = stoi(data.substr(2 + reciever_name_len + 13, 8));
	}
	catch (...)
	{
		//user->_mainPassiveSocketLocked = false;
		return;
	}

	if (image_size <= 0)
	{
		//user->_mainPassiveSocketLocked = false;
		return;
	}
	Send(clientSock, (to_string(PIC_FROM_SENDER_RES) + "0").c_str(), 6);
	SOCKET recieverSocket;
	long long time_in_secs = stoll(sending_time_from_client) / 1000;
	time_t help_t = time_in_secs;
	//string date = ctime(&help_t);
	struct tm *help_tm = localtime(&time_in_secs);
	//string date = asctime(localtime(&help_t));
	string date = padd_zeroes(to_string(help_tm->tm_hour), 2) + ";" + padd_zeroes(to_string(help_tm->tm_min), 2) + ";" + padd_zeroes(to_string(help_tm->tm_sec), 2) + " " + padd_zeroes(to_string(help_tm->tm_mday), 2) + "-" + padd_zeroes(to_string(help_tm->tm_mon), 2) + "-" + padd_zeroes(to_string(help_tm->tm_year + 1900), 2);
	string pic_path = string(PICS_LINK) + "images_from_chats\\" + "From " + userID + " To " + recieverID + " " + date + ".jpg";
	//string pic_path = string(PICS_LINK) + "images_from_chats\\" + "test.jpg";

	fstream image;
	image.open(pic_path.c_str(), ios::out);
	image.close();
	image.open(pic_path.c_str(), ios::in | ios::out | ios::binary | ios::trunc);
	bool *passiveSocketLocked = NULL, *mainPassiveSocketKilled = NULL;
	if (!image.is_open()) //cleaning the socket.
	{
		while (bytesRead < image_size)
		{
			if (bytesRead == 0)
			{
				bigDataRecv(clientSock, buff, BUFF_SIZE);
				part_size = BUFF_SIZE - 5;
			}
			else if (bytesRead + BUFF_SIZE <= image_size)
			{
				bigDataRecv(clientSock, buff, BUFF_SIZE);
				part_size = BUFF_SIZE;
			}
			else
			{
				part_size = image_size - bytesRead;
				bigDataRecv(clientSock, buff, part_size);
			}
			bytesRead += part_size;
		}
		//user->_mainPassiveSocketLocked = false;
		return;

	}

	else //reading the data of the image from the socket.
	{
		while (bytesRead < image_size)
		{
			if (bytesRead == 0)
			{
				if (!bigDataRecv(clientSock, buff, BUFF_SIZE))
					return;
				part_size = BUFF_SIZE - 5;
				image.write(buff + 5, part_size);

			}
			else if (bytesRead + BUFF_SIZE <= image_size)
			{
				if (!bigDataRecv(clientSock, buff, BUFF_SIZE))
					return;
				part_size = BUFF_SIZE;
				image.write(buff, part_size);
			}
			else
			{
				part_size = image_size - bytesRead;
				if (!bigDataRecv(clientSock, buff, part_size))
					return;
				image.write(buff, part_size);
			}
			bytesRead += part_size;
		}
	}

	//Finding the reciever's passive socket.
	bool userConnected = false;
	while (connectedUsersLocked);
	connectedUsersLocked = true;
	for (i = 0; i < numOfConnectedUsers; i++)
	{
		if (_connectedUsers[i]->getName() == reciever_name)
		{
			recieverSocket = _connectedUsers[i]->getPassiveSocket();
			passiveSocketLocked = &_connectedUsers[i]->_passiveSocketLocked;
			mainPassiveSocketKilled = &_connectedUsers[i]->_mainPassiveSocketKilled;
			userConnected = true;
			break;
		}
	}
	connectedUsersLocked = false;
	//image.seekg(0, image.beg);
	image.close();
	bool needToAdd = false;
	
	if (!userConnected || needToAdd)
	{
		pair<string, string> p;
		p.first = reciever_name;
		p.second = padd_zeroes(to_string(username.length()), 2) + username + sending_time_from_client + padd_zeroes(to_string(pic_path.length()), 3) + pic_path; //may need to add pic_path_length.
		standby_pics_messages.insert(p);
	}
	
	_db.insertNewMessage("", "pic", recieverID, userID, pic_path, sending_time_from_client, "sender");
	
	if (userConnected)
	{
		string header = to_string(SEND_PIC_TO_RECIEVER) + "0" + padd_zeroes(to_string(username.length()), 2) + username + sending_time_from_client + padd_zeroes(to_string(image_size), 8) + padd_zeroes(to_string(BUFF_SIZE), 7);
		string buffer = to_string(NTF_IMAGE) + padd_zeroes(to_string(header.length()), 3) + header + padd_zeroes(userID, 8) + pic_path;
		_connectedUsers[i]->setUserBuffer(buffer);

		/*
		bool gotMessage = false, timeout = false;
		char final_msg[BUFF_SIZE];
		for (i = 0; i < 5; i++)
		final_msg[i] = to_string(SEND_PIC_TO_RECIEVER)[i];
		final_msg[5] = '1';

		while (*passiveSocketLocked);
		*passiveSocketLocked = true;
		Send(recieverSocket, header.c_str(), header.size());

		sockHelp.fd_count = 1;
		sockHelp.fd_array[0] = recieverSocket;
		check = select(1, &sockHelp, NULL, NULL, &tmv);
		if (check > 0)
		{
		try
		{
		recv(recieverSocket, recv_buff , 6, 0);

		if (atoi(recv_buff) != stoi(to_string(SEND_PIC_TO_RECIEVER_RES) + "0"))
		{
		needToAdd = true;
		}
		}
		catch (...)
		{
		needToAdd = true;
		}
		}
		else
		{
		needToAdd = true;
		}

		if (!needToAdd)
		{
		while (totalReadChars < image_size && !needToAdd)
		{
		if (totalReadChars + (BUFF_SIZE - 13) < image_size)
		currChars = BUFF_SIZE - 13;
		else
		currChars = image_size - totalReadChars;
		totalReadChars += currChars;

		image.read(buff, currChars);
		for (i = 6; i < 6 + currChars; i++)
		final_msg[i] = buff[i - 6];

		if (Send(recieverSocket, final_msg, 6 + currChars) == false)
		{
		needToAdd = true;
		cout << "ServerClass::handleSendPic - Send failed" << endl;
		break;
		}

		sockHelp.fd_count = 1;
		sockHelp.fd_array[0] = recieverSocket;
		check = select(1, &sockHelp, NULL, NULL, &tmv);
		if (check > 0)
		{
		recv(recieverSocket, recv_buff, 6, 0);
		if (atoi(recv_buff) != stoi(to_string(SEND_PIC_TO_RECIEVER_RES) + "0"))
		needToAdd = true;
		}
		else
		{
		needToAdd = true;
		}
		}
		}
		*passiveSocketLocked = false;
		if (!needToAdd)
		_db.insertNewMessage("", "pic", recieverID, userID, pic_path, sending_time_from_client, "both");
		*/
	}
	

}

void ServerClass::sendPic(User* user, string data)
{
	while (connectedUsersLocked);
	connectedUsersLocked = true;
	SOCKET recieverSocket = user->getPassiveSocket();
	string recieverID = user->getID();
	string recieverName = user->getName();
	connectedUsersLocked = false;
	int headerSize = stoi(data.substr(0, 3));
	string header = data.substr(3, headerSize);
	string senderID = data.substr(3 + headerSize, 8);
	string pic_path = data.substr(3 + headerSize + 8);
	int name_len = stoi(header.substr(6, 2));
	string sending_time_from_client = header.substr(6 + 2 + name_len, 13);
	header.replace(6 + 2 + name_len, 13, padd_zeroes(to_string(time_now() - stoll(sending_time_from_client)), 13));

	bool needToAdd = false;
	timeval tmv;
	tmv.tv_sec = 1;
	tmv.tv_usec = 1000000;
	fd_set sockHelp;
	int i, check;
	char final_msg[BUFF_SIZE], recv_buff[6];
	for (i = 0; i < 5; i++)
		final_msg[i] = to_string(SEND_PIC_TO_RECIEVER)[i];
	final_msg[5] = '1';

	while (user->_passiveSocketLocked);
	user->_passiveSocketLocked;
	string KP_status_msg = to_string(KEEP_ALIVE_SERVER) + "1";
	Send(recieverSocket, KP_status_msg.c_str(), KP_status_msg.size());
	sockHelp.fd_array[0] = recieverSocket;
	sockHelp.fd_count = 1;
	char buff[BUFF_SIZE];
	check = select(1, &sockHelp, NULL, NULL, &tmv);
	if (check > 0)
		recv(recieverSocket, buff, BUFF_SIZE, 0);
	Send(recieverSocket, header.c_str(), header.size());
	sockHelp.fd_array[0] = recieverSocket;
	for (i = 0; true; i++) //this loop is responsible for checking if the client recieved the header message.
	{
		sockHelp.fd_count = 1;
		int check = select(1, &sockHelp, NULL, NULL, &tmv);
		if (check > 0)
		{
			recv(recieverSocket, recv_buff, 5, 0);
			try
			{
				if (atoi(recv_buff) == stoi(to_string(KEEP_ALIVE)) && i < 2)
				{
					cout << "got keep alive" << endl;
				}
				else
				{
					sockHelp.fd_count = 1;
					int check = select(1, &sockHelp, NULL, NULL, &tmv);
					if (check > 0)
						recv(recieverSocket, recv_buff + 5, 1, 0);
					else
					{
						needToAdd = true;
						break;
					}
					if (atoi(recv_buff) == stoi(to_string(SEND_PIC_TO_RECIEVER_RES) + "0"))
					{
						cout << "good" << endl;
						break;
					}
					else
					{
						needToAdd = true;
						break;
					}
				}
			}
			catch (...)
			{
				needToAdd = true;
				break;
			}
		}
		else
		{
			needToAdd = true;
			break;
		}
	}

	if (!needToAdd)
	{
		ifstream image;
		image.open(pic_path.c_str(), ios::in | ios::binary);
		image.seekg(0, image.end);
		int image_size = image.tellg();
		image.seekg(0, image.beg);
		int totalReadChars = 0, currChars;
		char buff[BUFF_SIZE];
		while (totalReadChars < image_size && !needToAdd)
		{
			if (totalReadChars + (BUFF_SIZE - 13) < image_size)
				currChars = BUFF_SIZE - 13;
			else
				currChars = image_size - totalReadChars;
			totalReadChars += currChars;

			image.read(buff, currChars);
			for (i = 6; i < 6 + currChars; i++)
				final_msg[i] = buff[i - 6];

			if (Send(recieverSocket, final_msg, 6 + currChars) == false)
			{
				needToAdd = true;
				cout << "ServerClass::handleSendPic - Send failed" << endl;
				break;
			}

			for (i = 0; true; i++) //this loop is responsible for checking if the client recieved the header message.
			{
				sockHelp.fd_count = 1;
				int check = select(1, &sockHelp, NULL, NULL, &tmv);
				if (check > 0)
				{
					emptyString(recv_buff, 0, 5);
					recv(recieverSocket, recv_buff, 5, 0);
					try
					{
						if (atoi(recv_buff) == stoi(to_string(KEEP_ALIVE)) && i < 2)
						{
							cout << "got keep alive" << endl;
						}
						else
						{
							sockHelp.fd_count = 1;
							int check = select(1, &sockHelp, NULL, NULL, &tmv);
							if (check > 0)
								recv(recieverSocket, recv_buff + 5, 1, 0);
							else
							{
								needToAdd = true;
								break;
							}
							if (atoi(recv_buff) == stoi(to_string(SEND_PIC_TO_RECIEVER_RES) + "0"))
							{
								cout << "good" << endl;
								break;
							}
							else
							{
								needToAdd = true;
								break;
							}
						}
					}
					catch (...)
					{
						needToAdd = true;
						break;
					}
				}
				else
				{
					needToAdd = true;
					break;
				}
			}
		}
	}

	if (!needToAdd)
		_db.insertNewMessage("", "pic", recieverID, senderID, pic_path, sending_time_from_client, "reciever");
	else
	{
		pair<string, string> p;
		p.first = recieverName;
		p.second = padd_zeroes(to_string(header.substr(6 + 2, name_len).length()), 2) + header.substr(6 + 2, name_len) + sending_time_from_client + padd_zeroes(to_string(pic_path.length()), 3) + pic_path; //may need to add pic_path_length.
		standby_pics_messages.insert(p);
	}
	KP_status_msg[5] = '0';
	Send(recieverSocket, KP_status_msg.c_str(), KP_status_msg.size());
	user->_passiveSocketLocked = false;
}

//handles a saved posts request.
void ServerClass::handleSavedPosts(User* user, string data)
{
	while (connectedUsersLocked);
	connectedUsersLocked = true;
	SOCKET clientSock = user->getSocket();
	connectedUsersLocked = false;
	string msg = to_string(SAVED_POSTS_REQ_RES);
	string type;
	if (data[0] == '0')
		type = "my posts";
	else if (data[0] == '1')
		type = "commented posts";
	else if (data[0] == '2')
		type = "posts to read later";
	else
	{
		cout << "ServerClass::handleSavedPosts - not in protocol" << endl;
		return;
	}
	try
	{
		string num_of_current_posts = data.substr(1, 4);
		string last_post_id = data.substr(1 + 4, 8);
		msg += _db.getSavedPosts(type, num_of_current_posts, last_post_id, user->getID());
	}
	catch (...)
	{
		msg += "1";
		Send(clientSock, msg.c_str(), msg.size());
		return;
	}
	Send(clientSock, msg.c_str(), msg.size());
}

//handles an all saved posts delete request.
void ServerClass::handleDeleteAllSavedPosts(User* user, string data)
{
	while (connectedUsersLocked);
	connectedUsersLocked = true;
	SOCKET clientSock = user->getSocket();
	connectedUsersLocked = false;
	string msg = to_string(DELETE_ALL_SAVED_POSTS_RES);
	if (_db.deleteAllPosts(user->getID()))
		msg += "0";
	else
		msg += "1";
	Send(clientSock, msg.c_str(), msg.size());
}

//handles a saved posts delete request.
void ServerClass::handleDeleteSavedPosts(User* user, string data)
{
	while (connectedUsersLocked);
	connectedUsersLocked = true;
	SOCKET clientSock = user->getSocket();
	connectedUsersLocked = false;
	string msg = to_string(DELETE_SAVED_POSTS_RES);
	int numOfPosts, i, index;
	vector<string> IDs;
	try
	{
		numOfPosts = stoi(data.substr(0, 3));
		for (i = 0, index = 3; i < numOfPosts; i++, index += 8)
		{
			IDs.push_back(data.substr(index, 8));
		}
	}
	catch (...)
	{
		return;
	}
	if (_db.deletePosts(user->getID(), numOfPosts, IDs))
		msg += "0";
	else
		msg += "1";
	Send(clientSock, msg.c_str(), msg.size());
}

//handles an add to read later list request.
void ServerClass::handleAddReadLaterPost(User* user, string data)
{
	while (connectedUsersLocked);
	connectedUsersLocked = true;
	SOCKET clientSock = user->getSocket();
	connectedUsersLocked = false;
	string msg = to_string(ADD_POST_TO_READ_LATER_RES);
	msg += "0";
	try
	{
		string postID = data.substr(0, 8);
		if (_db.addPostToReadLater(user->getID(), postID))
			msg += "0";
		else
			msg += "1";
	}
	catch (...)
	{
		msg += "1";
		Send(clientSock, msg.c_str(), msg.size());
		return;
	}
	Send(clientSock, msg.c_str(), msg.size());
}

//handles a posts search request.
void ServerClass::handlePostSearch(User* user, string data)
{
	while (connectedUsersLocked);
	connectedUsersLocked = true;
	SOCKET clientSock = user->getSocket();
	connectedUsersLocked = false;
	string msg = to_string(POST_SEARCH_REQ_RES);
	int title_len, time_range_ID, numOfCurrPosts;
	string title, categoryID;
	long long timeRange;
	try
	{
		title_len = stoi(data.substr(0, 2));
		title = data.substr(2, title_len);
		categoryID = data.substr(2 + title_len, 2);
		time_range_ID = stoi(data.substr(2 + title_len + 2, 1));
		timeRange = timeRangeList[time_range_ID];
		numOfCurrPosts = stoi(data.substr(2 + title_len + 2 + 1, 4));
		msg += _db.postsSearch(title, categoryID, timeRange, numOfCurrPosts);
	}
	catch (...)
	{
		msg += "1";
		Send(clientSock, msg.c_str(), msg.size());
		return;
	}
	Send(clientSock, msg.c_str(), msg.size());
}

//handles a status update request.
void ServerClass::handleStatusUpdate(User* user, string data)
{
	while (connectedUsersLocked);
	connectedUsersLocked = true;
	SOCKET clientSock = user->getSocket();
	connectedUsersLocked = false;
	string msg = to_string(STATUS_UPDATE_REQ_RES);
	int status_size;
	string status;
	try
	{
		status_size = stoi(data.substr(0, 3));
		status = data.substr(3, status_size);
		if (_db.updateStatus(user->getID(), status))
			msg += "0";
		else
			msg += "1";
	}
	catch (...)
	{
		msg += "1";
		Send(clientSock, msg.c_str(), msg.size());
		return;
	}
	Send(clientSock, msg.c_str(), msg.size());
}

//handles a presonal info request.
void ServerClass::handlePersonalInfoUpdate(User* user, string data)
{
	while (connectedUsersLocked);
	connectedUsersLocked = true;
	SOCKET clientSock = user->getSocket();
	connectedUsersLocked = false;
	string msg = to_string(PERSONAL_INFO_UPDATE_REQ_RES);
	try
	{
		string birthDay = data.substr(0, 2);
		string birthMonth = data.substr(2, 2);
		string birthYear = data.substr(2 + 2, 4);
		string hobbies = data.substr(2 + 2 + 4, 2);
		string occupation = data.substr(2 + 2 + 4 + 2, 2);
		string interests = data.substr(2 + 2 + 4 + 2 + 2, 2);
		if (_db.updatePersonalInfo(user->getID(), birthDay, birthMonth, birthYear, hobbies, occupation, interests))
			msg += "0";
		else
			msg += "1";
	}
	catch (...)
	{
		msg += "1";
		Send(clientSock, msg.c_str(), msg.size());
		return;
	}
	Send(clientSock, msg.c_str(), msg.size());
}

//handles a profile picture update request.
void ServerClass::handleProfilePicUpdate(User* user, string data)
{
	while (connectedUsersLocked);
	connectedUsersLocked = true;
	SOCKET clientSock = user->getSocket();
	string userID = user->getID();
	connectedUsersLocked = false;
	string msg = to_string(PROFILE_PICTURE_UPDATE_REQ_RES);
	int length_of_file, i, num_of_rounds, part_size;
	try
	{
		length_of_file = stoi(data.substr(0, 8));
		num_of_rounds = length_of_file / (BUFF_SIZE - 5);
	}
	catch (...)
	{
		msg += "1";
		Send(clientSock, msg.c_str(), msg.size());
		return;
	}
	if (length_of_file % (BUFF_SIZE - 5) != 0)
		num_of_rounds++;
	
	char msg_c[BUFF_SIZE];
	string fileName = string(PICS_LINK) + "profile_images\\profile_pic_of_" + userID + ".jpg";
	ofstream profile_pic;
	profile_pic.open(fileName.c_str(), ios::binary);
	if (profile_pic.is_open() == false)
	{
		cout << "ServerClass::handleProfilePicUpdate - couldn't open" << endl;
		Send(clientSock, (to_string(PROFILE_PICTURE_UPDATE_REQ_RES) + "1").c_str(), 6);
		/*for (i = 0; i < num_of_rounds; i++)
		{
			if (i != num_of_rounds - 1)
			{
				bigDataRecv(clientSock, msg_c, BUFF_SIZE);
			}
			else
			{
				part_size = length_of_file - i*(BUFF_SIZE - 5);
				bigDataRecv(clientSock, msg_c, part_size);
			}
		}*/
		return;
	}
	Send(clientSock, (to_string(PROFILE_PICTURE_UPDATE_REQ_RES) + "0" + padd_zeroes(to_string(BUFF_SIZE), 7)).c_str(), 13);
	
	int totalReadChars = 0;
	for (i = 0; i < num_of_rounds; i++)
	{
		if (i != num_of_rounds - 1)
		{
			bigDataRecv(clientSock, msg_c, BUFF_SIZE);
			part_size = BUFF_SIZE - 5;
			profile_pic.write(msg_c+5, part_size);
			totalReadChars += part_size;
			Send(clientSock, (to_string(PROFILE_PICTURE_UPDATE_REQ_RES) + "0").c_str(), 6);
		}
		else
		{
			part_size = length_of_file - i*(BUFF_SIZE - 5);
			bigDataRecv(clientSock, msg_c, 5 + part_size);
			profile_pic.write(msg_c+5, part_size);
			totalReadChars += part_size;

		}
	}
		
	profile_pic.close();
	msg += "0";
	Send(clientSock, msg.c_str(), msg.size());
}

//handles a profile info request.
void ServerClass::handleProfileReq(User* user, string data)
{
	while (connectedUsersLocked);
	connectedUsersLocked = true;
	SOCKET clientSock = user->getSocket();
	connectedUsersLocked = false;
	string msg = to_string(PROFILE_REQUEST_RES);
	try
	{
		string usernameLen = data.substr(0, 2);
		string username = data.substr(2, stoi(usernameLen));
		string user_info = _db.getProfile(username, usernameLen);
		msg += user_info;
	}
	catch (...)
	{
		msg += "1";
		Send(clientSock, msg.c_str(), msg.size());
		return;
	}
	Send(clientSock, msg.c_str(), msg.size());
}

//handles a profile picture request.
void ServerClass::handleProfilePicReq(User* user, string data)
{
	while (connectedUsersLocked);
	connectedUsersLocked = true;
	SOCKET clientSock = user->getSocket();
	connectedUsersLocked = false;
	timeval tmv;
	tmv.tv_sec = 3;
	tmv.tv_usec = 3000000;
	string usernameLen, username;
	try
	{
		usernameLen = data.substr(0, 2);
		username = data.substr(2, stoi(usernameLen));
	}
	catch (...)
	{
		Send(clientSock, (to_string(PROFILE_PIC_REQUEST_RES) + "1").c_str(), 6);
		return;
	}
	ifstream* profile_pic = _db.getProfilePic(username, usernameLen);
	if (profile_pic == NULL)
	{
		Send(clientSock, (to_string(PROFILE_PIC_REQUEST_RES) + "1").c_str(), 6);
	}
	else
	{
		profile_pic->seekg(0, profile_pic->end);
		int size = profile_pic->tellg();
		profile_pic->seekg(0, profile_pic->beg);
		//char file_info[BUFF_SIZE];
		int totalReadChars = 0, currChars, i;
		Send(clientSock, (to_string(PROFILE_PIC_REQUEST_RES) + "0" + padd_zeroes(to_string(size), 8) + padd_zeroes(to_string(BUFF_SIZE), 7)).c_str(), 21);

		char buff[BUFF_SIZE];
		char final_msg[BUFF_SIZE];
		char recv_buff[6];
		for (i = 0; i < 5; i++)
			final_msg[i] = to_string(PROFILE_PIC_REQUEST_RES)[i];
		while (totalReadChars < size)
		{
			//emptyString(final_msg, 5, BUFF_SIZE-1);
			//emptyString(buff, 0, BUFF_SIZE-1);
			if (totalReadChars + (BUFF_SIZE - 12) < size)
				currChars = BUFF_SIZE - 12;
			else
				currChars = size - totalReadChars;
			totalReadChars += currChars;
			
			profile_pic->read(buff, currChars);
			for (i = 5; i < 5 + currChars; i++)
				final_msg[i] = buff[i - 5];

			if (Send(clientSock, final_msg, 5 + currChars) == false)
			{
				cout << "ServerClass::handleProfilePicReq - Send failed" << endl;
				break;
			}
			recv(clientSock, recv_buff, 6, 0);
			if (strcmp(recv_buff, (to_string(SEND_PROFILE_LINK_TO_CLIENT_RES) + "1").c_str()) == 0)
			{
				cout << "ServerClass::handleProfilePicReq - no good" << endl;
				break;
			}
		}
		profile_pic->close();
		delete profile_pic;
	}

}

//handles a log out request.
void ServerClass::handleLogOut(User* user)
{
	while (connectedUsersLocked);
	connectedUsersLocked = true;
	cout << "Logging out - " << user->getName() << endl;
	string msg = to_string(SEND_CONNECTION_CODE);
	user->setID("0");
	user->setName("L-O");
	string connection_code = rndStr();
	user->setPassiveConnectionCode(connection_code);
	msg += connection_code;
	SOCKET clientSock = user->getSocket();
	Send(clientSock, msg.c_str(), msg.size());
	clientDisconnection(user, JUST_PASSIVE);
	connectedUsersLocked = false;
}

//handles a user date update request.
void ServerClass::handleUserDataUpdateReq(User* user, string data)
{
	while (connectedUsersLocked);
	connectedUsersLocked = true;
	SOCKET clientSock = user->getSocket();
	string userID = user->getID();
	connectedUsersLocked = false;
	string msg = to_string(USER_DATA_UPDATE_REQ_RES);
	string type;
	if (data[0] == '0')
		type = "password";
	else if (data[0] == '1')
		type = "email";
	else
	{
		cout << "ServerClass::userDataUpdateReq - type not in protocol" << endl;
		return;
	}
	try
	{
		int oldData_len = stoi(data.substr(1, 2));
		string oldData = data.substr(1 + 2, oldData_len);
		int newData_len = stoi(data.substr(1 + 2 + oldData_len, 2));
		string newData = data.substr(1 + 2 + oldData_len + 2, newData_len);
		msg += _db.updatePersonalData(userID, oldData, newData, type);
	}
	catch (...)
	{
		msg += "1";
		Send(clientSock, msg.c_str(), msg.size());
		return;
	}
	Send(clientSock, msg.c_str(), msg.size());
}

//handles a background update request.
void ServerClass::handleBackgroundUpdateReq(User* user, string data)
{
	while (connectedUsersLocked);
	connectedUsersLocked = true;
	SOCKET clientSock = user->getSocket();
	string userID = user->getID();
	connectedUsersLocked = false;
	int length_of_file;
	string msg = to_string(BACKGROUND_UPDATE_REQ_RES);
	int i, num_of_rounds, part_size;
	try
	{
		length_of_file = stoi(data.substr(0, 8));
		num_of_rounds = length_of_file / (BUFF_SIZE - 5);
	}
	catch (...)
	{
		msg += "1";
		Send(clientSock, msg.c_str(), msg.size());
		return;
	}
	if (length_of_file % (BUFF_SIZE - 5) != 0)
		num_of_rounds++;
	char msg_c[BUFF_SIZE];
	string fileName = string(PICS_LINK) + "backgrounds\\background_of_" + userID + ".jpg";
	ofstream profile_pic;
	profile_pic.open(fileName.c_str(), ios::binary);
	if (profile_pic.is_open() == false)
	{
		cout << "ServerClass::handleBackgroundUpdateReq - couldn't open" << endl;
		Send(clientSock, (to_string(BACKGROUND_UPDATE_REQ_RES) + "1").c_str(), 6);
		for (i = 0; i < num_of_rounds; i++)
		{
			if (i != num_of_rounds - 1)
			{
				bigDataRecv(clientSock, msg_c, BUFF_SIZE);
			}
			else
			{
				part_size = length_of_file - (i - 1)*BUFF_SIZE - (BUFF_SIZE - 5);
				bigDataRecv(clientSock, msg_c, part_size);
			}
		}
		return;
	}
	Send(clientSock, (to_string(BACKGROUND_UPDATE_REQ_RES) + "0" + padd_zeroes(to_string(BUFF_SIZE), 7)).c_str(), 13);
	
	int totalReadChars = 0;
	for (i = 0; i < num_of_rounds; i++)
	{
		if (i != num_of_rounds - 1)
		{
			bigDataRecv(clientSock, msg_c, BUFF_SIZE);
			part_size = BUFF_SIZE - 5;
			profile_pic.write(msg_c + 5, part_size);
			totalReadChars += part_size;
			Send(clientSock, (to_string(BACKGROUND_UPDATE_REQ_RES) + "0").c_str(), 6);
		}
		else
		{
			part_size = length_of_file - i*(BUFF_SIZE - 5);
			bigDataRecv(clientSock, msg_c, 5 + part_size);
			profile_pic.write(msg_c + 5, part_size);
			totalReadChars += part_size;

		}
	}

	profile_pic.close();
	msg += "0";
	Send(clientSock, msg.c_str(), msg.size());

}

//handles a block request.
void ServerClass::handleBlockReq(User* user, string data)
{
	while (connectedUsersLocked);
	connectedUsersLocked = true;
	SOCKET clientSock = user->getSocket();
	string userID = user->getID();
	connectedUsersLocked = false;
	string msg = to_string(BLOCK_REQ_RES);
	try
	{
		string usernameLen = data.substr(0, 2);
		string username = data.substr(2, stoi(usernameLen));
		string retC = _db.blockOrUnblock(userID, username);
		if (retC != "2")
			msg += "0" + retC;
		else
			msg += "1";
	}
	catch (...)
	{
		msg += "1";
		Send(clientSock, msg.c_str(), msg.size());
		return;
	}
	Send(clientSock, msg.c_str(), msg.size());
}

//handles a user search request.
void ServerClass::handleUserSearch(User* user, string data)
{
	while (connectedUsersLocked);
	connectedUsersLocked = true;
	SOCKET clientSock = user->getSocket();
	connectedUsersLocked = false;
	string msg = to_string(USER_SEARCH_REQ_RES);
	try
	{
		int numOfCurrUsers = stoi(data.substr(0, 4));
		int requested_name_len = stoi(data.substr(4, 2));
		string requested_name = data.substr(4 + 2, requested_name_len);
		int lowestAge = stoi(data.substr(4 + 2 + requested_name_len, 2));
		int highestAge = stoi(data.substr(4 + 2 + requested_name_len + 2, 2));
		string occupation = data.substr(4 + 2 + requested_name_len + 2 + 2, 2);
		string hobbies = data.substr(4 + 2 + requested_name_len + 2 + 2 + 2, 2);
		string interests = data.substr(4 + 2 + requested_name_len + 2 + 2 + 2 + 2, 2);
		msg += _db.userSearch(numOfCurrUsers, requested_name, lowestAge, highestAge, occupation, hobbies, interests);
	}
	catch (...)
	{
		msg += "1";
		Send(clientSock, msg.c_str(), msg.size());
		return;
	}
	Send(clientSock, msg.c_str(), msg.size());
}

//-----------------------------------------------------------------------------
//Standby sending functions:

//checks if there are standby messages for a user whose name equals to username.
void ServerClass::checkForMessages(SOCKET clientSock, string username, string userID)
{
	int i, num_of_messages, count;
	unordered_multimap<string, string>::iterator it;
	vector<unordered_multimap<string, string>::iterator> delete_list;
	//pair<unordered_multimap<string, string>::iterator, unordered_multimap<string, string>::iterator> help = text_messages.equal_range(username);

	auto help = text_messages.equal_range(username);
	it = help.first;
	num_of_messages = text_messages.count(username);
	for (count = 0; count < num_of_messages; it++)
	{
		if (it->first == username)
		{
			if (sendStandbyTextMessages(clientSock, userID, it->second))
				delete_list.push_back(it);
			count++;
		}
	}
	for (i = 0; i < delete_list.size(); i++)
	{
		text_messages.erase(delete_list[i]);
	}
	delete_list.clear();

	help = post_links.equal_range(username);
	it = help.first;
	num_of_messages = post_links.count(username);
	for (count = 0; count < num_of_messages; it++)
	{
		if (it->first == username)
		{
			if (sendStandbyPostLinks(clientSock, userID, it->second))
				delete_list.push_back(it);
			count++;
		}
	}
	for (i = 0; i < delete_list.size(); i++)
	{
		post_links.erase(delete_list[i]);
	}
	delete_list.clear();

	help = profile_links.equal_range(username);
	it = help.first;
	num_of_messages = profile_links.count(username);
	for (count = 0; count < num_of_messages; it++)
	{
		if (it->first == username)
		{
			if (sendStandbyProfileLinks(clientSock, userID, it->second))
				delete_list.push_back(it);
			count++;
		}
	}
	for (i = 0; i < delete_list.size(); i++)
	{
		profile_links.erase(delete_list[i]);
	}
	delete_list.clear();

	help = standby_pics_messages.equal_range(username);
	it = help.first;
	num_of_messages = standby_pics_messages.count(username);
	for (count = 0; count < num_of_messages; it++)
	{
		if (it->first == username)
		{
			if (sendStandbyPics(clientSock, userID, it->second))
				delete_list.push_back(it);
			count++;
		}
	}
	for (i = 0; i < delete_list.size(); i++)
	{
		standby_pics_messages.erase(delete_list[i]);
	}
	delete_list.clear();
}

//sends standby text messages to a user whose ID is recieverID.
bool ServerClass::sendStandbyTextMessages(SOCKET& passiveClientSock, string recieverID, string data) //returns "true" in case of success, "false" in case of failure.
{
	timeval tmv;
	tmv.tv_sec = 3;
	tmv.tv_usec = 3000000;
	fd_set sockHelp;
	sockHelp.fd_count = 1;
	sockHelp.fd_array[0] = passiveClientSock;
	char recv_buff[6];
	bool stop = false;
	string msg = to_string(SEND_MESSAGE_TO_RECIEVER);
	string creation_time, sender_id;
	int sender_name_len;
	try
	{
		creation_time = data.substr(0, 13);
		sender_name_len = stoi(data.substr(13, 2));
		msg += data.substr(13, 2 + sender_name_len);
		msg += padd_zeroes(to_string(time_now() - stoll(creation_time)), 13);
		msg += data.substr(13 + 2 + sender_name_len);
	}
	catch (...)
	{
		cout << "problem with data in ServerClass::sendStandbyTextMessages" << endl;
		return false;
	}
	Send(passiveClientSock, msg.c_str(), msg.size());
	int check = select(1, &sockHelp, NULL, NULL, &tmv);
	if (check > 0)
	{
		recv(passiveClientSock, recv_buff, 6, 0);
		/*try
		{
			//if (atoi(recv_buff) != stoi(to_string(SEND_MESSAGE_TO_RECIEVER_RES) + "0"))
				//return false;
			
		}
		catch (...)
		{
			return false;
		}*/
	}
	/*string KP_status_msg = to_string(KEEP_ALIVE_SERVER) + "1";
	Send(passiveClientSock, KP_status_msg.c_str(), KP_status_msg.size());
	Send(passiveClientSock, msg.c_str(), msg.size());
	for (int i = 0; !stop; i++)
	{
		int check = select(1, &sockHelp, NULL, NULL, &tmv);
		if (check > 0)
		{
			//SEND_MESSAGE_TO_RECIEVER_RES
			emptyString(recv_buff, 0, 5);
			recv(passiveClientSock, recv_buff, 5, 0);
			try
			{
				if (atoi(recv_buff) == stoi(to_string(KEEP_ALIVE)) && i < 2)
				{
					cout << "got keep alive" << endl;
				}
				else
				{
					sockHelp.fd_count = 1;
					int check = select(1, &sockHelp, NULL, NULL, &tmv);
					if (check > 0)
						recv(passiveClientSock, recv_buff + 5, 1, 0);
					else
						stop = true;

					if (atoi(recv_buff) == stoi(to_string(SEND_MESSAGE_TO_RECIEVER_RES) + "0"))
					{
						cout << "good" << endl;
						break;
					}
					else
						stop = true;
				}
			}
			catch (...)
			{
				stop = true;
			}
		}
		else
			stop = true;
	}
	KP_status_msg[5] = '1';
	Send(passiveClientSock, KP_status_msg.c_str(), KP_status_msg.size());
	*/
	try
	{
		sender_id = _db.getIDByName(msg.substr(13 + 2, sender_name_len));
		_db.insertNewMessage(msg, "text", recieverID, sender_id, "", creation_time, "reciever");
	}
	catch (...)
	{
		return false;
	}
	
	return true;
}

//sends standby post links to a user whose ID is recieverID.
bool ServerClass::sendStandbyPostLinks(SOCKET& passiveClientSock, string recieverID, string data) //returns "true" in case of success, "false" in case of failure.
{
	timeval tmv;
	tmv.tv_sec = 3;
	tmv.tv_usec = 3000000;
	fd_set sockHelp;
	sockHelp.fd_count = 1;
	sockHelp.fd_array[0] = passiveClientSock;
	char recv_buff[6];
	int sender_name_len, help_size;
	bool stop = false;
	string sending_time, time_from_sending, time_since_post_creation;
	try
	{
		sender_name_len = stoi(data.substr(5, 2));
		sending_time = data.substr(5 + 2 + sender_name_len, 13);
		time_from_sending = padd_zeroes(to_string(time_now() - stoll(sending_time)), 13);
		data.replace(5 + 2 + sender_name_len, 13, time_from_sending);
		help_size = data.size();
		time_since_post_creation = padd_zeroes(to_string(time_now() - stoll(data.substr(help_size - 13, 13))), 13);
		//need to put post_creation_time and sending_time into data and send it to the reciever.
		data.replace(help_size - 13, 13, time_since_post_creation);
	}
	catch (...)
	{
		cout << "Problem with data in ServerClass::sendStandbyPostLinks" << endl;
		return false;
	}
	Send(passiveClientSock, data.c_str(), data.size());
	int check = select(1, &sockHelp, NULL, NULL, &tmv);
	if (check > 0)
	{
		recv(passiveClientSock, recv_buff, 6, 0);
		//if (atoi(recv_buff) != stoi(to_string(SEND_POST_LINK_TO_CLIENT_RES) + "0"))
			//return false;
	}
	/*string KP_status_msg = to_string(KEEP_ALIVE_SERVER) + "1";
	Send(passiveClientSock, KP_status_msg.c_str(), KP_status_msg.size());
	Send(passiveClientSock, data.c_str(), data.size());
	for (int i = 0; !stop; i++)
	{
		int check = select(1, &sockHelp, NULL, NULL, &tmv);
		if (check > 0)
		{
			//SEND_MESSAGE_TO_RECIEVER_RES
			emptyString(recv_buff, 0, 5);
			recv(passiveClientSock, recv_buff, 5, 0);
			try
			{
				if (atoi(recv_buff) == stoi(to_string(KEEP_ALIVE)) && i < 2)
				{
					cout << "got keep alive" << endl;
				}
				else
				{
					sockHelp.fd_count = 1;
					int check = select(1, &sockHelp, NULL, NULL, &tmv);
					if (check > 0)
						recv(passiveClientSock, recv_buff + 5, 1, 0);
					else
						stop = true;

					if (atoi(recv_buff) == stoi(to_string(SEND_POST_LINK_TO_CLIENT_RES) + "0"))
					{
						cout << "good" << endl;
						break;
					}
					else
						stop = true;
				}
			}
			catch (...)
			{
				stop = true;
			}
		}
		else
			stop = true;
	}
	KP_status_msg[5] = '1';
	Send(passiveClientSock, KP_status_msg.c_str(), KP_status_msg.size());*/
	try
	{
		string sender_id = _db.getIDByName(data.substr(13 + 2, sender_name_len));
		_db.insertNewMessage(data, "post_link", recieverID, sender_id, "", sending_time, "reciever");
	}
	catch (...)
	{}
	return true;
}

//sends standby profile links to a user whose ID is recieverID.
bool ServerClass::sendStandbyProfileLinks(SOCKET& passiveClientSock, string userID, string data) //returns "true" in case of success, "false" in case of failure.
{
	timeval tmv;
	tmv.tv_sec = 3;
	tmv.tv_usec = 3000000;
	fd_set sockHelp;
	sockHelp.fd_count = 1;
	sockHelp.fd_array[0] = passiveClientSock;
	char recv_buff[6];
	int sender_name_len, name_len, pic_size;
	bool stop = false;
	string sender_name, sending_time_from_client, time_since_sending, name;
	try
	{
		sender_name_len = stoi(data.substr(6, 2));
		sender_name = data.substr(6 + 2, sender_name_len);
		sending_time_from_client = data.substr(6 + 2 + sender_name_len, 13);
		time_since_sending = to_string(time_now() - stoll(sending_time_from_client));
		name_len = stoi(data.substr(6 + 2 + sender_name_len + 13, 2)); //the name of the person whose profile is sent.
		name = data.substr(6 + 2 + sender_name_len + 13 + 2, name_len);
		pic_size = stoi(data.substr(6 + 2 + sender_name_len + 13 + 2 + name_len, 8));
	}
	catch (...)
	{
		cout << "problem with the data in ServerClass::sendStandbyProfileLinks" << endl;
		return true;
	}
	string KP_status_msg = to_string(KEEP_ALIVE_SERVER) + "1";
	Send(passiveClientSock, KP_status_msg.c_str(), KP_status_msg.size());
	if (pic_size == 0)
	{
		data.replace(6 + 2 + sender_name_len, 13, time_since_sending);
		data += padd_zeroes(to_string(BUFF_SIZE), 7);
		Send(passiveClientSock, data.c_str(), data.size());
		int check = select(1, &sockHelp, NULL, NULL, &tmv);
		if (check > 0)
		{
			recv(passiveClientSock, recv_buff, 6, 0);
			try
			{
				if (atoi(recv_buff) != stoi(to_string(SEND_PROFILE_LINK_TO_CLIENT_RES) + "0"))
					stop = true;
			}
			catch (...)
			{
				stop = true;
			}
		}
	}
	else
	{
		int totalReadChars = 0, currChars, i;
		char final_msg[BUFF_SIZE];
		char buff[BUFF_SIZE];
		char recv_buff[7] = "";
		for (i = 0; i < 5; i++)
			final_msg[i] = to_string(SEND_PROFILE_LINK_TO_CLIENT)[i];
		final_msg[5] = '1';
		int pic_link_len;
		string pic_link;
		ifstream profile_pic;
		try
		{
			pic_link_len = stoi(data.substr(6 + 2 + sender_name_len + 13 + 2 + name_len + 8, 2));
			pic_link = data.substr(6 + 2 + sender_name_len + 13 + 2 + name_len + 8 + 2, pic_link_len);
			profile_pic.open(pic_link, ios::in | ios::binary);
			data.replace(6 + 2 + sender_name_len, 13, time_since_sending);
			data += padd_zeroes(to_string(BUFF_SIZE), 7);
		}
		catch (...)
		{
			cout << "problem with the data in ServerClass::sendStandbyProfileLinks" << endl;
			stop = true;
		}
		if (!stop)
		{

			Send(passiveClientSock, data.c_str(), data.size());
			int check = select(1, &sockHelp, NULL, NULL, &tmv);
			if (check > 0)
			{
				recv(passiveClientSock, recv_buff, 6, 0);
				try
				{
					if (atoi(recv_buff) != stoi(to_string(SEND_PROFILE_LINK_TO_CLIENT_RES) + "0"))
						stop = true;
				}
				catch (...)
				{
					stop = true;
				}
			}
			while (totalReadChars < pic_size && !stop)
			{
				//emptyString(final_msg, 6, BUFF_SIZE - 1);
				//emptyString(buff, 0, BUFF_SIZE - 1);
				if (totalReadChars + (BUFF_SIZE - 13) < pic_size)
					currChars = BUFF_SIZE - 13;
				else
					currChars = pic_size - totalReadChars;
				totalReadChars += currChars;

				profile_pic.read(buff, currChars);
				for (i = 6; i < 6 + currChars; i++)
					final_msg[i] = buff[i - 6];

				Send(passiveClientSock, final_msg, 6 + currChars);
				int check = select(1, &sockHelp, NULL, NULL, &tmv);
				if (check > 0)
				{
					recv(passiveClientSock, recv_buff, 6, 0);
					try
					{
						if (atoi(recv_buff) != stoi(to_string(SEND_PROFILE_LINK_TO_CLIENT_RES) + "0"))
							stop = true;
					}
					catch (...)
					{
						stop = true;
					}
				}
				else
					stop = true;
			}
		}
	}
	KP_status_msg[5] = '1';
	Send(passiveClientSock, KP_status_msg.c_str(), KP_status_msg.size());
	if (stop)
		return false;
	return true;
}

//sends standby images to a user whose ID is recieverID.
bool ServerClass::sendStandbyPics(SOCKET& passiveClientSock, string userID, string data) //returns "true" in case of success, "false" in case of failure.
{
	bool stop = false;
	//padd_zeroes(to_string(username.length()), 2) + username + sending_time_from_client + padd_zeroes(to_string(pic_path.length()), 3) + pic_path
	int check, currChars, totalReadChars = 0, senderNameLen, pathLen, i;
	string SenderName, sending_time_from_client, image_path;
	try
	{
		senderNameLen = stoi(data.substr(0, 2));
		SenderName = data.substr(2, senderNameLen);
		sending_time_from_client = data.substr(2 + senderNameLen, 13);
		pathLen = stoi(data.substr(2 + senderNameLen + 13, 3));
		image_path = data.substr(2 + senderNameLen + 13 + 3, pathLen);
	}
	catch (...)
	{
		cout << "Problen with data in ServerClass::sendStandbyPics" << endl;
		return false;
	}
	ifstream image;
	image.open(image_path, ios::binary);
	if (!image.is_open())
	{
		cout << "ServerClass::sendStandbyPics - weird :/" << endl;
		return false; //Not supposed to happen.
	}
	image.seekg(0, image.end);
	int image_size = image.tellg();
	image.seekg(0, image.beg);
	string header = (to_string(SEND_PIC_TO_RECIEVER) + "0" + padd_zeroes(to_string(senderNameLen), 2) + SenderName + padd_zeroes(to_string(time_now() - stoll(sending_time_from_client)), 13) + padd_zeroes(to_string(image_size), 8) + padd_zeroes(to_string(BUFF_SIZE), 7));
	
	//string KP_status_msg = to_string(KEEP_ALIVE_SERVER) + "1";
	//Send(passiveClientSock, KP_status_msg.c_str(), KP_status_msg.size());
	timeval tmv;
	tmv.tv_sec = 3;
	tmv.tv_usec = 3000000;
	fd_set sockHelp;
	sockHelp.fd_array[0] = passiveClientSock;
	sockHelp.fd_count = 1;
	char recv_buff[6];
	Send(passiveClientSock, header.c_str(), header.size());
	check = select(1, &sockHelp, NULL, NULL, &tmv);
	if (check > 0)
	{
		recv(passiveClientSock, recv_buff, 6, 0);
		//if (atoi(recv_buff) == stoi(to_string(SEND_PIC_TO_RECIEVER_RES) + "0"))
			//return false;
	}
	else
		return false;
	char final_msg[BUFF_SIZE], buff[BUFF_SIZE];
	for (i = 0; i < 5; i++)
		final_msg[i] = to_string(SEND_PIC_TO_RECIEVER)[i];
	final_msg[5] = '1';
	while (totalReadChars < image_size)
	{
		//emptyString(final_msg, 5, BUFF_SIZE-1);
		//emptyString(buff, 0, BUFF_SIZE-1);
		sockHelp.fd_count = 1;
		if (totalReadChars + (BUFF_SIZE - 13) < image_size)
			currChars = BUFF_SIZE - 13;
		else
			currChars = image_size - totalReadChars;
		totalReadChars += currChars;

		image.read(buff, currChars);
		for (i = 6; i < 6 + currChars; i++)
			final_msg[i] = buff[i - 6];

		if (Send(passiveClientSock, final_msg, 6 + currChars) == false)
		{
			stop = true;
			cout << "ServerClass::handleSendPic - Send failed" << endl;
			return false;
		}
		int check = select(1, &sockHelp, NULL, NULL, &tmv);
		if (check > 0)
		{
			recv(passiveClientSock, recv_buff, 6, 0);
			//if (atoi(recv_buff) != stoi(to_string(SEND_PIC_TO_RECIEVER_RES) + "0"))
				//return false;
		}
		/*for (int i = 0; !stop; i++)
		{
			int check = select(1, &sockHelp, NULL, NULL, &tmv);
			if (check > 0)
			{
				//SEND_MESSAGE_TO_RECIEVER_RES
				emptyString(recv_buff, 0, 5);
				recv(passiveClientSock, recv_buff, 5, 0);
				try
				{
					if (atoi(recv_buff) == stoi(to_string(KEEP_ALIVE)) && i < 2)
					{
						cout << "got keep alive" << endl;
					}
					else
					{
						sockHelp.fd_count = 1;
						int check = select(1, &sockHelp, NULL, NULL, &tmv);
						if (check > 0)
							recv(passiveClientSock, recv_buff + 5, 1, 0);
						else
						{
							stop = true;
							break;
						}
						if (atoi(recv_buff) == stoi(to_string(SEND_PIC_TO_RECIEVER_RES) + "0"))
						{
							cout << "good" << endl;
							break;
						}
						else
							stop = true;
					}
				}
				catch (...)
				{
					stop = true;
				}
			}
			else
				stop = true;
		}*/
	}
	//KP_status_msg[5] = '1';
	//Send(passiveClientSock, KP_status_msg.c_str(), KP_status_msg.size());
	image.close();
	_db.insertNewMessage("", "pic", userID, _db.getIDByName(SenderName), image_path, sending_time_from_client, "reciever");
	return true;
}
//-----------------------------------------------------------------------------


//Help functions:

//All the sends go through this function. It adds the message length to the message and send the message. If there's a problem with the socket it closes it and deletes the user form the connected users list.
bool ServerClass::Send(SOCKET& sock, const char data[], int size)
{
	int i, check, numOfConnectedUsers;
	char msg[BUFF_SIZE];
	addMessageLength(msg, data, size);
	check = send(sock, msg, size+7, 0);
	if (check <= 0) //if sending failed.
	{
		while (connectedUsersLocked);
		connectedUsersLocked = true;
		numOfConnectedUsers = _connectedUsers.size();
		//Find the user and remove him from the _connectedUsers list.
		for (i = 0; i < numOfConnectedUsers; i++)
		{
			if (_connectedUsers[i]->getPassiveSocket() == sock)
			{
				clientDisconnection(_connectedUsers[i], ALL_OUT_CLOSURE);
				break;
			}

			else if (_connectedUsers[i]->getSocket() == sock)
			{
				_connectedUsers[i]->setSocket(NULL);
				break;
			}
		}
		connectedUsersLocked = false;
		return false;
	}
	else
		return true;
}

//This function is used when big data (like pictures) need to be sent.
bool ServerClass::bigDataRecv(SOCKET& sock, char msg_c[], int size)
{
	int numOfReadBytes = 0, check;
	timeval tmv;
	tmv.tv_sec = 120;
	tmv.tv_usec = 120000000;
	fd_set sockHelp;
	sockHelp.fd_count = 1;
	sockHelp.fd_array[0] = sock;
	while (numOfReadBytes < size)
	{
		if (numOfReadBytes + MTU < size)
		{
			check = select(1, &sockHelp, NULL, NULL, &tmv);
			if (check > 0)
			{
				//this_thread::sleep_for(chrono::milliseconds(0));
				check = 0;
				while (check < MTU)
					check += recv(sock, msg_c + numOfReadBytes + check, MTU - check, 0);
				numOfReadBytes += MTU;
				if (check != MTU)
					cout << "ServerClass::bigDataRecv - El problema is here" << endl;
			}
			else
			{
				cout << "ServerClass::bigDataRecv - Y U ! send mi anything" << endl;
				return false;
			}
		}
		else
		{
			int len = size - numOfReadBytes;
			check = select(1, &sockHelp, NULL, NULL, &tmv);
			if (check > 0)
			{
				//this_thread::sleep_for(chrono::milliseconds(0));
				check = 0;
				while (check < len)
					check += recv(sock, msg_c + numOfReadBytes + check, len - check, 0);
				numOfReadBytes += len;
				if (check != len)
					cout << "ServerClass::bigDataRecv - El problema is here" << endl;
			}
			else
			{
				cout << "ServerClass::bigDataRecv - Y U ! send mi anything" << endl;
				return false;
			}
		}
	}
	return true;
}

//This fucntions determines whether the user connected successfuly or not. Respectively, it chooses whether to open a thread for him or not.
void ServerClass::connectionHandler(SOCKET& clientSock, string data)
{
	//COMMENT: In this function "clientSock" is the same as "clientSocket" since the "_accept" function send "clientSocket" as the parameter
	fd_set sockHelp;
	sockHelp.fd_count = 1;
	sockHelp.fd_array[0] = clientSock;
	timeval tmv;
	tmv.tv_sec = 3;
	tmv.tv_usec = 3000000;
	if (data[0] == '0') //active first
	{
		string connectionCode = rndStr();
		User* user = new User(clientSock, NULL, "0", "0", connectionCode, "", false, false);
		while (connectedUsersLocked);
		connectedUsersLocked = true;
		_connectedUsers.push_back(user);
		connectedUsersLocked = false;
		
		char buf[BUFF_SIZE];
		_itoa(CONNECTION_RES, buf, 10);
		string msg = buf;
		msg += "0"; //success
		msg += connectionCode;
		
		Send(clientSock, msg.c_str(), msg.size());

		// create new thread for client	and detach from it
		cout << "Active socket connected" << endl;
		user->setActiveOpen(true);
		thread tr = thread(&ServerClass::clientHandler, this, user);
		tr.detach();
	}
	
	else if (data[0] == '1') //passive second
	{
		User* user = NULL;
		char buf[BUFF_SIZE];
		string msgConnectionCode = data.substr(1, 10), username, userID;
		int i, numOfConnectedUsers;
		try
		{
			while (connectedUsersLocked);
			connectedUsersLocked = true;
			numOfConnectedUsers = _connectedUsers.size();
			for (i = 0; i < numOfConnectedUsers; i++)
			{
				if (_connectedUsers[i]->getPassiveConnectionCode() == msgConnectionCode)
				{
					_connectedUsers[i]->setPassiveSocket(clientSocket);
					user = _connectedUsers[i];
					username = user->getName();
					userID = user->getID();
					break;
				}
			}
			connectedUsersLocked = false;
		}
		catch (const std::out_of_range& oor)
		{
			connectedUsersLocked = false;
			return;
		}
		catch (...)
		{
			connectedUsersLocked = false;
			return;
		}
		strcpy(buf, "");
		_itoa(CONNECTION_RES, buf, 10);

		if (i == numOfConnectedUsers)
			buf[5] = '1';
		else
			buf[5] = '0';
		buf[6] = NULL;
		string msg = buf;
		
		Send(clientSock, msg.c_str(), msg.size());
		if (buf[5] == '0')
		{
			//see if there are any messages waiting for him
			//-------------------------------------------------------------------------------------------------------------
			checkForMessages(clientSock, username, userID);
			
			cout << "Passive Socket connected - " << user->getName() << endl;
			user->setPassiveOpen(true);
			thread tr = thread(&ServerClass::passiveClientHandler, this, user);
			tr.detach();
		}
	}
	
	else if (data[0] == '2') //passive first
	{
		char msg_c[BUFF_SIZE] = "";
		string data = "", code = "0";
		string connectionCode = rndStr();
		
		char buf[BUFF_SIZE];
		_itoa(CONNECTION_RES, buf, 10);
		string msg = buf;
		msg += "0"; //success
		msg += connectionCode;
		
		Send(clientSock, msg.c_str(), msg.size());
		int check = select(1, &sockHelp, NULL, NULL, &tmv);
		if (check > 0)
			check = recv(clientSock, msg_c, BUFF_SIZE, 0);
		if (check <= 0)
			return;
		User* user = new User(NULL, clientSock, "0", "0", "", connectionCode, false, false);
		while (connectedUsersLocked);
		connectedUsersLocked = true;
		_connectedUsers.push_back(user);
		connectedUsersLocked = false;
		msg = msg_c;
		try
		{
			code = msg.substr(0, 5);
			data = msg.substr(5);
			if (stoi(code) != SIGN_IN)
				handleNotInProtocol(clientSock, code);
			else
			{
				bool check = handleSignIn(user, data, false);
				while (connectedUsersLocked);
				connectedUsersLocked = true;
				string username = user->getName();
				string userID = user->getID();
				connectedUsersLocked = false;
				data = msg.substr(5);
				if (check)
				{
					checkForMessages(clientSock, username, userID);
					cout << "Passive socket connected - " << username << endl;
					user->setPassiveOpen(true);
					thread tr = thread(&ServerClass::passiveClientHandler, this, user);
					tr.detach();
				}
			}
		}
		catch (...)
		{
			cout << "ServerClass::connectionHandler - exception. msg = " << msg_c << endl;
			handleNotInProtocol(clientSock, code);
			//Send him a message that says that sign in failed and don't create a thread.
		}
	}

	else if (data[0] == '3') //active second
	{
		User* user = NULL;
		char buf[BUFF_SIZE];
		string msgConnectionCode = data.substr(1, 10);
		int i, numOfConnectedUsers;
		try
		{
			while (connectedUsersLocked);
			connectedUsersLocked = true;
			numOfConnectedUsers = _connectedUsers.size();
			for (i = 0; i < numOfConnectedUsers; i++)
			{
				if (_connectedUsers[i]->getActiveConnectionCode() == msgConnectionCode)
				{
					_connectedUsers[i]->setSocket(clientSocket);
					user = _connectedUsers[i];
					break;
				}
			}
			connectedUsersLocked = false;
		}
		catch (const std::out_of_range& oor)
		{
			connectedUsersLocked = false;
			return;
		}
		catch (...)
		{
			connectedUsersLocked = false;
			return;
		}
		strcpy(buf, "");
		_itoa(CONNECTION_RES, buf, 10);

		if (i == numOfConnectedUsers)
			buf[5] = '1';
		else
			buf[5] = '0';
		buf[6] = NULL;
		string msg = buf;
		
		Send(clientSock, msg.c_str(), msg.size());
		if (buf[5] == '0')
		{
			cout << "Active socket connected" << endl;
			user->setActiveOpen(true);
			thread tr = thread(&ServerClass::clientHandler, this, user);
			tr.detach();
		}
	}
	else
	{
		cout << "ServerClass::connectionHandler - Message from client not in protocol." << endl;
		handleNotInProtocol(clientSock, data);
	}
}

//This function disconnects a client's socket/s. If after the requested disconnections noth of the client's sockets are disconnected, the function deletes him from the connected users list.
void ServerClass::clientDisconnection(User* user, int option)
{
	int i;
	try
	{
		for (i = 0; i < (signed)_connectedUsers.size(); i++)
		{
			if (_connectedUsers[i] == user)
			{
				if (user->getActiveOpen())
				{
					if (option == JUST_ACTIVE || option == ALL_OUT_CLOSURE)
					{
						cout << "Active disconnecting client - " << user->getName() << endl;
						closesocket(user->getSocket());
						user->setSocket(0);
						user->setActiveOpen(false);
					}
				}
				if (user->getPassiveOpen())
				{
					if (option == JUST_PASSIVE || option == ALL_OUT_CLOSURE)
					{
						cout << "Passive disconnecting client - " << user->getName() << endl;
						closesocket(user->getPassiveSocket());
						user->setPassiveSocket(0);
						user->setPassiveOpen(false);
					}
				}
				if (user->getPassiveOpen() == false && user->getActiveOpen() == false)
				{
					cout << "Disconnecting client - " << user->getName() << endl;
					delete user;
					_connectedUsers.erase(_connectedUsers.begin() + i);
				}
				break;
			}
		}
	}
	catch (...)
	{}
	/*for (i = 0; i < (signed)_connectedUsers.size(); i++)
	{
		if (_connectedUsers[i] == user)
		{
			if (user->getSocket() != NULL)
				closesocket(user->getSocket());

			if (user->getPassiveSocket() != NULL)
				closesocket(user->getPassiveSocket());

			if (user->getName().empty() == false)
				cout << "Disconnection - " << _connectedUsers[i]->getName() << endl;

			delete user;

			_connectedUsers.erase(_connectedUsers.begin() + i);
			break;
		}
	}*/
}

/*bool ServerClass::connectPassiveSocket(char* address)
{
	WSADATA info;
	int checker = WSAStartup(MAKEWORD(2, 0), &info), endConnection = 0;
	if (checker)
		cout << "problem with WSAStartup" << endl;
	passiveSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (passiveSocket == INVALID_SOCKET)
		cout << "problem with socket function" << endl;
	clientServicePassive.sin_family = AF_INET;
	clientServicePassive.sin_addr.s_addr = inet_addr(address);
	clientServicePassive.sin_port = htons(PORT);
	checker = connect(passiveSocket, (sockaddr*)&clientServicePassive, sizeof(clientService));
	if (checker == SOCKET_ERROR)
	{
		cout << "failed at connecting to the passive socket." << endl;
		return false;
	}
	else
		return true;
}*/
//This function calls through the CMD to a program that sends an email to a user containing his new password.
void ServerClass::sendEmail(string clientEmail, string new_password) //return fail or success.
{
	while (email_locked);
	email_locked = true;
	string sendEmailProgramPath = EMAIL_PROGRAM_PATH;
	string command = "\"" + sendEmailProgramPath + "\"" + " " + clientEmail + " " + new_password;
	system(command.c_str());
	email_locked = false;
}

//This function produces random string.
string ServerClass::rndStr()
{
	if (!didSrand)
	{
		srand(time(NULL));
		didSrand = true;
	}
	//string alphanum = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	string rnd_str = "";
	int i, help;
	for (i = 0; i < 10; i++)
	{
		do
		{
			help = rand();
		} while (help < 48 || (help > 57 && help < 65) || (help > 90 && help < 97) || help > 122);
		//rnd_str += alphanum[rand() % (alphanum.length() - 1)];
		rnd_str += (char)help;
	}
	return rnd_str;
}

//This function checks if there's a user connected whose socket is sock.
bool ServerClass::isUserConnectedBySock(SOCKET& sock)
{
	while (connectedUsersLocked);
	connectedUsersLocked = true;
	int i, numOfConnectedUsers = _connectedUsers.size();
	bool found = false;
	for (i = 0; i < numOfConnectedUsers; i++)
	{
		if (_connectedUsers[i]->getSocket() == sock || _connectedUsers[i]->getPassiveSocket() == sock)
		{
			found = true;
			break;
		}
	}
	connectedUsersLocked = false;
	return found;
}

//This function checks if there's a user connected whose ID matches the parameter.
bool ServerClass::isUserConnectedByID(string ID)
{
	while (connectedUsersLocked);
	connectedUsersLocked = true;
	int i, numOfConnectedUsers = _connectedUsers.size();
	bool found = false;
	for (i = 0; i < numOfConnectedUsers; i++)
	{
		if (_connectedUsers[i]->getID() == ID)
		{
			found = true;
			break;
		}
	}
	connectedUsersLocked = false;
	return found;
}

//This function checks if there's a user connected whose name matches the parameter.
bool ServerClass::isUserConnectedByName(string name)
{
	while (connectedUsersLocked);
	connectedUsersLocked = true;
	int i, numOfConnectedUsers = _connectedUsers.size();
	bool found = false;
	for (i = 0; i < numOfConnectedUsers; i++)
	{
		if (_connectedUsers[i]->getName() == name)
		{
			found = true;
			break;
		}
	}
	connectedUsersLocked = false;
	return found;
}

//This function adds a 7 chars long string that contains the length of the parameter data after the 5th index.
void ServerClass::addMessageLength(char dest[], const char data[], int size)
{
	int len_of_data = size - 5;
	char len_of_data_c[7];
	string len_of_data_str, help_str;
	_itoa(len_of_data, len_of_data_c, 10);
	len_of_data_str = len_of_data_c;
	len_of_data_str = padd_zeroes(len_of_data_str, 7);
	/*while(len_of_data_str.size() < 7)
	{
		help_str = len_of_data_str;
		len_of_data_str = "0" + help_str;
	}*/
	//string help_data = data;
	//string data_str = help_data.substr(0, 5) + len_of_data_str + help_data.substr(5, len_of_data);
	int i;
	for (i = 0; i < size + 7; i++)
	{
		if (i < 5)
			dest[i] = data[i];
		else if (i >= 5 && i < 12)
			dest[i] = len_of_data_str[i - 5];
		else if (i >= 12)
			dest[i] = data[i - 7];
		else
			cout << "ServerClass::addMessageLength - WTF?? Not supposed to happen!!" << endl;
	}
}
