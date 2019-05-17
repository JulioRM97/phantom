#include "asset_manager.h"

#include "mesh.h"
#include "graphics/material.h"
#include "graphics/texture.h"

#include "vk_Internal.h"

#define STBI_MSC_SECURE_CRT
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "tiny_gltf.h"

#define GLTFType(t) _gltfTypeToFormat(t)
#define GLTFCount(t)_gltfCount(t)
#define getTextureFilter(f) _getTextureFilter(f)
#define getTextureWrapMode(m) _getTextureWrapMode(m)

static Format _gltfTypeToFormat(int gltfType)
{
  switch (gltfType)
  {
  case TINYGLTF_TYPE_SCALAR:
    return Format::k_R32_SFLOAT;
    break;
  case TINYGLTF_TYPE_VEC2:
    return Format::k_R32G32_SFLOAT;
    break;
  case TINYGLTF_TYPE_VEC3:
    return Format::k_R32G32B32_SFLOAT;
    break;
  case TINYGLTF_TYPE_VEC4:
    return Format::k_R32G32B32A32_SFLOAT;
    break;
  default:
    return Format::k_UNDEFINED;
    break;
  }
}
///////////////////////////////////////////////////////////////
static int _gltfCount(int gltfType)
{

  switch (gltfType)
  {
  case TINYGLTF_TYPE_SCALAR:
    return 1;
    break;
  case TINYGLTF_TYPE_VEC2:
    return 2;
    break;
  case TINYGLTF_TYPE_VEC3:
    return 3;
    break;
  case TINYGLTF_TYPE_VEC4:
    return 4;
    break;
  default:
    return 5;
    break;
  }
  return 0;
}
///////////////////////////////////////////////////////////////
static TextureFilter _getTextureFilter(int gltfTextureFilter)
{
  switch (gltfTextureFilter)
  {
  case TINYGLTF_TEXTURE_FILTER_NEAREST:
    return TextureFilter::k_NEAREST;
    break;

  case TINYGLTF_TEXTURE_FILTER_LINEAR:
    return TextureFilter::k_LINEAR;
    break;

  default:
    return TextureFilter::k_NEAREST;
    break;
  }
}
///////////////////////////////////////////////////////////////
static TextureMode _getTextureWrapMode(int gltfWrapMode)
{
  switch (gltfWrapMode)
  {
  case TINYGLTF_TEXTURE_WRAP_REPEAT:
    return TextureMode::k_MODE_REPEAT;
    break;

  case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE:
    return TextureMode::k_MODE_CLAMP_TO_EDGE;
    break;

  case TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT:
    return TextureMode::k_MODE_MIRRORED_REPEAT;
    break;

  default:
    return TextureMode::k_MODE_CLAMP_TO_BORDER;
    break;
  }
}
///////////////////////////////////////////////////////////////
namespace phantom
{
///////////////////////////////////////////////////////////////
struct AssetManager::Data
{
  tinygltf::Model model;

  std::map<std::string, std::shared_ptr<phantom::Mesh>> meshes;

