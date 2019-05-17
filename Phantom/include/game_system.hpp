#pragma once

#include "types.h"

#include <cstdio>
#include <typeinfo>

#include "memory_manager.hpp"

namespace phantom
{

class Component;

////////////////////////////////////////////////////////////////
template<typename T>
class GameSystem : public Updatable
{
public:
  void preUpdate() override;

  void update() override;

  void fixedUpdate() override;

  void postUpdate() override;

  MemoryManager<T> memory_manager_;
};

////////////////////////////////////////////////////////////////
template<typename T>
void GameSystem<T>::preUpdate()
{
  for (auto& component : memory_manager_)
  {
    component.preUpdate();
  }
}

////////////////////////////////////////////////////////////////
template<typename T>
void GameSystem<T>::update()
{
  for (auto& component : memory_manager_)
  {
    component.update();
  }
}

////////////////////////////////////////////////////////////////
template<typename T>
void GameSystem<T>::fixedUpdate()
{
  for (auto& component : memory_manager_)
  {
    component.fixedUpdate();
  }
}

////////////////////////////////////////////////////////////////
template<typename T>
void GameSystem<T>::postUpdate()
{
  for (auto& component : memory_manager_)
  {
    component.postUpdate();
  }
}

} // end namespace phantom