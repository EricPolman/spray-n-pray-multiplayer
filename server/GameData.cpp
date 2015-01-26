#include "GameData.h"
#include <windows.h>
#include <map>

static std::map<unsigned short, CRITICAL_SECTION> critSecs;
GameData::GameData()
{
  unitData.reserve(256);
}


GameData::~GameData()
{
}


UnitData GameData::GetUnit(unsigned short id)
{
  EnterCriticalSection(&critSecs[id]);
  UnitData dat = unitData[id];
  LeaveCriticalSection(&critSecs[id]);
  return dat;
}

unsigned short GameData::AddUnit(UnitData& data)
{
  unsigned short id = unitData.size();
  unitData.push_back(data);
  critSecs.insert(std::pair<unsigned short, CRITICAL_SECTION>(id, CRITICAL_SECTION()));
  InitializeCriticalSection(&critSecs[id]);
  return id;
}

unsigned short GameData::GetNextId()
{
  if (freedUnits.empty())
  {
    unitData.push_back(UnitData());
    return unitData.size() - 1;
  }
  else
  {
    unsigned short id = *freedUnits.begin();
    freedUnits.erase(freedUnits.begin());
    return id;
  }
}
