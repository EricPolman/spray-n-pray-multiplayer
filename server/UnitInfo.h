#pragma once
enum UnitType : unsigned char
{
  BULLET,
  ENEMY,
  PLAYER
};

struct UnitData
{
  UnitData() : changed(false), alive(true) {}
  unsigned short id;
  UnitType type;
  bool changed;
  unsigned char owner;
  bool alive;

  float x, y;
  float health;

  float rotation;
};
