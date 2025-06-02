#ifndef NETWORK_H
#define NETWORK_H

#include "declarations.h"

#ifdef BUILD_FOR_WINDOWS
#include <WinSock2.h>

#pragma comment(lib, "Ws2_32.lib")
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include <string>
#include <functional>
#include "DataPackets.h"
#include <mutex>

class Network {
protected:
#ifdef BUILD_FOR_WINDOWS
    WSADATA wsaData;
	SOCKET sock;
#else
    int sock;
#endif
	std::function<void(uint8*)> packetRecievedCallback;
	int port;
	uint8 dataPacketBuffer[DATA_PACKET_BUFFER_SIZE];
	std::mutex sendPacketMutex;
	volatile bool listenThreadSwitch, listenThreadIsDead;

	void checkListenThread();

public:
	Network();
	virtual ~Network() { }

	virtual bool initialiseNetwork() = 0;
};

class ClientNetwork : public Network {
private:
	sockaddr_in serverAddress;

	static void listenToServerThreadFunc(void *obj);
	void listenToServer();

public:
	ClientNetwork(std::string serverIp, int serverPort, int port, std::function<void(uint8*)> packetRecievedCallback);
	~ClientNetwork();

	bool initialiseNetwork();
	void sendPacket(uint8 *packet, int packetSize);
	void clientEnableBroadcast();
	void clientDisableBroadcast();
};

class ServerNetwork : public Network {
private:
	sockaddr_in sockAddr, fromAddr;
#ifdef BUILD_FOR_WINDOWS
    int fromAddrLen;
#else
    socklen_t fromAddrLen;
#endif
	
	static void listenToClientThreadFunc(void *obj);
	void listenToClient();

public:
	ServerNetwork(int port, std::function<void(uint8*)> packetRecievedCallback);
	~ServerNetwork();

	bool initialiseNetwork();
	void sentPacket(uint8 *packet, int packetSize, sockaddr_in clientAddr);
	sockaddr_in getClientAddress();
};

#endif
