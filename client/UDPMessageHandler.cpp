#include "UDPMessageHandler.h"
#include "GameObject.h"
#include <map>
#include "Enemy.h"
#include "Bullet.h"
#include "Player.h"
#include "Client.h"

UDPMessageHandler::UDPMessageHandler()
{
}


UDPMessageHandler::~UDPMessageHandler()
{
}

void UDPMessageHandler::ParseUDPMessage(char* a_buffer, unsigned int a_size, Client* a_Cli)
{
  char* bufferPointer = a_buffer;
  std::map<unsigned short, GameObject*>& gameObjectList = a_Cli->GetGameObjectList();
  while (bufferPointer < a_buffer + a_size)
  {
    BlankPacket* blankPacket = (BlankPacket*)bufferPointer;
    unsigned short sequenceCounter = blankPacket->header.sequenceCounter;
    unsigned char messageSize = blankPacket->header.messageSize;

    switch (blankPacket->header.command)
    {
      case UDPCommand::RESEND_PACKET:
      {
        break;
      }
      case UDPCommand::SET_POSITION:
      {
        SetPositionPacket* pPacket = (SetPositionPacket*)bufferPointer;
        m_Incommingsequences.push_back(pPacket->header.sequenceCounter);

        unsigned int structCount = (pPacket->header.messageSize - sizeof(UdpPacketHeader)) / 8;
        SetPositionStruct* structs = (SetPositionStruct*)(pPacket + 1);
        for (unsigned int i = 0; i < structCount; ++i)
        {
          SetPositionStruct& setPosStruct = structs[i];
          unsigned short objId = setPosStruct.GetId();
          auto it = gameObjectList.find(objId);
          if (it != gameObjectList.end())
            gameObjectList[objId]->SetPosition(setPosStruct.GetX(), setPosStruct.GetY());
        }
        break;
      }
      case UDPCommand::TRANSLATE:
      {
        SetPositionPacket* pPacket = (SetPositionPacket*)bufferPointer;
        unsigned int structCount = (pPacket->header.messageSize - sizeof(UdpPacketHeader)) / 8;
        SetPositionStruct* structs = (SetPositionStruct*)(pPacket + 1);
        for (unsigned int i = 0; i < structCount; ++i)
        {
          SetPositionStruct& setPosStruct = structs[i];
          unsigned short objId = setPosStruct.GetId();
          gameObjectList[objId]->Move(setPosStruct.GetX(), setPosStruct.GetY());
        }
        break;
      }
      case UDPCommand::NETWORK_INSTANTIATE:
      {
        NetworkInstantiatePacket* iPacket = (NetworkInstantiatePacket*)bufferPointer;
        InstantiateGameObject(iPacket, a_Cli);
        break;
      }
      case UDPCommand::RECEIVED_UPDATE:
      {
        ReceivedServerUpdatePacket uPacket = *(ReceivedServerUpdatePacket*)bufferPointer;
        m_lastReceivedSequence = uPacket.sequenceId;
        //printf("Received sequence counter: %i\n", m_lastReceivedSequence);
        a_Cli->m_gameFlowState = uPacket.gameflowState;
        SendUDPMessage((char*)&uPacket, a_Cli);
        break;
      }
      case UDPCommand::SYNC_STATE:
      {
        SyncStatePacket* sPacket = (SyncStatePacket*)bufferPointer;
        SyncStatePacketHandler(sPacket, a_Cli);
        break;
      }
      case UDPCommand::DESTROY_OBJECT:
      {
        DestroyObjectPacket* dPacket = (DestroyObjectPacket*)bufferPointer;
        auto it = gameObjectList.find(dPacket->objectId);
        if (it != gameObjectList.end())
          gameObjectList.erase(dPacket->objectId);
        break;
      }
      case UDPCommand::SET_HEAlTH:
      {
          SetHealthPacket* hPacket = (SetHealthPacket*)bufferPointer;
          auto it = gameObjectList.find(hPacket->objectId);
          if (it != gameObjectList.end())
            gameObjectList[hPacket->objectId]->m_Health = hPacket->health;
        break;
      }
    }

    bufferPointer += messageSize;
  }
}

