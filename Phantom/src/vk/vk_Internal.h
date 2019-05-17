#pragma once

#include "vulkan/vulkan.h"

#include "types.h"
#include "render_manager.h"
#include "asset_manager.h"
#include "mesh.h"
#include "graphics/material.h"
#include "graphics/texture.h"
#include "graphics/window.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <optional>
#include <set>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <deque>

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

typedef VkShaderModule Shader;
typedef VkSurfaceKHR Surface;
typedef VkPipelineShaderStageCreateInfo ShaderInfo;

const std::vector<const char*> validationLayers = {
    "VK_LAYER_LUNARG_standard_validation"
};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#define checkDevice(n, g) _checkDevice(n, g)
#define checkSwapChain(n, g) _checkSwapChainSupport(n, g);
#define chooseSwapSurfaceFormat(n) _chooseSwapSurfaceFormat(n)
#define chooseSwapPresentMode(n) _chooseSwapPresentMode(n)
#define chooseSwapExtent(n, w) _chooseSwapExtent(n, w)
#define createShader(c, d) _createShaderModule (c, d)
#define createShaderFromFile(name, d) _createShaderFromFile(name, d)
#define createDefaultRenderPass() _createDefaultRenderPass()
#define createImage(d, pd, w, h, f, t, u, p, i, im) _createImage(d, pd, w, h, f, t, u, p, i, im)
#define createImageView(d, i, f, af) _createImageView(d, i, f, af)
#define VkShaderType(t) _vkShaderType(t)
#define findMemoryType(d, t, f) _vkFindMemoryType(d, t, f)
#define bufferType(t) _vkGetBufferType(t)
#define createBuffer(fd, d, s, u, p, b, m) _createBuffer(fd, d, s, u, p, b, m)
#define beginSingleTimeCommand(d, cp) _beginSingleTimeCommand(d, cp)
#define endSingleTimeCommand(d, cp, gq, cb) _endSingleTimeCommand(d, cp, gq, cb)

const int MAX_FRAMES_IN_FLIGHT = 2;

namespace phantom
{


struct RenderManager::Data
{
 
  VkInstance instance;
  VkDebugUtilsMessengerEXT debugMessenger;

  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  VkDevice device;

  VkQueue graphicsQueue;
  VkQueue presentQueue;

  VkSwapchainKHR swapChain;
  std::vector<VkImage> swapChainImages;
  VkFormat swapChainImageFormat;
  VkExtent2D swapChainExtent;
  std::vector<VkImageView> swapChainImageViews;
  std::vector<VkFramebuffer> swapChainFramebuffers;

  VkRenderPass renderPass;
  VkPipelineLayout pipelineLayout;
  VkPipeline graphicsPipeline;

  VkCommandPool commandPool;
  std::vector<VkCommandBuffer> commandBuffers;

  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;
  std::vector<VkFence> inFlightFences;

  VkImage depthImage;
  VkDeviceMemory depthImageMemory;
  VkImageView depthImageView;

  VkDescriptorPool descriptorPool;


};
//////////////////////////////////////
struct Mesh::Data : GPUDataBase 
{
  //TBI
  //ubo
  //bao
  //vao
  void load() override;
  void unload() override;

  struct Vertex
  {
    VkBuffer buffer;
    VkDeviceMemory memory;
  };

  std::vector<Vertex> vertexAttributeBuffers;
  std::vector<VkBuffer> buffers;
  std::deque<MeshInfo> meshInfos;

  std::map<int, MeshInfo> infos;

  VkBuffer indexBuffer;
  bool hasIndexBuffer = false;

  ui32 instanceCount = 1;
  ui32 vertextCount = 0;
  ui32 indexCount = 0;
  ui32 firstInstance = 0;
  ui32 firstVertex = 0;
};
////////////////////////////////////////

namespace graphics
{

////////////////////////////////////////
struct Vertex
{
  glm::vec3 pos;
  glm::vec3 normal;
  glm::vec2 texCoord;
};
////////////////////////////////////////
struct Window::Data
{
  GLFWwindow* window = nullptr;
  VkSurfaceKHR surface;

