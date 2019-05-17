#pragma once

#ifdef VULKAN_API

#include "vulkan/vulkan.h"

#endif
#include "types.h"
#include "graphics/material.h"

#include <memory>


namespace phantom
{
class Mesh
{

 public:
  Mesh();
  ~Mesh();

  void init();

  void load();
  void unload();

  void addMeshData(MeshInfo& info);

  //If there arent index count use put them, otherwise put index count.
  void set_VertextCount(ui32 count);
  void set_IndexCount(ui32 count);

  void updateUniforms(phantom::graphics::Material& material);

#ifdef VULKAN_API
  //Used with vulkan rendering
  void prepare(VkCommandBuffer& buffers);
  //Buffer pos in gltf file
  int get_bindingPos(int buffer);
#endif

 private:

  struct Data;
  std::shared_ptr<Data> data_;

};

}