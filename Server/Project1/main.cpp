#include "serverClass.h"
#include <chrono>
#include <iostream>

using namespace std;

void a_function(SOCKET);


int main(void)
{
	ServerClass server_class;
	server_class.serve();
	/*WSADATA info;
	int checker = WSAStartup(MAKEWORD(2, 0), &info), endConnection = 0;
	if (checker)
		cout << "problem with WSAStartup" << endl;
	SOCKET connectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (connectSocket == INVALID_SOCKET)
		cout << "problem with socket function: " << WSAGetLastError() << endl;
	struct sockaddr_in clientService;
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr("192.168.1.26");
	clientService.sin_port = htons(8888);
	checker = ::bind(connectSocket, (struct sockaddr *) &clientService, sizeof(clientService));
	if (checker == SOCKET_ERROR)
		cout << "problem with bind: " << WSAGetLastError() << endl;
	else
		cout << "Server socket succesfuly formed: " << connectSocket << endl;
	checker = listen(connectSocket, SOMAXCONN);
	if (checker == SOCKET_ERROR)
		cout << "problem with listen: " << WSAGetLastError() << endl;

	char buf[1024];
	int addrlen = sizeof(clientService);
	SOCKET clientSocket = accept(connectSocket, (sockaddr*)&clientService, &addrlen);
	if (clientSocket == INVALID_SOCKET)
	{
		cout << "accept failed" << endl;
		return 1;
	}
	thread t = thread(a_function, clientSocket);
	t.detach();
	int check = recv(clientSocket, buf, 1024, 0);
	cout << "it worked!!!" << endl;
	check = recv(clientSocket, buf, 1024, 0);
	cout << buf << endl;
	if (string(buf) == "Is there a god?")
		cout << "There IS a god!!" << endl;
	closesocket(clientSocket);*/
	getchar();
	return 0;
}



void a_function(SOCKET clientSocket)
{
	this_thread::sleep_for(chrono::seconds(3));
	fd_set sock_help;
	sock_help.fd_count = 1;
	sock_help.fd_array[0] = clientSocket;
	timeval t;
	t.tv_sec = 0;
	t.tv_usec = 0;
	int check = select(1, &sock_help, NULL, NULL, &t);
	cout << check;
}