  glm::ivec2 size;
  glm::dvec2 mouse_pos;
  glm::dvec2 mouse_axis;
};
////////////////////////////////////////
static Surface surface()
{
  return RenderManager::instance().window().data_->surface;
}
////////////////////////////////////////
struct Material::Data : GPUDataBase {

  Data() : GPUDataBase()
  {
    uniformBlocks.reserve(MAX_UNIFORM_BLOCKS);
  }

  struct UniformBlock
  {
    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    size_t bytes = 0;
    size_t location = 0;
  };
  Type type = k_Opaque;

  std::string name;

  CameraInfo cameraInfo;

  std::map<ShaderType, std::deque<std::string>> shaderFromFile;
  std::map<ShaderType, std::deque<std::string>> shaderFromCode;

  std::vector<Shader> shadersCreated;
  std::vector<ShaderInfo> shadersInfo;
  std::map<int, std::deque<AttributeInfo>> attributesInfo;
  std::vector<VkVertexInputBindingDescription> bindingDescriptions;
  std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
  std::vector<VkDescriptorSetLayoutBinding> bindings;
  ui32 numVertexBuffers = 0;
  ui32 numAttributes = 0;

  std::map<std::string, UniformBlock> uniforms;
  std::deque<std::shared_ptr<Texture>> textures;
  std::vector<UniformBlockInfo> uniformBlocks;
  std::vector<VkDescriptorSet> descriptorSets;
  VkDescriptorSetLayout descriptorSetLayout;
  VkDescriptorPool descriptorPool;

  uint stages = 0;

  VkPipelineLayout pipelineLayout;
  VkPipeline graphicsPipeline;
  VkRenderPass renderPass;



  void load() override;
  void unload() override;

  void createUniformBuffers();

  void createDescriptorSets();

  void createDescriptorPool();
};
////////////////////////////////////////
struct Texture::Data : GPUDataBase
{

  void load() override;
  void unload() override;

  VkImage textureImage;
  VkImageLayout imageLayout;
  VkImageView view;

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  VkDeviceMemory textureImageMemory;

  VkDescriptorImageInfo descriptor;

  VkSampler textureSampler;
  VkFilter magFilter = VK_FILTER_NEAREST;
  VkFilter minFilter = VK_FILTER_NEAREST;
  VkSamplerAddressMode addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  VkSamplerAddressMode addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  VkSamplerAddressMode addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

  uint32_t width = 32;
  uint32_t height = 32;
  uint32_t mipLevels = 1;
  uint32_t layerCount = 1;

  TextureCreateInfo info;

  void createTextureImage();

  void createView();

  void createSampler();
};


}//end namespace graphics

}//end namespace phantom

static std::vector<char> readFile(const std::string& filename) {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("failed to open file!");
  }

  size_t fileSize = (size_t)file.tellg();
  std::vector<char> buffer(fileSize);

  file.seekg(0);
  file.read(buffer.data(), fileSize);

  file.close();

  return buffer;
}
/////////////////////////////////////////////////////////////////////
struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  bool isComplete() {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};
/////////////////////////////////////////////////////////////////////
struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};
/////////////////////////////////////////////////////////////////////
static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
  if (func != nullptr) {
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  }
  else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}
