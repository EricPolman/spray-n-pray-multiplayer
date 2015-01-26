#pragma once

class Client; 
namespace Tmpl8
{
  class Surface;
  class Font;
};
using namespace Tmpl8;

class MainMenu
{
public:
  MainMenu() /*: font(new Font("assets/fonts/font_grey.png", "0123456789abcdefghijklmnopqrstuvwxyz-"))*/ {}
  MainMenu(Surface* a_scr, Client*);
  ~MainMenu();

  void Render();
  void Update();

  Font* font;
  Surface* screen;
  Surface* background, *lobbyBackground;

  Client* client;
};

