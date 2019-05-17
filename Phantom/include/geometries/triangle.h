#pragma once

#include "graphics/material.h"
#include "mesh.h"

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

#include <vector>

namespace phantom
{
  
class Triangle
{
 public:
   Triangle();
   ~Triangle();

  struct Vertex
  {
    glm::vec2 pos;
    glm::vec3 color;
  };

  const std::vector<Vertex> vertices = {
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
  };


  void load();
  void unload();
  phantom::graphics::Material& material() { return *mat_; }
  phantom::Mesh& mesh() { return mesh_; }
#ifdef VULKAN_API
  //Used with vulkan rendering
  void prepare(VkCommandBuffer& buffers);
#endif

private:
  phantom::Mesh mesh_;
  phantom::graphics::Material* mat_;

};

}//end namespace phantom