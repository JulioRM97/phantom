#include "engine.h"
#include "geometries/triangle.h"
#include "graphics/material.h"
#include  "types.h"

#include <stdexcept>
#include <iostream>

Phantom engine;

void createDuckMaterial()
{
  auto& material = engine.asset_manager_.material("blinn3-fx");
  material->createFromFile("duck_vert", ShaderType::k_Vertex);
  material->createFromFile("duck_frag", ShaderType::k_Fragment);

  phantom::UniformBlockInfo infoBlock;

  infoBlock.location = 0;
  infoBlock.name = "mvp";
  infoBlock.type = ShaderType::k_Vertex;
  infoBlock.bytes = sizeof(phantom::MeshUniform);
  material->addUniformBlock(infoBlock);

}

void createLightSettings()
{
  auto& material = engine.asset_manager_.material("blinn3-fx");
  material->createFromFile("dir_light_vert", ShaderType::k_Vertex);
  material->createFromFile("dir_light_frag", ShaderType::k_Fragment);

  phantom::UniformBlockInfo infoBlock;

  infoBlock.location = 0;
  infoBlock.name = "mvp";
  infoBlock.type = ShaderType::k_Vertex;
  infoBlock.bytes = sizeof(phantom::MeshUniform);
  material->addUniformBlock(infoBlock);

  infoBlock.location = 1;
  infoBlock.name = "light";
  infoBlock.type = ShaderType::k_Fragment;
  infoBlock.bytes = sizeof(phantom::LightData);
  material->addUniformBlock(infoBlock);

  phantom::LightData data;
  data.direction = glm::vec3(0.0, 0.0, -1.0f);
  data.intensity = 10.0f;
  data.position = glm::vec3(-100.0, 0.0, 0.0f);
  
  engine.render_manager_->addLight(data, LightType::k_Dir, material);
}

int main() {

  static phantom::Triangle triangle;

  //triangle.load();
 // engine.asset_manager_.loadFromGLTF("Triangle.gltf");
  engine.asset_manager_.loadFromGLTF("Duck.gltf");

  //createDuckMaterial();

  createLightSettings();


  std::shared_ptr<phantom::Mesh> mesh = engine.asset_manager_.mesh("mesh_0");

  std::shared_ptr<phantom::Mesh> duck_mesh = engine.asset_manager_.mesh("LOD3spShape");

  auto duck_material = engine.asset_manager_.material("blinn3-fx");

  //engine.render_manager_->renderMeshWithMaterial(*mesh.get(), triangle.material());
  engine.render_manager_->renderMeshWithMaterial(*duck_mesh.get(), *duck_material.get());


  engine.start();


    try {
      engine.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}