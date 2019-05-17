#pragma once

#include <memory>
#include <vector>
#include <deque>
#include <map>

#include "types.h"

namespace phantom
{
class Mesh;
class CameraComponent;

namespace graphics
{
class Window;
class Material;
class Window;
class Texture;
}

struct DrawInfo
{
  Mesh& mesh;
  graphics::Material& material;
};

class RenderManager
{
  friend class phantom::Mesh;
  friend class phantom::graphics::Material;
  friend class phantom::graphics::Window;
  friend class phantom::graphics::Texture;
 public:
   ~RenderManager();
  
  static RenderManager& instance()
  {
    static RenderManager* gInstance = new RenderManager();

    return *gInstance;
  }

  void init();

  void mainLoop();

  void draw();

  void drawFrame();

  void cleanup();

  void set_DepthValues(float minDepth, float maxDepth = 1.0f);

  void renderMeshWithMaterial(Mesh& mesh, graphics::Material& material);

  void addLight(LightData data, LightType type, std::shared_ptr< phantom::graphics::Material> material);

  inline phantom::graphics::Window& window() { return *window_; }
  inline size_t currentImage() { return currentFrame; }

  size_t numberImages();

  phantom::graphics::Window* window_;

  phantom::CameraComponent* current_camera_ = nullptr;

 private:
   RenderManager();

   void createInstance();
   void setupDebugMessenger();
   void createSurface();
   void pickPhysicalDevice();
   void createLogicalDevice();
   void createSwapChain();
   void createImageViews();
   void createRenderPass();
   void createGraphicsPipeline();
   void createFramebuffers();
   void createCommandPool();
   void createCommandBuffers();
   void createSyncObjects();
   void createDescriptorPool();
   void loadObjects();
   void updateUniformBlocks();
   void createDepthTexture();

   std::vector<const char*> getRequiredExtensions();

   
   struct Data;
   std::unique_ptr<Data> data_;

   std::map<LightType, std::deque<LightData>> lights_;

   std::map<LightType, std::shared_ptr< phantom::graphics::Material>> LightMaterials_;

   std::deque<DrawInfo> objectsToDraw_;

   size_t currentFrame = 0;
};

}//end namespace phantom