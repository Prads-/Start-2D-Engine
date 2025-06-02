#include "Network.h"
#ifdef BUILD_FOR_WINDOWS
#include <process.h>
#else
#include <pthread.h>
#include <unistd.h>

#define INVALID_SOCKET      -1
#define SOCKET_ERROR        -1
#endif
#include "CrossPlatformSleep.h"
using namespace CrossPlatformSleep;

Network::Network() {
	listenThreadIsDead = true;
	listenThreadSwitch = false;
}

void Network::checkListenThread() {
	listenThreadSwitch = false;
	while (!listenThreadIsDead) {
		crossPlatformSleep(1);
	}
}

ClientNetwork::ClientNetwork(std::string serverIp, int serverPort, int port, std::function<void(uint8*)> packetRecievedCallback) {
	serverAddress.sin_addr.s_addr = inet_addr(serverIp.c_str());
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(serverPort);

	this->port = port;
	this->packetRecievedCallback = packetRecievedCallback;
}

ClientNetwork::~ClientNetwork() {
#ifdef BUILD_FOR_WINDOWS
    closesocket(sock);
	WSACleanup();
#else
    close(sock);
#endif
	checkListenThread();
}

bool ClientNetwork::initialiseNetwork() {
    int err;
#ifdef BUILD_FOR_WINDOWS
    err = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (err != 0 || LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		return false;
	}
#endif

    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) {
		return false;
	}

	sockaddr_in addr;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);

	err = bind(sock, (const sockaddr*)&addr, sizeof(addr));
	if (err == SOCKET_ERROR) {
		return false;
	}

	listenThreadSwitch = true;
	listenThreadIsDead = false;
#ifdef BUILD_FOR_WINDOWS
	_beginthread(listenToServerThreadFunc, 0, this);
#else
    pthread_t t;
    pthread_create(&t, 0, (void *(*)(void*))listenToServerThreadFunc, (void*)this);
#endif
	return true;
}

void ClientNetwork::listenToServerThreadFunc(void *obj) {
	((ClientNetwork*)obj)->listenToServer();
}

void ClientNetwork::listenToServer() {
#ifdef BUILD_FOR_WINDOWS
	int fromAddrLen = sizeof(serverAddress);
#else
	socklen_t fromAddrLen;
#endif
	while (listenThreadSwitch) {
		int retVal = recvfrom(sock, (char*)dataPacketBuffer, DATA_PACKET_BUFFER_SIZE, 0, (sockaddr*)&serverAddress, &fromAddrLen);
		if (retVal > 0) {
			packetRecievedCallback(dataPacketBuffer);
		}
	}
	listenThreadIsDead = true;
}

void ClientNetwork::sendPacket(uint8 *packet, int packetSize) {
	sendPacketMutex.lock();
	sendto(sock, (const char*)packet, packetSize, 0, (const sockaddr*)&serverAddress, sizeof(serverAddress));
	sendPacketMutex.unlock();
}

void ClientNetwork::clientEnableBroadcast() {
	int broadcast = 1;
	setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (const char*)&broadcast, sizeof(broadcast));
}

void ClientNetwork::clientDisableBroadcast() {
	int broadcast = 0;
	setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (const char*)&broadcast, sizeof(broadcast));
}

ServerNetwork::ServerNetwork(int port, std::function<void(uint8*)> packetRecievedCallback) {
	this->port = port;
	this->packetRecievedCallback = packetRecievedCallback;
}

ServerNetwork::~ServerNetwork() {
#ifdef BUILD_FOR_WINDOWS
    closesocket(sock);
    WSACleanup();
#else
    close(sock);
#endif
	checkListenThread();
}

bool ServerNetwork::initialiseNetwork() {
    int err;
#ifdef BUILD_FOR_WINDOWS
    err = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (err != 0 || LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		return false;
	}
#endif

	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET) {
		return false;
	}

	sockAddr.sin_addr.s_addr = INADDR_ANY;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(port);

	err = bind(sock, (const sockaddr*)&sockAddr, sizeof(sockAddr));
    if (err == SOCKET_ERROR) {
		return false;
	}

	fromAddrLen = sizeof(fromAddr);
	listenThreadSwitch = true;
	listenThreadIsDead = false;
#ifdef BUILD_FOR_WINDOWS
    _beginthread(listenToClientThreadFunc, 0, this);
#else
    pthread_t t;
    pthread_create(&t, 0, (void*(*)(void*))listenToClientThreadFunc, (void*)this);
#endif
}

void ServerNetwork::sentPacket(uint8 *packet, int packetSize, sockaddr_in clientAddr) {
	sendPacketMutex.lock();
	sendto(sock, (const char*)packet, packetSize, 0, (const sockaddr*)&clientAddr, sizeof(clientAddr));
	sendPacketMutex.unlock();
}
sockaddr_in ServerNetwork::getClientAddress() {
	return fromAddr;
}

void ServerNetwork::listenToClientThreadFunc(void *obj) {
	((ServerNetwork*)obj)->listenToClient();	
}

void ServerNetwork::listenToClient() {
	while (listenThreadSwitch) {
        int retVal = recvfrom(sock, (char*)dataPacketBuffer, DATA_PACKET_BUFFER_SIZE, 0, (sockaddr*)&fromAddr, &fromAddrLen);
		if (retVal > 0) {
			packetRecievedCallback(dataPacketBuffer);
		}
	}
	listenThreadIsDead = true;
}
