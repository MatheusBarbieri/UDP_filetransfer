#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <memory>
#include <iostream>
#include <fstream>

#define DGRAMSIZE 512
#define DATASIZE (DGRAMSIZE-3*sizeof(int32_t))

#define CONNECT 1
#define ACK 2
#define MESSAGE 3
#define DATAGRAM 4
#define DATAFILE 5
#define ENDFILE 6

#define ACCEPT 30
#define REJECT 31

#define TIMEOUT -99

class UDPSocket;
class UDPConnection;
class UDPServer;
class UDPClient;

typedef std::shared_ptr<UDPServer> udpserver_ptr;
typedef std::shared_ptr<UDPConnection> udpconnection_ptr;
typedef std::shared_ptr<UDPClient> udpclient_ptr;

typedef struct datagram {
    int32_t type;
    int32_t seqNumber;
    int32_t size;
    char data[DATASIZE];
} Datagram;

void zerosDatagram (Datagram* dg);

class UDPSocket {
protected:
    struct sockaddr_in socketAddrLocal;
    int socketDesc;
public:
    struct sockaddr_in* getAddr();
    int getSocketDesc();
};

class UDPConnection: public UDPSocket {
friend class UDPServer;
protected:
    bool connected;
    int port;

public:
    UDPConnection();
    UDPConnection(int port, sockaddr_in socketAddrRemote, sockaddr_in socketAddrLocal);

    struct sockaddr_in socketAddrRemote;
    int sendDatagram(Datagram &dg);
    int recDatagram();
    int sendDatagramMaxTries(Datagram &dg, int maxTries);
    int recDatagramTimeOut(int timeOut);
    char sendbuffer[DGRAMSIZE];
    char recvbuffer[DGRAMSIZE];
    long recvMessageSize;

    Datagram* getRecvbuffer();
    int sendString(std::string str);
    std::string receiveString();
    int sendMessage(char* buffer, int length);
    int sendFile(FILE* file);
    char* receiveMessage();
    long getRecvMessageSize();
    int receiveFile(FILE* file);

    struct sockaddr_in* getAddrRemote();
    bool isConnected();
    void close();
};

class UDPClient: public UDPConnection {
public:
    UDPClient();
    UDPClient(int port, std::string ip);
    ~UDPClient();
    int connect();
    int waitResponse();
    void close();
};

class UDPServer: public UDPConnection {
public:
    UDPServer();
    UDPServer(int port);
    ~UDPServer();
    udpconnection_ptr accept();
    void _bind();
};
