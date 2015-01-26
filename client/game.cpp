#include "game.h"
#include "Client.h"
#include "MainScene.h"
#include "MainMenu.h"
#include "GameObject.h"
#include "InputManager.h"
#include <iostream>
#include "time.h"

using namespace Tmpl8;				// to use template classes

Client* c;
MainScene mainScene;
MainMenu mainMenu;
// -----------------------------------------------------------
// Initialize the game
// -----------------------------------------------------------
void Game::Init()
{
  c = new Client();
  mainScene = MainScene(c);
  mainMenu = MainMenu(screen, c);
}

// -----------------------------------------------------------
// Main game tick function
// -----------------------------------------------------------
void Game::Tick( float _DT )
{
  screen->Clear(0);

  c->Update(screen, _DT);


  if (c->GetState() == ClientState::IN_ROOM)
  {
    mainScene.Update(_DT);
  }
  else
    mainMenu.Update();
}

void Game::Render()
{
  if (c->GetState() == ClientState::IN_ROOM)
  {
    mainScene.Render(screen);
  }
  else
  {
    mainMenu.Render();
  }
}

void Game::OnGUI()
{
  c->OnGUI(screen);
}

void Game::Shutdown()
{
  if (c)
    delete c;
}