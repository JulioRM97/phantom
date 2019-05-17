#pragma once

#include <memory>

#include "graphics/common.h"
#include "types.h"

namespace phantom
{
namespace graphics
{


class Texture
{
  friend class Material;

 public:
  explicit Texture(TextureCreateInfo info);
  ~Texture();

  void load();
  void unload();

  void set_TextureMode(TextureMode modeU, TextureMode modeV, TextureMode modeW);

  void set_TextureFilter(TextureFilter min, TextureFilter mag);

  void set_Size(uint32_t width, uint32_t height);
 private:
  struct Data;
  std::unique_ptr<Data> data_;
};

}
}