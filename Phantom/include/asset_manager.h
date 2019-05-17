#pragma once

#include "types.h"

#include <string>
#include <map>
#include "tiny_gltf.h"


namespace phantom
{
class Mesh;

namespace graphics
{
class Material;
class Texture;
}

class AssetManager
{
public:

  AssetManager();
  ~AssetManager();

  void loadFromGLTF(std::string file_name);

  //void loadFromObj(std::string file_name, std::string name);

  std::shared_ptr<Mesh> mesh(std::string name);

  std::shared_ptr<graphics::Material> material(std::string name);

  void set_info(AssetManagerInfo& info);

  std::shared_ptr<Mesh> createMesh(MeshInfo& info, std::string name);

  std::shared_ptr<graphics::Material> createMaterial(MaterialInfo& info, std::string name);

private:
  std::map<std::string, std::shared_ptr<Mesh>> meshes_;

  std::map<std::string, std::shared_ptr<graphics::Material>> materials_;

  std::map<std::string, std::shared_ptr<graphics::Texture>> textures_;

  void createMeshInfos();

  void createMeshes();

  void createMaterial(std::shared_ptr<graphics::Material> material, const tinygltf::Material& gltfMaterial);

  struct Data;
  std::unique_ptr<Data> data_;
};

}