void UDPMessageHandler::SendUDPMessage(char* a_buffer, Client* a_Cli)
{
  BlankPacket* blankPacket = (BlankPacket*)a_buffer;

  switch (blankPacket->header.command)
  {
    case UDPCommand::RESEND_PACKET:
    {
      break;
    }
    case UDPCommand::SET_POSITION:
    {
      SetPositionPacket pPacket = *((SetPositionPacket*)a_buffer);
      pPacket.header.id = a_Cli->GetID();
      pPacket.header.sequenceCounter = (unsigned char)a_Cli->GetSequence();
      char* b = (char*)&pPacket;

      sendto(a_Cli->UDPSocket, b, pPacket.header.messageSize, 0, (sockaddr*)&a_Cli->udpAddr, sizeof(a_Cli->udpAddr));
      a_Cli->IntergrateSequence();
      a_Cli->udpmessagesSent++;
      a_Cli->udpbytesSent += pPacket.header.messageSize;
      break;
    }
    case UDPCommand::NETWORK_INSTANTIATE:
    {
      NetworkInstantiatePacket iPacket = *((NetworkInstantiatePacket*)a_buffer);
      iPacket.header.id = a_Cli->GetID();
      iPacket.header.sequenceCounter = (unsigned char)a_Cli->GetSequence();
      char* b = (char*)&iPacket;
     
      sendto(a_Cli->UDPSocket, b, iPacket.header.messageSize, 0, (sockaddr*)&a_Cli->udpAddr, sizeof(a_Cli->udpAddr));
      a_Cli->IntergrateSequence();
      a_Cli->udpmessagesSent++;
      a_Cli->udpbytesSent += iPacket.header.messageSize;
      break;
    }
    case UDPCommand::RECEIVED_UPDATE:
    {
      ReceivedServerUpdatePacket uPacket = *((ReceivedServerUpdatePacket*)a_buffer);
      uPacket.header.id = a_Cli->GetID();;
      uPacket.header.sequenceCounter = (unsigned char)a_Cli->GetSequence();
      uPacket.sequenceId = m_lastReceivedSequence;
      char* b = (char*)&uPacket;
      sendto(a_Cli->UDPSocket, b, uPacket.header.messageSize, 0, (sockaddr*)&a_Cli->udpAddr, sizeof(a_Cli->udpAddr));
      a_Cli->IntergrateSequence();
      a_Cli->udpmessagesSent++;
      a_Cli->udpbytesSent += uPacket.header.messageSize;
      break;
    }
    case UDPCommand::CLIENT_INPUT:
    {
      InputPacket ciPacket = *((InputPacket*)a_buffer);
      ciPacket.header.id = a_Cli->GetID();
      ciPacket.header.sequenceCounter = (unsigned char)a_Cli->GetSequence();
      m_Outgoingsequences.push_back(ciPacket.header.sequenceCounter);
      char* b = (char*)&ciPacket;
      sendto(a_Cli->UDPSocket, b, ciPacket.header.messageSize, 0, (sockaddr*)&a_Cli->udpAddr, sizeof(a_Cli->udpAddr));
      a_Cli->IntergrateSequence();
      a_Cli->udpmessagesSent++;
      a_Cli->udpbytesSent += ciPacket.header.messageSize;
      break;
    }
    case UDPCommand::MOUSE_INPUT:
    {
      MouseInputPacket miPacket = *((MouseInputPacket*)a_buffer);
      miPacket.header.id = a_Cli->GetID();;
      miPacket.header.sequenceCounter = (unsigned char)a_Cli->GetSequence();
      char* b = (char*)&miPacket;
      sendto(a_Cli->UDPSocket, b, miPacket.header.messageSize, 0, (sockaddr*)&a_Cli->udpAddr, sizeof(a_Cli->udpAddr));
      a_Cli->IntergrateSequence();
      a_Cli->udpmessagesSent++;
      a_Cli->udpbytesSent += miPacket.header.messageSize;
      break;
    }
  }
}

void UDPMessageHandler::InstantiateGameObject(NetworkInstantiatePacket* a_ipacket, Client* a_Cli)
{
  switch (a_ipacket->unitType)
  {
    case UnitType::BULLET:
    {
      BulletData bd = *(BulletData*)a_ipacket->data;
      float speed = 350.0f;
      float length = sqrtf((bd.xdir * bd.xdir) + (bd.ydir * bd.ydir));
      bd.xdir = (bd.xdir / length) * speed;
      bd.ydir = (bd.ydir / length) * speed;
      Bullet *b = new Bullet(bd.xpos, bd.ypos, bd.xdir, bd.ydir, speed, "assets/bullet.png", a_Cli);
      b->m_Owner = bd.owner;
      a_Cli->AddGameobject(b, a_ipacket->objectId);
      break;
    }
    case UnitType::ENEMY:
    {
      BulletData bd = *(BulletData*)a_ipacket->data;
      Enemy *e = new Enemy(bd.xpos, bd.ypos, 80.0f, "assets/enemy.png", a_Cli);
      e->m_Owner = bd.owner;
      a_Cli->AddGameobject(e, a_ipacket->objectId);
      break;
    }
    case UnitType::PLAYER:
    {
      BulletData bd = *(BulletData*)a_ipacket->data;
      Player *p = new Player(bd.xpos, bd.ypos, 120.0f, "assets/player.png", a_Cli);
      p->m_Owner = bd.owner;
      a_Cli->AddGameobject(p, a_ipacket->objectId);
      break;
    }
  }
}

void UDPMessageHandler::SyncStatePacketHandler(SyncStatePacket* a_sPacket, Client* a_Cli)
{
  switch (a_sPacket->data.type)
  {
    case UnitType::BULLET:
    {
      Bullet* b = new Bullet(a_sPacket->data.x, a_sPacket->data.y, 0, 0, 0, "assets/bullet.png", a_Cli);
      a_Cli->AddGameobject(b, a_sPacket->objectId);
      break;
    }
    case UnitType::ENEMY:
    {
      Enemy* e = new Enemy(a_sPacket->data.x, a_sPacket->data.y, 80.0f, "assets/enemy.png", a_Cli);
      e->m_Health = (unsigned char)a_sPacket->data.health;
      a_Cli->AddGameobject(e, a_sPacket->objectId);
      break;
    }
    case UnitType::PLAYER:
    {
      Player *p = new Player(a_sPacket->data.x, a_sPacket->data.y, 120.0f, "assets/player.png", a_Cli);
      p->m_Health = (unsigned char)a_sPacket->data.health;
      a_Cli->AddGameobject(p, a_sPacket->objectId);
      break;
    }
  }
}