#pragma once
#include <map>
#include "glm/glm.hpp"
#include "Singleton.h"
#include "Keycodes.h"

//using namespace glm;

struct vector2
{
  float x, y;
};

struct InputData
{
  glm::vec2 m_mousePosition;
  glm::vec2 m_mouseMovement;

  std::map<Keycode, bool> m_keysDown;
  std::map<Keycode, bool> m_keysPressed;
  std::map<Keycode, bool> m_keysUp;

  std::map<unsigned char, bool> m_mouseButtonDown;
  std::map<unsigned char, bool> m_mouseButtonPressed;
  std::map<unsigned char, bool> m_mouseButtonUp;
};

class InputManager : public Singleton<InputManager>
{
public:
	~InputManager(void);
	InputManager(void);
	
  bool IsKeyPressed(unsigned short a_cliId, Keycode a_key);
  bool IsKeyDown(unsigned short a_cliId, Keycode a_key);
  bool IsKeyUp(unsigned short a_cliId, Keycode a_key);
	
  void SetKeyPressed(unsigned short a_cliId, Keycode a_key);
  void SetKeyUp(unsigned short a_cliId, Keycode a_key);

  void RemoveClient(unsigned short);

  bool IsMouseButtonPressed(unsigned short a_cliId, unsigned char a_mouseButton)
  { 
    return ipc[a_cliId].m_mouseButtonPressed[a_mouseButton];
  }
  bool IsMouseButtonDown(unsigned short a_cliId, unsigned char a_mouseButton)
  { 
    return ipc[a_cliId].m_mouseButtonDown[a_mouseButton]; 
  }
  bool IsMouseButtonUp(unsigned short a_cliId, unsigned char a_mouseButton)
  { 
    return ipc[a_cliId].m_mouseButtonUp[a_mouseButton]; 
  }
	
  void SetMouseButtonPressed(unsigned short a_cliId, unsigned char a_mouseButton);
  void SetMouseButtonUp(unsigned short a_cliId, unsigned char a_mouseButton);
	
  void SetMousePosition(unsigned short a_cliId, float a_X, float a_Y)
  {
    ipc[a_cliId].m_mousePosition.x = a_X;
    ipc[a_cliId].m_mousePosition.y = a_Y;
  }
  
  glm::vec2 GetMousePosition(unsigned short a_cliID)
  {
    return ipc[a_cliID].m_mousePosition;
  }

  char GetKeyPress();
  void SetCharacters();

  void Update(float a_fDeltaTime);
private:
  std::map<unsigned short, InputData> ipc; //input per client

  std::map<Keycode, char> m_characters;
};

#define Input InputManager::GetInstance()