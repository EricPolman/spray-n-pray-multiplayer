#include "InputManager.h"


InputManager::InputManager(void)
{
  SetCharacters();
}


InputManager::~InputManager(void)
{
}

void InputManager::RemoveClient(unsigned short clid)
{
  for (auto cli = ipc.begin(); cli != ipc.end(); ++cli)
  {
    if (cli->first == clid)
    {
      ipc.erase(cli);
      break;
    }
  }
}

bool InputManager::IsKeyPressed(unsigned short a_cliId, Keycode a_key)
{
  return ipc[a_cliId].m_keysPressed[a_key];
}
bool InputManager::IsKeyDown(unsigned short a_cliId, Keycode a_key)
{
  return ipc[a_cliId].m_keysDown[a_key];
}
bool InputManager::IsKeyUp(unsigned short a_cliId, Keycode a_key)
{
  return ipc[a_cliId].m_keysUp[a_key];
}

void InputManager::SetKeyPressed(unsigned short a_cliId, Keycode a_key)
{
  ipc[a_cliId].m_keysUp[a_key] = false; if (!ipc[a_cliId].m_keysDown[a_key]){ ipc[a_cliId].m_keysDown[a_key] = true; ipc[a_cliId].m_keysPressed[a_key] = true; }
}
void InputManager::SetKeyUp(unsigned short a_cliId, Keycode a_key)
{
  ipc[a_cliId].m_keysUp[a_key] = true; ipc[a_cliId].m_keysDown[a_key] = false;
}

void InputManager::Update(float a_fDeltaTime)
{
  for (auto cli = ipc.begin(); cli != ipc.end(); cli++)
  {
    for (auto iter = cli->second.m_keysPressed.begin(); iter != cli->second.m_keysPressed.end(); iter++)
    {
      iter->second = false;
    }
    for (auto iter = cli->second.m_keysUp.begin(); iter != cli->second.m_keysUp.end(); iter++)
    {
      iter->second = false;
    }

    for (auto iter = cli->second.m_mouseButtonPressed.begin(); iter != cli->second.m_mouseButtonPressed.end(); iter++)
    {
      iter->second = false;
    }
    for (auto iter = cli->second.m_mouseButtonUp.begin(); iter != cli->second.m_mouseButtonUp.end(); iter++)
    {
      iter->second = false;
    }
  }
}

void InputManager::SetMouseButtonPressed(unsigned short a_cliId, unsigned char a_mouseButton)
{
  ipc[a_cliId].m_mouseButtonUp[a_mouseButton] = false;
  if (!ipc[a_cliId].m_mouseButtonDown[a_mouseButton])
  {
    ipc[a_cliId].m_mouseButtonDown[a_mouseButton] = true;
    ipc[a_cliId].m_mouseButtonPressed[a_mouseButton] = true;
  }
}

void InputManager::SetMouseButtonUp(unsigned short a_cliId, unsigned char a_mouseButton)
{
  ipc[a_cliId].m_mouseButtonUp[a_mouseButton] = true;
  ipc[a_cliId].m_mouseButtonDown[a_mouseButton] = false;
}

void InputManager::SetCharacters()
{
  m_characters[SDLK_a] = 'a';
  m_characters[SDLK_b] = 'b';
  m_characters[SDLK_c] = 'c';
  m_characters[SDLK_d] = 'd';
  m_characters[SDLK_e] = 'e';
  m_characters[SDLK_f] = 'f';
  m_characters[SDLK_g] = 'g';
  m_characters[SDLK_h] = 'h';
  m_characters[SDLK_i] = 'i';
  m_characters[SDLK_j] = 'j';
  m_characters[SDLK_k] = 'k';
  m_characters[SDLK_l] = 'l';
  m_characters[SDLK_m] = 'm';
  m_characters[SDLK_n] = 'n';
  m_characters[SDLK_o] = 'o';
  m_characters[SDLK_p] = 'p';
  m_characters[SDLK_q] = 'q';
  m_characters[SDLK_r] = 'r';
  m_characters[SDLK_s] = 's';
  m_characters[SDLK_t] = 't';
  m_characters[SDLK_u] = 'u';
  m_characters[SDLK_v] = 'v';
  m_characters[SDLK_w] = 'w';
  m_characters[SDLK_x] = 'x';
  m_characters[SDLK_y] = 'y';
  m_characters[SDLK_z] = 'z';
  m_characters[SDLK_SPACE] = ' ';
  m_characters[SDLK_BACKSPACE] = '\b';

  m_characters[SDLK_PERIOD] = '.';
  m_characters[SDLK_KP_PERIOD] = '.';

  m_characters[SDLK_0] = '0';
  m_characters[SDLK_1] = '1';
  m_characters[SDLK_2] = '2';
  m_characters[SDLK_3] = '3';
  m_characters[SDLK_4] = '4';
  m_characters[SDLK_5] = '5';
  m_characters[SDLK_6] = '6';
  m_characters[SDLK_7] = '7';
  m_characters[SDLK_8] = '8';
  m_characters[SDLK_9] = '9';

  m_characters[SDLK_KP_0] = '0';
  m_characters[SDLK_KP_1] = '1';
  m_characters[SDLK_KP_2] = '2';
  m_characters[SDLK_KP_3] = '3';
  m_characters[SDLK_KP_4] = '4';
  m_characters[SDLK_KP_5] = '5';
  m_characters[SDLK_KP_6] = '6';
  m_characters[SDLK_KP_7] = '7';
  m_characters[SDLK_KP_8] = '8';
  m_characters[SDLK_KP_9] = '9';
}