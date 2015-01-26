#include "InputManager.h"


InputManager::InputManager(void)
{
  SetCharacters();
}


InputManager::~InputManager(void)
{
}


bool InputManager::IsKeyPressed(SDL_Keycode a_key){return m_keysPressed[a_key];}
bool InputManager::IsKeyDown(SDL_Keycode a_key){return m_keysDown[a_key];}
bool InputManager::IsKeyUp(SDL_Keycode a_key){return m_keysUp[a_key];}

void InputManager::SetKeyPressed(SDL_Keycode a_key){ m_keysUp[a_key] = false; if (!m_keysDown[a_key]){ m_keysDown[a_key] = true; m_keysPressed[a_key] = true; } }
void InputManager::SetKeyUp(SDL_Keycode a_key){m_keysUp[a_key] = true; m_keysDown[a_key] = false;}

void InputManager::Update(float a_fDeltaTime)
{
	for(auto iter = m_keysPressed.begin(); iter != m_keysPressed.end(); iter++)
	{
		iter->second = false;
	}	
  for (auto iter = m_keysUp.begin(); iter != m_keysUp.end(); iter++)
	{
		iter->second = false;
	}

  for (auto iter = m_mouseButtonPressed.begin(); iter != m_mouseButtonPressed.end(); iter++)
	{
		iter->second = false;
	}	
  for (auto iter = m_mouseButtonUp.begin(); iter != m_mouseButtonUp.end(); iter++)
	{
		iter->second = false;
	}
}

void InputManager::SetMouseButtonPressed(Uint8 a_mouseButton)
{
	m_mouseButtonUp[a_mouseButton] = false; 
  if (!m_mouseButtonDown[a_mouseButton])
  {
    m_mouseButtonDown[a_mouseButton] = true;
    m_mouseButtonPressed[a_mouseButton] = true;
  }
}

void InputManager::SetMouseButtonUp(Uint8 a_mouseButton)
{
	m_mouseButtonUp[a_mouseButton] = true;
	m_mouseButtonDown[a_mouseButton] = false;
}

char InputManager::GetKeyPress()
{
  for (auto i = m_keysPressed.begin(); i != m_keysPressed.end(); i++)
  {
    if (i->second == true)
    {
      for (auto it = m_characters.begin(); it != m_characters.end(); it++)
      if (it->first == i->first)
      {
        return it->second;
      }
    }
  }
  return 0;
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
  m_characters[SDLK_SLASH] = '/';

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