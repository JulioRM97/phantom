#include "graphics/material.h"

#include "vk_Internal.h"

namespace phantom
{
namespace graphics
{

/////////////////////////////////////////////////////////
void Material::Data::load()
{
  if (cpu_version_ <= gpu_version_) {
    // Material is up to date... do not update
    return;
  }
  
  uint32_t next_version = cpu_version_;

  createUniformBuffers();

  createDescriptorPool();

  //Reserve Descriptor sets.
  createDescriptorSets();

  shadersCreated.reserve(stages);
  shadersInfo.reserve(stages);

  for (auto toLoad : shaderFromFile)
  {
    for (std::string fileName : toLoad.second)
    {
      Shader shader = createShaderFromFile(fileName, RenderManager::instance().data_->device);

      VkPipelineShaderStageCreateInfo ShaderInfo = {};
      ShaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      ShaderInfo.stage = VkShaderType(toLoad.first);
      ShaderInfo.module = shader;
      ShaderInfo.pName = "main";

      shadersInfo.push_back(ShaderInfo);
      shadersCreated.push_back(shader);
    }
  }

  //TODO Add to load from code.
  
  if (attributesInfo.empty())
    throw std::runtime_error("the attribute info map is empty pls fill it!");
  
  bindingDescriptions.reserve(attributesInfo.size());
  attributeDescriptions.reserve(numAttributes);

  for (auto atri : attributesInfo)
  {
    
    VkVertexInputBindingDescription bindingDescription;
    bindingDescription.binding = atri.first;
    bindingDescription.stride = atri.second[0].stride;
    bindingDescription.inputRate = (VkVertexInputRate)atri.second[0].rate;

    bindingDescriptions.push_back(bindingDescription);

    for (auto attribute : atri.second)
    {
      VkVertexInputAttributeDescription attributeDescription;

      attributeDescription.binding = attribute.binding;
      attributeDescription.location = attribute.location;
      attributeDescription.format = (VkFormat)attribute.format;
      attributeDescription.offset = attribute.offset;

      attributeDescriptions.push_back(attributeDescription);
    }
  }

  VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
  vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount = bindingDescriptions.size ();
  vertexInputInfo.vertexAttributeDescriptionCount = attributeDescriptions.size ();
  vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
  vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();

  VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
  inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  auto window        = RenderManager::instance().window_;
  auto clippingSpace = window->settings->scissor;
  auto viewport      = window->viewport();

  VkViewport vkviewport = {};
  vkviewport.x = viewport.pos.x;
  vkviewport.y = viewport.pos.y;
  vkviewport.width = viewport.size.x;
  vkviewport.height = viewport.size.y;
  //TODO Move the minDepth and maxDepth to the material?
  vkviewport.minDepth = viewport.minDepth;
  vkviewport.maxDepth = viewport.maxDepth;

  VkRect2D scissor = {};
  scissor.offset = { clippingSpace.offset.x, clippingSpace.offset.y };
  scissor.extent = { clippingSpace.extent.x, clippingSpace.extent.y };

  VkPipelineViewportStateCreateInfo viewportState = {};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.pViewports = &vkviewport;
  viewportState.scissorCount = 1;
  viewportState.pScissors = &scissor;

  //TODO Put proper values.
  VkPipelineRasterizationStateCreateInfo rasterizer = {};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;

  //TODO Put proper values.
  VkPipelineMultisampleStateCreateInfo multisampling = {};
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  //TODO Put proper values.
  VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
  colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = VK_FALSE;

  //TODO Put proper values.
  VkPipelineColorBlendStateCreateInfo colorBlending = {};
  colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.logicOp = VK_LOGIC_OP_COPY;
  colorBlending.attachmentCount = 1;
  colorBlending.pAttachments = &colorBlendAttachment;
  colorBlending.blendConstants[0] = 0.0f;
  colorBlending.blendConstants[1] = 0.0f;
  colorBlending.blendConstants[2] = 0.0f;
  colorBlending.blendConstants[3] = 0.0f;

  VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.pushConstantRangeCount = 0;
  pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
  pipelineLayoutInfo.setLayoutCount = 1;

  if (vkCreatePipelineLayout(RenderManager::instance().data_->device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }

  VkPipelineDepthStencilStateCreateInfo depthStencil = {};
  depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depthStencil.depthTestEnable = VK_TRUE;
  depthStencil.depthWriteEnable = VK_TRUE;
  depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
  depthStencil.depthBoundsTestEnable = VK_FALSE;
  depthStencil.stencilTestEnable = VK_FALSE;

  VkGraphicsPipelineCreateInfo pipelineInfo = {};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = stages;
  pipelineInfo.pStages = shadersInfo.data ();
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.layout = pipelineLayout;
  pipelineInfo.renderPass = RenderManager::instance().data_->renderPass;
  pipelineInfo.subpass = 0;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
  pipelineInfo.pDepthStencilState = &depthStencil;

  if (vkCreateGraphicsPipelines(RenderManager::instance().data_->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
    throw std::runtime_error("failed to create graphics pipeline!");
  }

  //TODO Put here to prepare the commandBuffer when we need to render with the material 
  //maybe move the draw part to the mesh so there we can bind the vertex buffer.
  /*{
    commandBuffers.resize(swapChainFramebuffers.size());

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

    if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
      throw std::runtime_error("failed to allocate command buffers!");
    }

    for (size_t i = 0; i < commandBuffers.size(); i++) {
      VkCommandBufferBeginInfo beginInfo = {};
      beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
      beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

      if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
      }

      VkRenderPassBeginInfo renderPassInfo = {};
      renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      renderPassInfo.renderPass = renderPass;
      renderPassInfo.framebuffer = swapChainFramebuffers[i];
      renderPassInfo.renderArea.offset = { 0, 0 };
      renderPassInfo.renderArea.extent = swapChainExtent;

      VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
      renderPassInfo.clearValueCount = 1;
      renderPassInfo.pClearValues = &clearColor;

      vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

      vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

      vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

      vkCmdEndRenderPass(commandBuffers[i]);

      if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
      }
    }
  }*/
  
  for (auto shader : shadersCreated)
  {
    vkDestroyShaderModule(RenderManager::instance().data_->device, shader, nullptr);
  }

  gpu_version_ = next_version;
}
/////////////////////////////////////////////////////////
void Material::Data::unload()
{
  auto& rm = RenderManager::instance();
  vkDestroyPipeline(rm.data_->device, graphicsPipeline, nullptr);
  vkDestroyPipelineLayout(rm.data_->device, pipelineLayout, nullptr);
  vkDestroyDescriptorSetLayout(rm.data_->device, descriptorSetLayout, nullptr);
}
/////////////////////////////////////////////////////////
void Material::Data::createUniformBuffers()
{

  bindings.reserve(uniformBlocks.size() + textures.size());

  int i = 0;
  for (auto uniform : uniformBlocks)
  {
    {
      UniformBlock block;
      uniforms.emplace(uniform.name, block);
    }
    
    UniformBlock &block = uniforms[uniform.name];
    block.uniformBuffers.resize(RenderManager::instance().numberImages());
    block.uniformBuffersMemory.resize(RenderManager::instance().numberImages());
    block.bytes = uniform.bytes;
    block.location = uniform.location;

    VkDescriptorSetLayoutBinding LayoutBinding = {};
    LayoutBinding.binding = uniform.location;
    LayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    LayoutBinding.descriptorCount = 1;
    LayoutBinding.stageFlags = uniform.type == ShaderType::k_Fragment ? VK_SHADER_STAGE_FRAGMENT_BIT : VK_SHADER_STAGE_VERTEX_BIT;
    LayoutBinding.pImmutableSamplers = nullptr; // Optional

    bindings.push_back(LayoutBinding);

    VkDeviceSize bufferSize = uniform.bytes;

    for (size_t x = 0; x < RenderManager::instance().numberImages(); x++) {
      createBuffer(RenderManager::instance().data_->physicalDevice, RenderManager::instance().data_->device,
                   bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                   block.uniformBuffers[x],
                   block.uniformBuffersMemory[x]);
    }

    i++;
  }

  for (auto texture : textures)
  {
    VkDescriptorSetLayoutBinding LayoutBinding = {};
    LayoutBinding.binding = i;
    LayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    LayoutBinding.descriptorCount = 1;
    LayoutBinding.stageFlags =  VK_SHADER_STAGE_FRAGMENT_BIT;
    LayoutBinding.pImmutableSamplers = nullptr; // Optional

    bindings.push_back(LayoutBinding);
    i++;
  }

  VkDescriptorSetLayoutCreateInfo layoutInfo = {};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size()); //
  layoutInfo.pBindings = bindings.data();

  if (vkCreateDescriptorSetLayout(RenderManager::instance().data_->device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor set layout!");
  }

}
/////////////////////////////////////////////////////////
void Material::Data::createDescriptorPool()
{
  if (uniformBlocks.size() + textures.size() <= 0)
    return;

  std::vector<VkDescriptorPoolSize> poolSizes;

  poolSizes.reserve(uniforms.size() + textures.size());

  size_t descriptorCount = RenderManager::instance().numberImages();

  //Load uniform descriptors
  for (size_t i = 0; i < uniforms.size(); i++)
  {
    VkDescriptorPoolSize size;
    size.descriptorCount = descriptorCount;
    size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes.push_back(size);
  }

  //Load texture descriptors
  for (size_t i = 0; i < textures.size(); i++)
  {
    VkDescriptorPoolSize size;
    size.descriptorCount = descriptorCount;
    size.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes.push_back(size);
  }

  VkDescriptorPoolCreateInfo poolInfo = {};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
  poolInfo.pPoolSizes = poolSizes.data();
  poolInfo.maxSets = static_cast<uint32_t>(RenderManager::instance ().numberImages ());

  if (vkCreateDescriptorPool(RenderManager::instance ().data_->device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor pool!");
  }

}
/////////////////////////////////////////////////////////
void Material::Data::createDescriptorSets()
{
  if (uniformBlocks.size() + textures.size() <= 0)
    return;

  auto& rm = RenderManager::instance();

  std::vector<VkDescriptorSetLayout> layouts(rm.numberImages (), descriptorSetLayout);

  VkDescriptorSetAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorPool;
  allocInfo.descriptorSetCount = static_cast<uint32_t>(rm.numberImages ());
  allocInfo.pSetLayouts = layouts.data();

  descriptorSets.resize(rm.numberImages());

  if (vkAllocateDescriptorSets(rm.data_->device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate descriptor sets!");
  }


  for (size_t i = 0; i < RenderManager::instance().numberImages(); i++)
  {
    int x = 0;
    for (auto& block : uniforms)
    {
      VkDescriptorBufferInfo bufferInfo = {};
      bufferInfo.buffer = block.second.uniformBuffers[i];
      bufferInfo.offset = 0;
      bufferInfo.range = block.second.bytes;

      VkWriteDescriptorSet descriptorWrite = {};
      descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptorWrite.dstSet = descriptorSets[i];
      descriptorWrite.dstBinding = block.second.location;
      descriptorWrite.dstArrayElement = 0;
      descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      descriptorWrite.descriptorCount = 1;
      descriptorWrite.pBufferInfo = &bufferInfo;

      vkUpdateDescriptorSets(rm.data_->device,1, &descriptorWrite, 0, nullptr);

      x++;
    }
    
    for (auto texture : textures)
    {
      VkDescriptorImageInfo imageInfo = {};
      imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      imageInfo.imageView = texture->data_->view;
      imageInfo.sampler = texture->data_->textureSampler;

      VkWriteDescriptorSet descriptorWrite = {};
      descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptorWrite.dstSet = descriptorSets[i];
      descriptorWrite.dstBinding = x;
      descriptorWrite.dstArrayElement = 0;
      descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      descriptorWrite.descriptorCount = 1;
      descriptorWrite.pImageInfo = &imageInfo;

      vkUpdateDescriptorSets(rm.data_->device, 1, &descriptorWrite, 0, nullptr);
    }

  }

}
/////////////////////////////////////////////////////////
Material::Material(Type type, std::string name)
  : data_(new Data())
{

}
/////////////////////////////////////////////////////////
Material::~Material()
{
  
}
/////////////////////////////////////////////////////////
void Material::createFromFile(std::string filename, ShaderType type)
{
  data_->shaderFromFile[type].push_back(filename);
  data_->stages++;

  data_->cpu_version_++;
}
/////////////////////////////////////////////////////////
void Material::createFromCode(std::string filename, ShaderType type)
{
  data_->shaderFromCode[type].push_back(filename);
  data_->stages++;

  data_->cpu_version_++;
}
/////////////////////////////////////////////////////////
void Material::enableMultisampling(bool enable, uint count)
{
  //TBI
}
/////////////////////////////////////////////////////////
void Material::enableBlending(BlendData data)
{
  //TBI
}
/////////////////////////////////////////////////////////
void Material::addAttribute(AttributeInfo* info, ui32 num = 1)
{
  auto& atri = data_->attributesInfo[data_->numVertexBuffers];

  for (ui32 i = 0; i < num; ++i)
  {
    atri.push_back(info[i]);
    data_->numAttributes++;
  }

  data_->numVertexBuffers++;

  data_->cpu_version_++;
}
/////////////////////////////////////////////////////////
void Material::addUniformBlock(UniformBlockInfo& info)
{
  //TBI
  data_->uniformBlocks.push_back(info);
}
/////////////////////////////////////////////////////////
void Material::load()
{
  for (auto texture : data_->textures)
  {
    texture->load();
  }

  data_->load();
}
/////////////////////////////////////////////////////////
void Material::unload()
{
  data_->unload();
}

/////////////////////////////////////////////////////////
void Material::prepare(VkCommandBuffer& buffers)
{

  vkCmdBindPipeline(buffers, VK_PIPELINE_BIND_POINT_GRAPHICS, data_->graphicsPipeline);

  if(data_->descriptorSets.size () > 0)
    vkCmdBindDescriptorSets(buffers, VK_PIPELINE_BIND_POINT_GRAPHICS, data_->pipelineLayout,
                            0, 1,
                            &data_->descriptorSets[RenderManager::instance ().currentImage ()],
                            0, nullptr);
}
/////////////////////////////////////////////////////////
void Material::updateUniformBlock(std::string name, void* data, size_t bytes)
{
  if (data_->uniforms.find(name) == data_->uniforms.end())
    return;

  auto& rm = RenderManager::instance();

  void* data_local;
  int currentImage = rm.currentImage();

  vkMapMemory(rm.data_->device, data_->uniforms[name].uniformBuffersMemory[currentImage], 0, bytes, 0, &data_local);

  memcpy(data_local, data, bytes);

  vkUnmapMemory(rm.data_->device, data_->uniforms[name].uniformBuffersMemory[currentImage]);

}
/////////////////////////////////////////////////////////
void Material::updateUniformBlocks()
{
 /* auto& rm = RenderManager::instance();

  int i = 0;
  for (auto uniform : data_->uniformBlocks)
  {
    void* data;
    vkMapMemory(rm.data_->device, data_->uniformBuffersMemory[(i * rm.numberImages ()) + rm.currentImage()], 
                                                              0, uniform.bytes, 0, &data);

    memcpy(data, uniform.data, uniform.bytes);

    vkUnmapMemory(rm.data_->device, data_->uniformBuffersMemory[(i * rm.numberImages()) + rm.currentImage()]);
  }*/
}

void Material::addTexture(std::shared_ptr<Texture> info)
{
  data_->textures.push_back(info);
}

}
}