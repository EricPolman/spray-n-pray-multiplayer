#pragma once
#include "UnitInfo.h"

enum UDPCommand : unsigned char
{
  SET_ADDRESS = 0,
  RESEND_PACKET = 1,
  SET_POSITION = 2,
  NETWORK_INSTANTIATE = 3,
  SYNC_STATE = 4,
  TRANSLATE = 5,
  RECEIVED_UPDATE = 6,
  CLIENT_INPUT = 7,
  MOUSE_INPUT = 8,
  DESTROY_OBJECT = 9,
  SET_HEAlTH = 10,
  GAME_STATE = 11
};

enum GameFlowState : unsigned char
{
  GAME_UNINITIALIZED = 0,
  GAME_STARTING = 1,
  GAME_RUNNING = 2,
  GAME_PAUSED = 3,
  GAME_OVER = 4,
  GAME_RESTARTING = 5
};

struct BulletData
{
  unsigned char owner;
  float xpos, ypos;
  float xdir, ydir;
};

struct UdpPacketHeader
{
  UdpPacketHeader(unsigned char cliId, unsigned char seqCount, UDPCommand cmd)
  : id(cliId), sequenceCounter(seqCount), command(cmd)
  {

  }
  unsigned char id, sequenceCounter, messageSize;
  UDPCommand command;
};

struct BlankPacket
{
  BlankPacket(unsigned char cliId, unsigned char seqCount)
  : header(cliId, seqCount, SET_ADDRESS)
  {
    header.messageSize = (unsigned char)sizeof(BlankPacket);
  }
  UdpPacketHeader header;
};

struct SetPositionStruct
{
  SetPositionStruct(){}
  SetPositionStruct(unsigned short _id, float _x, float _y)
  {
    Set(_id, _x, _y);
  }
  void Set(unsigned short _id, float _x, float _y)
  {
    x = _x, y = _y;
    ix = (ix & 0xFFFFFF00) + (_id >> 8);
    iy = (iy & 0xFFFFFF00) + (_id & 0xFF);
  }
  unsigned short GetId()
  {
    return ((ix & 0xFF) << 8) + (iy & 0xFF);
  }
  float GetX() { unsigned int val = (ix & 0xFFFFFF00); return *reinterpret_cast<float*>(&val); }
  float GetY() { unsigned int val = (iy & 0xFFFFFF00); return *reinterpret_cast<float*>(&val); }
  union { float x; unsigned int ix; };
  union { float y; unsigned int iy; };
};
struct SetPositionPacket
{
  SetPositionPacket(unsigned char cliId, unsigned char seqCount)
  : header(cliId, seqCount, SET_POSITION)
  {
    header.messageSize = (unsigned char)sizeof(SetPositionPacket);
  }
  UdpPacketHeader header;
};
struct NetworkInstantiatePacket
{
  NetworkInstantiatePacket(unsigned char cliId, unsigned char seqCount, unsigned short objId)
  : header(cliId, seqCount, NETWORK_INSTANTIATE), objectId(objId)
  {
    header.messageSize = (unsigned char)sizeof(NetworkInstantiatePacket)-32;
  }
  NetworkInstantiatePacket(unsigned short objId, bool clientToServer, BulletData& a_bulletdata) //bullet constructor
    : header(0, 0, NETWORK_INSTANTIATE), objectId(objId)
  {
    header.messageSize = (unsigned char)sizeof(BulletData)+(unsigned char)sizeof(NetworkInstantiatePacket)-32;
    *reinterpret_cast<BulletData*>(&data) = a_bulletdata;
  }
  UdpPacketHeader header;

  unsigned short objectId;
  UnitType unitType;
  char data[32];
};

struct SetHealthPacket
{
  SetHealthPacket(unsigned char cliId, unsigned char seqCount, unsigned short objId, unsigned char a_health)
  : header(cliId, seqCount, SET_HEAlTH), health(a_health), objectId(objId)
  {
    header.messageSize = (unsigned char)sizeof(SetHealthPacket);
  }
  UdpPacketHeader header;
  unsigned short objectId;
  unsigned char health;
};

struct SyncStatePacket
{
  SyncStatePacket(unsigned char cliId, unsigned char seqCount, unsigned short objId, UnitData& udata)
  : header(cliId, seqCount, SYNC_STATE), objectId(objId), data(udata)
  {
    header.messageSize = (unsigned char)sizeof(SyncStatePacket);
  }
  UdpPacketHeader header;
  unsigned short objectId;
  UnitData data;
};

struct ReceivedServerUpdatePacket
{
  ReceivedServerUpdatePacket(unsigned char cliId, unsigned char seqCount, unsigned char receivedSeqCount, GameFlowState a_gfState)
  : header(cliId, seqCount, RECEIVED_UPDATE), sequenceId(receivedSeqCount), gameflowState(a_gfState)
  {
    header.messageSize = (unsigned char)sizeof(ReceivedServerUpdatePacket);
  }
  UdpPacketHeader header;
  GameFlowState gameflowState;
  unsigned char sequenceId;
};

struct InputPacket
{
  InputPacket(unsigned char cliId, unsigned char seqCount, char pInput, char inputstate)
  : header(cliId, seqCount, CLIENT_INPUT), input(pInput), state(inputstate)
  {
    header.messageSize = (unsigned char)sizeof(InputPacket);
  }
  UdpPacketHeader header;
  char input;
  char state;
};

struct MouseInputPacket
{
  MouseInputPacket(unsigned char cliId, unsigned char seqCount, unsigned short xpos, unsigned short ypos, char inputstate)
  : header(cliId, seqCount, MOUSE_INPUT), x(xpos), y(ypos), state(inputstate)
  {
    header.messageSize = (unsigned char)sizeof(MouseInputPacket);
  }
  UdpPacketHeader header;
  unsigned short x, y;
  char state;
};

struct DestroyObjectPacket
{
  DestroyObjectPacket(unsigned char cliId, unsigned char seqCount, unsigned short objId)
  : header(cliId, seqCount, DESTROY_OBJECT), objectId(objId)
  {
    header.messageSize = (unsigned char)sizeof(DestroyObjectPacket);
  }
  UdpPacketHeader header;
  unsigned short objectId;
};
