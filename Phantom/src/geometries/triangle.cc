#include "geometries/triangle.h"

namespace phantom
{
///////////////////////////////////////////
Triangle::Triangle()
{
}

///////////////////////////////////////////
Triangle::~Triangle()
{

}

///////////////////////////////////////////
void Triangle::load()
{
  MeshInfo aux;
  aux.data = (void*)vertices.data();
  aux.size = vertices.size() * sizeof(Vertex);
  aux.usage = BufferType::k_Vertex;

  mesh_.addMeshData(aux);
  mesh_.set_VertextCount(vertices.size());
  //mesh_.load();

  //////----------------->>
  mat_ = new phantom::graphics::Material(phantom::graphics::Material::Type::k_Opaque, "triangle");

  mat_->createFromFile("hello_triangle_vert", ShaderType::k_Vertex);
  mat_->createFromFile("hello_triangle_frag", ShaderType::k_Fragment);

  AttributeInfo info[2];

  info[0].binding = 0;
  info[0].location = 0;
  info[0].format = Format::k_R32G32B32_SFLOAT;
  info[0].offset = 0;
  info[0].stride = 12;
  info[0].rate = InputVertexRate::k_Vertex;

  UniformBlockInfo infoBlock;

  infoBlock.location = 0;
  infoBlock.name = "test";
  infoBlock.type = ShaderType::k_Vertex;
  infoBlock.bytes = sizeof(UniformTest);
  mat_->addUniformBlock(infoBlock);

  infoBlock.location = 1;
  infoBlock.name = "mio";
  infoBlock.type = ShaderType::k_Vertex;
  infoBlock.bytes = sizeof(UniformTest);
  mat_->addUniformBlock(infoBlock);

  //info[0].binding = 0;
  //info[0].location = 0;
  //info[0].format = Format::k_R32G32_SFLOAT;
  //info[0].offset = offsetof(Vertex, pos);
  //info[0].stride = sizeof(Vertex);
  //info[0].rate = InputVertexRate::k_Vertex;

  //info[1].binding = 0;
  //info[1].location = 1;
  //info[1].format = Format::k_R32G32B32_SFLOAT;
  //info[1].offset = offsetof(Vertex, color);
  //info[1].stride = sizeof(Vertex);
  //info[1].rate = InputVertexRate::k_Vertex;

  mat_->addAttribute(info, 1);
  //mat_->load();
}

///////////////////////////////////////////
void Triangle::unload()
{
  mesh_.unload();
  mat_->unload();
}

///////////////////////////////////////////
void Triangle::prepare(VkCommandBuffer& buffers)
{
  mat_->prepare(buffers);
  mesh_.prepare(buffers);
}

}