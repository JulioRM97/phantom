#pragma once



#include <deque>
#include <map>
#include <memory>
#include <string>
#include <typeindex>

#include "game_system.hpp"
#include "memory_manager.hpp"



namespace phantom
{
class Node;

class NodeManager : public MemoryManager<Node>
{
};

class Scene
{
public:
  Scene();

  Scene(const Scene& other);

  ~Scene();

  Node& addChild();

  template<typename T>
  bool hasSystem()
  {
    return game_systems_.count(typeid(T)) > 0;
  }

  template<typename T>
  GameSystem<T>* getOrCreateSystem()
  {
    std::type_index id = typeid(T);

    if (!game_systems_.count(id))
    {
      game_systems_[id] = std::make_shared<GameSystem<T>>();
    }

    return static_cast<GameSystem<T>*>(game_systems_[id].get());
  }

  template<typename T>
  GameSystem<T>* getSystem() const
  {
    return static_cast<GameSystem<T>*>(game_systems_.at(typeid(T)).get());
  }

  const class Game* game_ = nullptr;

  std::deque<Node*> nodes_;

  NodeManager node_manager_;

  std::map<std::type_index, std::shared_ptr<Updatable>> game_systems_;
};
}