  std::map<std::string, std::shared_ptr<phantom::graphics::Material>> materials;
};
///////////////////////////////////////////////////////////////
AssetManager::AssetManager()
  : data_(new Data())
{

}
///////////////////////////////////////////////////////////////
AssetManager::~AssetManager()
{

}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
void AssetManager::loadFromGLTF(std::string file_name)
{
  tinygltf::TinyGLTF loader;
  std::string err;
  std::string warn;

  bool res = loader.LoadASCIIFromFile(&data_->model, &err, &warn, file_name);

  if (!warn.empty()) {
    std::cout << "WARN: " << warn << std::endl;
  }

  if (!err.empty()) {
    std::cout << "ERR: " << err << std::endl;
  }

  if (!res)
    std::cout << "Failed to load glTF: " << file_name << std::endl;
  else
    std::cout << "Loaded glTF: " << file_name << std::endl;

  createMeshInfos(); //empty


  createMeshes();
}

///////////////////////////////////////////////////////////////
std::shared_ptr<phantom::Mesh> AssetManager::mesh(std::string name)
{
  return meshes_[name];
}

///////////////////////////////////////////////////////////////
std::shared_ptr<phantom::graphics::Material> AssetManager::material(std::string name)
{
  return materials_[name];
}

///////////////////////////////////////////////////////////////
void AssetManager::set_info(AssetManagerInfo& info)
{

}

///////////////////////////////////////////////////////////////
std::shared_ptr<phantom::Mesh> AssetManager::createMesh(MeshInfo& info, std::string name)
{
  //TBI
  return std::make_shared<phantom::Mesh>();
}

///////////////////////////////////////////////////////////////
std::shared_ptr<phantom::graphics::Material> AssetManager::createMaterial(MaterialInfo& info, std::string name)
{
  return std::make_shared<phantom::graphics::Material>();
}
///////////////////////////////////////////////////////////////
void AssetManager::createMeshInfos()
{

}
///////////////////////////////////////////////////////////////
void AssetManager::createMeshes()
{
  //TBI
  tinygltf::Model &model = data_->model;

  for (size_t i = 0; i < model.meshes.size (); ++i)
  {
    std::shared_ptr<phantom::Mesh> newMesh = std::make_shared<phantom::Mesh>();

    //Generate mesh
    const tinygltf::Mesh &mesh = model.meshes[i];

   


    //Generate materials
    for (auto primitive : mesh.primitives)
    {

      std::shared_ptr<graphics::Material> mat;
      if (primitive.material != -1)
      {
        const tinygltf::Material &gltf_mat = model.materials[primitive.material];

        std::string mat_name = gltf_mat.name.empty() ? std::to_string(materials_.size()) : gltf_mat.name;

        mat = std::make_shared<graphics::Material>(graphics::Material::Type::k_Opaque, mat_name);

        if (gltf_mat.name.empty())
        {
          std::cout << "WARN: please insert material names" << std::endl;

          materials_.emplace(mat_name, mat);

        }
        else
        {
          materials_.emplace(gltf_mat.name, mat);
        }

        mat = materials_[mat_name];

        createMaterial(mat, gltf_mat);

      }
      else
      {
        std::string mat_name = "default_" + std::to_string(materials_.size());

        mat = std::make_shared<graphics::Material>(graphics::Material::Type::k_Opaque, mat_name);

        materials_[mat_name] = mat;
      }


      //Create index buffer
      if(primitive.indices != -1)
      {
        MeshInfo info_indices;
        const tinygltf::Accessor &accessor = model.accessors[primitive.indices];

        const tinygltf::BufferView& buffer_view = model.bufferViews[accessor.bufferView];

        const tinygltf::Buffer& buffer = model.buffers[buffer_view.buffer];


        info_indices.usage = BufferType::k_Index;
        info_indices.data = (void*)(buffer.data.data() + buffer_view.byteOffset);
        info_indices.size = buffer_view.byteLength;
        info_indices.pos = -2;

        newMesh->addMeshData(info_indices);
        newMesh->set_IndexCount(accessor.count);
      }
      
      int location = 0;
      for (auto& attribute : primitive.attributes)
      {

        const tinygltf::Accessor &accessor = model.accessors[attribute.second];

        const tinygltf::BufferView& buffer_view = model.bufferViews[accessor.bufferView];

        const tinygltf::Buffer& buffer = model.buffers[buffer_view.buffer];


        MeshInfo meshInfo;

        meshInfo.usage = BufferType::k_Vertex;
        meshInfo.data = (void*)(buffer.data.data () + buffer_view.byteOffset + accessor.byteOffset);
        meshInfo.pos = buffer_view.buffer;
        meshInfo.binding_pos = location;
        meshInfo.size = GLTFCount(accessor.type) * sizeof(float) * accessor.count;

        newMesh->addMeshData(meshInfo);

        AttributeInfo info;
        
        info.binding = location;
        info.location = location;
        info.format = GLTFType(accessor.type);
        info.offset = 0;
        info.stride = buffer_view.byteStride;
        info.rate = InputVertexRate::k_Vertex;

        mat->addAttribute(&info, 1);

        location++;
      }

      if (mesh.name.empty())
      {
        std::cout << "WARN: please insert mesh names" << std::endl;

        std::string name = std::string("mesh_") + std::to_string(meshes_.size());
        meshes_.emplace(name, newMesh);
      }
      else
      {
        meshes_.emplace(mesh.name, newMesh);
      }


    }

    //tinygltf::Buffer buffer = model.buffers[bufferView.buffer];
    //std::cout << "bufferview.target " << bufferView.target << std::endl;



  }
}
///////////////////////////////////////////////////////////////
void AssetManager::createMaterial(std::shared_ptr<graphics::Material> material, const tinygltf::Material& gltfMaterial)
{
  tinygltf::Model &model = data_->model;

  for (auto mat : gltfMaterial.values)
  {
    if (mat.first.find("Texture") != std::string::npos)
    {
      const tinygltf::Texture& gltfTexture = model.textures[mat.second.json_double_value["index"]];

      std::string texture_name;

      if (gltfTexture.name.empty())
      {
        texture_name = "texture_" + std::to_string(mat.second.json_double_value["index"]);
      }
      else
      {
        texture_name = gltfTexture.name;
      }

      std::shared_ptr<phantom::graphics::Texture> texture_to_add;

      if (textures_.count(texture_name) <= 0)
      {
        const tinygltf::Sampler& sampler = model.samplers[gltfTexture.sampler];
        const tinygltf::Image& image = model.images[gltfTexture.source];


        TextureCreateInfo info;
        info.data = (void*)image.image.data();
        info.size = image.image.size();
        info.component = image.component;
        info.height = image.height;
        info.width = image.width;

        texture_to_add = std::make_shared<phantom::graphics::Texture>(info);
        
        texture_to_add->set_TextureFilter(getTextureFilter(sampler.minFilter), getTextureFilter(sampler.magFilter));
        texture_to_add->set_TextureMode(getTextureWrapMode(sampler.wrapS),
                                        getTextureWrapMode(sampler.wrapT), 
                                        getTextureWrapMode(sampler.wrapR));
        texture_to_add->set_Size(info.width, info.height);
      }
      else
      {
        texture_to_add = textures_[texture_name];
      }

     

      material->addTexture(texture_to_add);
    }
    
  }
}

}