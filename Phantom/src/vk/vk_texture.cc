#include "graphics/texture.h"

#include "vk_Internal.h"

namespace phantom
{
namespace graphics
{

///////////////////////////////////////
void Texture::Data::createTextureImage()
{
  auto& rm = RenderManager::instance();

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;

  createBuffer(rm.data_->physicalDevice,
               rm.data_->device,
               info.size,
               VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
               stagingBuffer, 
               stagingBufferMemory);

  void* data;
  vkMapMemory(rm.data_->device, stagingBufferMemory, 0, info.size, 0, &data);
    memcpy(data, info.data, static_cast<size_t>(info.size));
  vkUnmapMemory(rm.data_->device, stagingBufferMemory);

  //Image creation
  {
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = static_cast<uint32_t>(width);
    imageInfo.extent.height = static_cast<uint32_t>(height);
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = mipLevels;
    imageInfo.arrayLayers = 1;
    imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.flags = 0; // Optional

    if (vkCreateImage(rm.data_->device, &imageInfo, nullptr, &textureImage) != VK_SUCCESS) {
      throw std::runtime_error("failed to create image!");
    }


    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(rm.data_->device, textureImage, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(RenderManager::instance().data_->physicalDevice,
      memRequirements.memoryTypeBits,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (vkAllocateMemory(rm.data_->device, &allocInfo, nullptr, &textureImageMemory) != VK_SUCCESS) {
      throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(rm.data_->device, textureImage, textureImageMemory, 0);
  }

  VkCommandBuffer buffer = beginSingleTimeCommand(rm.data_->device, rm.data_->commandPool);
  transitionImageLayout(buffer, textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  endSingleTimeCommand(rm.data_->device, rm.data_->commandPool, rm.data_->graphicsQueue, buffer);

  buffer = beginSingleTimeCommand(rm.data_->device, rm.data_->commandPool);
  copyBufferToImage(buffer, stagingBuffer, textureImage, static_cast<uint32_t>(width), static_cast<uint32_t>(height));
  endSingleTimeCommand(rm.data_->device, rm.data_->commandPool, rm.data_->graphicsQueue, buffer);

  buffer = beginSingleTimeCommand(rm.data_->device, rm.data_->commandPool);
  transitionImageLayout(buffer, textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
  endSingleTimeCommand(rm.data_->device, rm.data_->commandPool, rm.data_->graphicsQueue, buffer);

  vkDestroyBuffer(rm.data_->device, stagingBuffer, nullptr);
  vkFreeMemory(rm.data_->device, stagingBufferMemory, nullptr);
}
///////////////////////////////////////
void Texture::Data::createView()
{
  VkImageViewCreateInfo viewInfo = {};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image = textureImage;
  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
  viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.levelCount = 1;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = 1;

  if (vkCreateImageView(RenderManager::instance ().data_->device, &viewInfo, nullptr, &view) != VK_SUCCESS) {
    throw std::runtime_error("failed to create texture image view!");
  }

}
///////////////////////////////////////
void Texture::Data::createSampler()
{
  VkSamplerCreateInfo samplerInfo = {};
  samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerInfo.magFilter = magFilter;
  samplerInfo.minFilter = minFilter;
  samplerInfo.addressModeU = addressModeU;
  samplerInfo.addressModeV = addressModeV;
  samplerInfo.addressModeW = addressModeW;
  samplerInfo.anisotropyEnable = VK_FALSE;
  samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  samplerInfo.unnormalizedCoordinates = VK_FALSE;
  samplerInfo.compareEnable = VK_FALSE;
  samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
  samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

  if (vkCreateSampler(RenderManager::instance ().data_->device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
    throw std::runtime_error("failed to create texture sampler!");
  }
}
///////////////////////////////////////
void Texture::Data::load()
{

  if (cpu_version_ <= gpu_version_) {
    // Material is up to date... do not update
    return;
  }

  uint32_t next_version = cpu_version_;

  auto& rm = RenderManager::instance();

  createTextureImage();

  createView();

  createSampler();

  gpu_version_ = next_version;
}
///////////////////////////////////////
void Texture::Data::unload()
{

}
///////////////////////////////////////
Texture::Texture(TextureCreateInfo info)
  : data_(new Data())
{
  data_->info = info;
  data_->cpu_version_++;
}
///////////////////////////////////////
Texture::~Texture()
{

}
///////////////////////////////////////
void Texture::load()
{
  data_->load();
}
///////////////////////////////////////
void Texture::unload()
{
  data_->unload();
}
///////////////////////////////////////
void Texture::set_TextureMode(TextureMode modeU, TextureMode modeV, TextureMode modeW)
{
  data_->addressModeU = (VkSamplerAddressMode)modeU;
  data_->addressModeW = (VkSamplerAddressMode)modeW;
  data_->addressModeV = (VkSamplerAddressMode)modeV;
  data_->cpu_version_++;
}
///////////////////////////////////////
void Texture::set_TextureFilter(TextureFilter min, TextureFilter mag)
{
  data_->minFilter = (VkFilter)min;
  data_->magFilter = (VkFilter)mag;
  data_->cpu_version_++;
}
///////////////////////////////////////
void Texture::set_Size(uint32_t width, uint32_t height)
{
  data_->height = height;
  data_->width = width;
  data_->cpu_version_++;
}

}
}


