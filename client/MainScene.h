#pragma once
#include <map>
#include <time.h>
#include <list>

namespace Tmpl8
{
  class Surface;
};
class Client;
class GameObject;

struct KeyPressInfo
{
  KeyPressInfo(){}
  KeyPressInfo(char a_key, char a_state) : key(a_key), state(a_state), startTime(clock()) {}
  char key;
  char state;
  clock_t startTime;
};

class MainScene
{
public:
  MainScene() {}
  MainScene(Client* a_Client);
  ~MainScene(){}

  void Update(float a_Dt);
  void HandleInput();
  void Render(Tmpl8::Surface* a_Screen);

private:
  static Tmpl8::Surface backdrop, gameover;
  static Tmpl8::Surface Starting, s1, s2, s3;
  Tmpl8::Surface* m_startingTexture;
  Client* m_Client;
  unsigned int m_gameObjectCount;
  std::map<unsigned short, KeyPressInfo> m_sequenceMap; //sequence number, keypress
  float m_startTimer;
  float m_respawnTimer;
  GameObject* myPlayer;
};

