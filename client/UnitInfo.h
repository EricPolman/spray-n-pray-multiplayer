#pragma once
enum UnitType : unsigned char
{
  BULLET,
  ENEMY,
  PLAYER
};

struct UnitData
{
  UnitData() : changed(true) {}
  unsigned short id;
  UnitType type;
  bool changed;
  unsigned char owner;

  float x, y;
  float health;

  float rotation;
};
