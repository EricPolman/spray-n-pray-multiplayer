#include "Server.h"
#include <winsock2.h>
#include <WS2tcpip.h>
#include <iostream>

#define BUFFER_SIZE 128
#define ADDRESS "127.0.0.1"
#define PORT 3742

Server::Server()
{
  WSAData wsaData;
  WSAStartup(MAKEWORD(2, 2), &wsaData);

  std::cout << "Server started.\n";

  SOCKET socketFileDescriptor;
  sockaddr_in myAddress;

  char buffer[BUFFER_SIZE];
  int msgSize = 0;

  socketFileDescriptor = socket(AF_INET, SOCK_DGRAM, 0);

  myAddress.sin_family = AF_INET;
  inet_pton(AF_INET, ADDRESS, &(myAddress.sin_addr.s_addr));
  myAddress.sin_port = htons(PORT);

  bind(socketFileDescriptor, (sockaddr*)&myAddress, sizeof(myAddress));

  // other info
  sockaddr_in otherAddress;
  int otherAddressSize;
  short otherPort;
  char otherIp[16];

  do
  {
    otherAddressSize = sizeof(otherAddress);
    msgSize = recvfrom(socketFileDescriptor, buffer, BUFFER_SIZE, 0, (sockaddr*)&otherAddress, &otherAddressSize);
    // send message echo
    sendto(socketFileDescriptor, buffer, msgSize, 0, (sockaddr*)&otherAddress, otherAddressSize);

    printf("New incoming message: [%s]\n", buffer);

    inet_ntop(AF_INET, &(otherAddress.sin_addr.s_addr), otherIp, 16);
    otherPort = ntohs(otherAddress.sin_port);
    printf("Message came from: %s:%i\n\n", otherIp, otherPort);
  }
  while (strcmp(buffer, "quit") != 0);

  printf("Bye!\n");

  closesocket(socketFileDescriptor);
  WSACleanup();
}


Server::~Server()
{
}
