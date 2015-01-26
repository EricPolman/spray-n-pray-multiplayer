#include "MainMenu.h"
#include "Client.h"
#include "surface.h"

MainMenu::MainMenu(Surface* a_scr, Client* a_client)
: screen(a_scr), client(a_client)/*, font(new Font("assets/fonts/font_grey.png", "0123456789abcdefghijklmnopqrstuvwxyz-"))*/
{
  background = new Surface("assets/background.png");
  lobbyBackground = new Surface("assets/lobbybg.png");
}


MainMenu::~MainMenu()
{
  //delete font;
}

void MainMenu::Render()
{
  if (client->GetState() == IN_LOBBY)
    lobbyBackground->CopyTo(screen, 0, 0);
  else if (client->GetState() != IN_ROOM)
    background->CopyTo(screen, 0, 0);
}

void MainMenu::Update()
{

}
