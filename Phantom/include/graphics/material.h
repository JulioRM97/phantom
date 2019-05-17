#pragma once

#include "types.h"
#include "graphics/texture.h"

#include "glm/glm.hpp"

#include <string>
#include <memory>

#include "vulkan/vulkan.h"

namespace phantom
{
namespace graphics
{

class Material 
{

struct Data;

public:

  enum Type : ui8
  {
    k_Opaque = 0,
    k_Transparent,
    k_UI,
    k_Pos,
    k_None
  };

  Material() = default;
  Material(Type type, std::string name);
  virtual ~Material();


  //void use(uint render_location = 0);

  void createFromFile(std::string filename, ShaderType type);

  void createFromCode(std::string code,     ShaderType type);

  void enableMultisampling(bool enable, uint count);

  void enableBlending(BlendData data);
  //Add attribute to the material, when u call this function it will imply all 
  //the attributes are in the same buffer
  void addAttribute(AttributeInfo* info, ui32 num);

  void addUniformBlock(UniformBlockInfo& info);

  void addTexture(std::shared_ptr<Texture> texture);

  //TODO add texture to material.

  virtual void load();
  virtual void unload();

  void updateUniformBlock(std::string name, void* data, size_t bytes);

  void updateUniformBlocks();

#ifdef VULKAN_API
  //Used with vulkan rendering
  void prepare(VkCommandBuffer& buffers);
#endif
  /*
  Type type() const;

  //void addOutput(Texture::Type type, std::shared_ptr<Texture>& out, uint render_location = 0);

  virtual void set_MaterialType(Material::Type type);
  virtual void set_UniformData(float data, std::string name);
  virtual void set_UniformData(int data, std::string name);
  virtual void set_UniformData(uint data, std::string name);
  virtual void set_UniformData(glm::vec3 data, std::string name);
  virtual void set_UniformData(glm::vec2 data, std::string name);
  virtual void set_UniformData(glm::mat4 data, std::string name);
  virtual void set_CameraInfo(CameraInfo& info, std::string name);
  virtual void setupAttachments(uint location = 0);
  virtual void addTexture(std::shared_ptr<TextureAttachment> tex, std::string name);*/

  std::unique_ptr<Data> data_;

private:

  
  
  

  //void bindTextures();

};

}
}