/////////////////////////////////////////////////////////////////////
static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != nullptr) {
    func(instance, debugMessenger, pAllocator);
  }
}
/////////////////////////////////////////////////////////////////////
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
  std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

  return VK_FALSE;
}
/////////////////////////////////////////////////////////////////////
static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
{
  QueueFamilyIndices indices;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

  int i = 0;
  for (const auto& queueFamily : queueFamilies) {
    if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsFamily = i;
    }

    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

    if (queueFamily.queueCount > 0 && presentSupport) {
      indices.presentFamily = i;
    }

    if (indices.isComplete()) {
      break;
    }

    i++;
  }

  return indices;
}
/////////////////////////////////////////////////////////////////////
static bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
  uint32_t extensionCount;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

  std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

  for (const auto& extension : availableExtensions) {
    requiredExtensions.erase(extension.extensionName);
  }

  return requiredExtensions.empty();
}
/////////////////////////////////////////////////////////////////////
static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
  SwapChainSupportDetails details;

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

  if (formatCount != 0) {
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
  }

  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

  if (presentModeCount != 0) {
    details.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
  }

  return details;
}
/////////////////////////////////////////////////////////////////////
static bool _checkDevice(VkPhysicalDevice device, VkSurfaceKHR surface)
{
  QueueFamilyIndices indices = findQueueFamilies(device, surface);

  bool extensionsSupported = checkDeviceExtensionSupport(device);

  bool swapChainAdequate = false;
  if (extensionsSupported) {
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device, surface);
    swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
  }

  return indices.isComplete() && extensionsSupported && swapChainAdequate;
}
/////////////////////////////////////////////////////////////////////
static SwapChainSupportDetails _checkSwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
{
  SwapChainSupportDetails details;

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

  if (formatCount != 0) {
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
  }

  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

  if (presentModeCount != 0) {
    details.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
  }

  return details;
}
/////////////////////////////////////////////////////////////////////
static VkSurfaceFormatKHR _chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
  if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) {
    return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
  }

  for (const auto& availableFormat : availableFormats) {
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return availableFormat;
    }
  }

  return availableFormats[0];
}
/////////////////////////////////////////////////////////////////////
static VkPresentModeKHR _chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes) {
  VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

  for (const auto& availablePresentMode : availablePresentModes) {
    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
      return availablePresentMode;
    }
    else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
      bestMode = availablePresentMode;
    }
  }

  return bestMode;
}
/////////////////////////////////////////////////////////////////////
static VkExtent2D _chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window) {
  if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  }
  else {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    VkExtent2D actualExtent = {
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height)
    };

    actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
    actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

    return actualExtent;
  }
}
/////////////////////////////////////////////////////////////////////
static Shader _createShaderModule(const std::vector<char>& code, VkDevice device) {
  VkShaderModuleCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size();
  createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

  VkShaderModule shaderModule;
  if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
    throw std::runtime_error("failed to create shader module!");
  }

  return shaderModule;
}
/////////////////////////////////////////////////////////////////////
static Shader _createShaderFromFile(const std::string& filename, VkDevice device)
{
  auto code = readFile(filename + ".spv");

  return createShader(code, device);
}
/////////////////////////////////////////////////////////////////////
static VkShaderStageFlagBits _vkShaderType(ShaderType type)
{
  switch (type)
  {
  case ShaderType::k_None:
    break;
  case ShaderType::k_Fragment:
    return VK_SHADER_STAGE_FRAGMENT_BIT;
    break;
  case ShaderType::k_Vertex:
    return VK_SHADER_STAGE_VERTEX_BIT;
    break;
  case ShaderType::k_Geometry:
    return VK_SHADER_STAGE_GEOMETRY_BIT;
    break;
  case ShaderType::k_Compute:
    return VK_SHADER_STAGE_COMPUTE_BIT;
    break;
  default:
    return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
    break;
  }

  return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
}
/////////////////////////////////////////////////////////////
static VkRenderPass _createDefaultRenderPass()
{
  VkRenderPass renderPass;

  //TBI
  return renderPass;
}
/////////////////////////////////////////////////////////////
static ui32 _vkFindMemoryType(VkPhysicalDevice physicalDevice, ui32 typeFilter, VkMemoryPropertyFlags properties)
{
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
      return i;
    }
  }

  throw std::runtime_error("failed to find suitable memory type!");
}
/////////////////////////////////////////////////////////////
static VkBufferUsageFlagBits _vkGetBufferType(BufferType type)
{
  switch (type)
  {
  case BufferType::k_None:
    return VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
    break;
  case BufferType::k_Index:
    return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    break;
  case BufferType::k_Vertex:
    return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    break;
  case BufferType::k_Uniform:
    return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    break;
  case BufferType::k_Storage:
    return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    break;
  default:
    return VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
    break;
  }

  return VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
}
/////////////////////////////////////////////////////////////
static void  _createBuffer(VkPhysicalDevice fdevice, VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
  VkBufferCreateInfo bufferInfo = {};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to create buffer!");
  }

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

  VkMemoryAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = findMemoryType(fdevice, memRequirements.memoryTypeBits, properties);

  if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate buffer memory!");
  }

  vkBindBufferMemory(device, buffer, bufferMemory, 0);
}
/////////////////////////////////////////////////////////////
static VkCommandBuffer  _beginSingleTimeCommand(VkDevice device, VkCommandPool commandPool)
{
  VkCommandBufferAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = commandPool;
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

  VkCommandBufferBeginInfo beginInfo = {};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(commandBuffer, &beginInfo);

  return commandBuffer;
}
/////////////////////////////////////////////////////////////
static void _endSingleTimeCommand(VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkCommandBuffer commandBuffer)
{
  vkEndCommandBuffer(commandBuffer);

  VkSubmitInfo submitInfo = {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(graphicsQueue);

  vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}
/////////////////////////////////////////////////////////////
static bool hasStencilComponent(VkFormat format) 
{
  return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}
/////////////////////////////////////////////////////////////
static void transitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {

  VkImageMemoryBarrier barrier = {};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.oldLayout = oldLayout;
  barrier.newLayout = newLayout;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = image;

  if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

    if (hasStencilComponent(format)) {
      barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }
  }
  else {
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  }

  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = 1;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;

  VkPipelineStageFlags sourceStage;
  VkPipelineStageFlags destinationStage;

  if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  }
  else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  }
  else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  }
  else {
    throw std::invalid_argument("unsupported layout transition!");
  }

  vkCmdPipelineBarrier(
    commandBuffer,
    sourceStage, destinationStage,
    0,
    0, nullptr,
    0, nullptr,
    1, &barrier
  );

}
/////////////////////////////////////////////////////////////
static void copyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {

  VkBufferImageCopy region = {};
  region.bufferOffset = 0;
  region.bufferRowLength = 0;
  region.bufferImageHeight = 0;
  region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount = 1;
  region.imageOffset = { 0, 0, 0 };
  region.imageExtent = {
      width,
      height,
      1
  };

  vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

}
////////////////////////////////////////
static void _createImage(VkDevice device, VkPhysicalDevice physicalDevice, int32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
{
  VkImageCreateInfo imageInfo = {};
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.imageType = VK_IMAGE_TYPE_2D;
  imageInfo.extent.width = width;
  imageInfo.extent.height = height;
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = 1;
  imageInfo.arrayLayers = 1;
  imageInfo.format = format;
  imageInfo.tiling = tiling;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageInfo.usage = usage;
  imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
    throw std::runtime_error("failed to create image!");
  }

  VkMemoryRequirements memRequirements;
  vkGetImageMemoryRequirements(device, image, &memRequirements);

  VkMemoryAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

  if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate image memory!");
  }

  vkBindImageMemory(device, image, imageMemory, 0);
}
////////////////////////////////////////
static VkImageView  _createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
  VkImageViewCreateInfo viewInfo = {};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image = image;
  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format = format;
  viewInfo.subresourceRange.aspectMask = aspectFlags;
  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.levelCount = 1;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = 1;

  VkImageView imageView;
  if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
    throw std::runtime_error("failed to create texture image view!");
  }

  return imageView;
}
