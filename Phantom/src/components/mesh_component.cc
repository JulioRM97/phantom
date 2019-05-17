#include "components/mesh_component.h"

#include "components/camera_component.h"
#include "node.h"


namespace phantom
{

  ////////////////////////////////////////////////////////////////
  MeshComponent::MeshComponent(Node& parent)
    : Component(parent)
  {
  }

  ////////////////////////////////////////////////////////////////
  MeshComponent::~MeshComponent() = default;


  void MeshComponent::update()
  {

  }

} // end namespace phantom