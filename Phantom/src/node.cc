#include "node.h"

#include "scene.h"

namespace phantom
{

////////////////////////////////////////////////////////////////
Node::Node(Scene* scene, Node* parent)
  : scene_(scene),
  parent_(parent)
{
  createComponent<TransformComponent>();
}

////////////////////////////////////////////////////////////////
Node::Node(const Node& other) = default;

////////////////////////////////////////////////////////////////
Node::~Node() = default;

////////////////////////////////////////////////////////////////
Node& Node::addChild()
{
  auto& node = scene_->node_manager_.emplace(this, scene_, this);
  children_.push_back(&node);
  return node;
}

} // end namespace phantom