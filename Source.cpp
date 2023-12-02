#include"header.h"

int main()
{
	while (true) {
		system("cls");
		cout << "-----------------------------MENU-------------------------------" << endl;
		cout << "1.SEND EMAIL." << endl;
		cout << "2.SEE LIST EMAIL." << endl;
		cout << "3.EXIT" << endl;
		int luachon;
		cout << " NHAP LUA CHON: ";
		cin >> luachon;
		if (luachon == 1) {
			SOCKET clientsocket;
			WSADATA wsaData;
			int wsaerr;
			setupDLL(wsaerr);
			if (wsaerr != 0)
				return 0;
			clientsocket = INVALID_SOCKET;
			clientsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (clientsocket == INVALID_SOCKET) {
				cout << "Error at socket " << WSAGetLastError() << endl;
				WSACleanup();
				return 0;
			}
			sockaddr_in clientService;
			clientService.sin_family = AF_INET;
			InetPton(AF_INET, L"127.0.0.1", &clientService.sin_addr.s_addr);
			clientService.sin_port = htons(2225);
			if (connect(clientsocket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR)
			{
				cout << "Client: connect() - failed to connect" << endl;
				WSACleanup();
				return 0;
			}
			sendmail sm;
			inputsendemail(sm);
			sendemail(clientsocket,sm);
			system("pause");
			closesocket(clientsocket);
			WSACleanup();
		}
		else if (luachon == 2)
		{
			SOCKET clientsocket;
			WSADATA wsaData;
			int wsaerr;
			setupDLL(wsaerr);
			if (wsaerr != 0)
				return 0;
			clientsocket = INVALID_SOCKET;
			clientsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (clientsocket == INVALID_SOCKET) {
				cout << "Error at socket " << WSAGetLastError() << endl;
				WSACleanup();
				return 0;
			}
			sockaddr_in serverAddress;
			serverAddress.sin_family = AF_INET;
			serverAddress.sin_port = htons(3335);
			InetPton(AF_INET, L"127.0.0.1", &serverAddress.sin_addr.s_addr);
			if (connect(clientsocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR)
			{
				cerr << "Failed to connect to POP3 server" << endl;
				closesocket(clientsocket);
				WSACleanup();
				return 1;
			}
			POP(clientsocket);
			closesocket(clientsocket);
			WSACleanup();
			system("pause");
		}
		else if (luachon == 3)
		{
			return 0;
		}
	}
}