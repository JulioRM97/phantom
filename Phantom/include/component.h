#pragma once



#include <types.h>

namespace phantom
{

class Node;

class Component : public Updatable
{
public:
  explicit Component(Node& parent) : owner_node_(parent) {};

  virtual ~Component() = default;

  virtual void editorSettings() {};

  Node& owner_node_;
};

} // end namespace phantom