#pragma once

#define GLM_FORCE_RADIANS

#include "graphics/common.h"

#include "glm/mat4x4.hpp"
#include "glm/gtc/type_precision.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include <inttypes.h>
#include <atomic>
#include <functional>

typedef unsigned int uint;
typedef uint8_t ui8;
typedef uint32_t ui32;
typedef int8_t  i8;
typedef int32_t i32;
typedef int64_t i64;
typedef uint16_t Key;
typedef uint8_t Button;

namespace phantom
{
extern struct Buffer Buffer;

struct AssetManagerInfo
{
  //TBI
};


struct AttributeInfo
{
  //void* data = nullptr;
  //Is the vertex identifier, is depends on the other when it is binded.
  ui32 binding = 0;
  //Location inside the shader.
  ui32 location = 0;
  ui32 offset   = 0;
  ui32 stride   = 0;
  Format format = Format::k_UNDEFINED;
  InputVertexRate rate = InputVertexRate::k_Vertex;
  BufferType usage = BufferType::k_None;
};

struct UniformBlockInfo
{
  uint32_t location = 0;
  uint32_t count = 0;
  ShaderType type = ShaderType::k_None;
  size_t bytes = 0;
  std::string name;
};



struct MaterialInfo
{
  AttributeInfo* info = nullptr;
  ui32 numAttributes = 0;
  struct BlendData* blendData;
  //enum phantom::graphics::Material::Type type = graphics::Material::k_None;
  UniformBlockInfo* uniformInfo = nullptr;
  ui32 numUniformBlocks = 0;
  bool instance = false;
};

struct MeshInfo
{
  void* data;
  size_t size;
  //Position in gltf file
  size_t pos = 0;
  size_t binding_pos = -1;
  BufferType usage = BufferType::k_None;
};

namespace graphics
{


}//end namespace graphics

struct GPUDataBase {
  virtual void load() = 0;

  virtual void unload() = 0;

  std::atomic_uint cpu_version_{ 0 };

  uint32_t gpu_version_ = 0;
};

struct CameraInfo
{
  glm::vec3 position = glm::vec3(0.0f);
  glm::mat4 projection = glm::mat4(1.f);
  glm::mat4 view = glm::mat4(1.f);
};


struct BlendData {
  bool enable = false;
  BlendType type = BlendType::k_Add;
  BlendFunc sfactor = BlendFunc::k_One;
  BlendFunc dfactor = BlendFunc::k_One;
};

struct TextureCreateInfo
{
  uint64_t size;
  std::string name;
  int width;
  int height;
  int component;
  void* data;
};

struct CameraSettings
{
  glm::mat4 projection;
  glm::mat4 view;
};

struct Transform
{
  glm::mat4 new_transform;
  glm::mat4 old_transform;

  glm::vec3 pos;
  glm::vec3 scale;
  glm::vec3 rotation;
};

struct SceneData
{

};

struct ClippingSpace
{
  glm::ivec2 offset = glm::ivec2(0);
  glm::u32vec2 extent = glm::u32vec2(0);
};

struct ViewportSettings
{
  glm::vec2 size = glm::vec2(0.0f);
  glm::vec2 pos = glm::vec2(0.0f);
  float minDepth = 0.0f;
  float maxDepth = 1.0f;
};

struct WindowSettings
{
  char name[255];

  glm::ivec2 size;

  ClippingSpace scissor;

};

struct MeshUniform
{
  glm::mat4 model   = glm::mat4(1.0f);
  glm::mat4 view    = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  glm::mat4 project = glm::perspective(0.6605925559997559f, 1.5f, 1.0f, 100000.0f);
};

struct LightData
{
  glm::vec3 position{ 0 };

  float range = 0;

  glm::vec3 camera_pos { 0 };
  float intensity = 1.0f;

  glm::vec3 direction{ 0 };
  float padding_3;
};

struct UniformTest
{
  glm::vec4 color = glm::vec4(1.0f, 1.0f, 0.0f, 0.0f);
};

class Updatable
{
 public:
   virtual void preUpdate() {};
   virtual void update() {};
   virtual void fixedUpdate() {};
   virtual void postUpdate() {};
};


struct rect2i
{
  glm::uvec2 pos;
  glm::uvec2 size;
};


}
