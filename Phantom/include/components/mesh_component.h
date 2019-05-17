#pragma once

#include "component.h"
#include "mesh.h"

#include <memory>

namespace phantom
{
class MeshComponent : Component
{

 public:
  explicit MeshComponent(Node& parent);

  ~MeshComponent() override;

  void update() override;

  std::shared_ptr<Mesh> mesh_;

};

} // end namespace phantom
