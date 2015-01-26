#pragma once
#include <map>

class Room;
class Client;

class MessageManager
{
public:
  MessageManager() : sendBufferPosition(0), processBufferPosition(0) 
  {
    _sendBuffer = new char[2048];
    _processBuffer = new char[2048];
    _processBuffer = new char[4096 << 2];
  }
  ~MessageManager();

  void FlushToRoom(Room*);
  void FlushToClient(Client*);
  void BufferMessage(char* message, unsigned int size);
  void ClearBuffer()
  {
    sendBufferPosition = 0;
    memset(_sendBuffer, 0, 2048);
  }
  void ClearProcessBuffer()
  {
    processBufferPosition = 0;
    memset(_processBuffer, 0, 4096 << 2);
  }
  void ClearInputBuffer()
  {
    inputBufferPosition = 0;
    memset(_inputBuffer, 0, 4096 << 2);
  }

  unsigned int sendBufferPosition;
  unsigned int inputBufferPosition;
  unsigned int processBufferPosition;
  char* _sendBuffer;
  char* _inputBuffer;
  char* _processBuffer;
private:
  std::map <unsigned short, std::pair<unsigned short, char*>> _savedMessages;

};

