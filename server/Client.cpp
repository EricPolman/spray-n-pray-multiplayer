#include "Client.h"
#include "GameServer.h"

//Client::Client()
//{
//}


Client::~Client()
{
}


void Client::Disconnect()
{
  closesocket(tcpSocket);
}

void Client::SendServerMessage(std::string message)
{
  send(tcpSocket, message.c_str(), message.size() + 1, 0);
  GameServer::instance->_clientManager.bytesSent += message.size() + 1;
  GameServer::instance->_clientManager.messagesSent++;

  //printf("\nClientManager\n");
  //printf("Messages sent:    %u\n", GameServer::instance->_clientManager.messagesSent);
  //printf("Bytes sent:    %u\n", GameServer::instance->_clientManager.bytesSent);
  //printf("Messages received:    %u\n", GameServer::instance->_clientManager.messagesReceived);
  //printf("Bytes received:    %u\n\n", GameServer::instance->_clientManager.bytesReceived);
}
