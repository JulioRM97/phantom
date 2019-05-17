#include "render_manager.h"

#include "graphics/window.h"
#include "components/camera_component.h"

#include "vk_Internal.h"

#include <stdexcept>


namespace phantom
{

/////////////////////////////////////////////////////////////////////////
RenderManager::RenderManager()
  : data_(new Data())
{

}

/////////////////////////////////////////////////////////////////////////
RenderManager::~RenderManager()
{

}

/////////////////////////////////////////////////////////////////////////
void RenderManager::init()
{
  window_ = new phantom::graphics::Window("Hello triangle 2.0", 800, 600);

  createInstance();//leave here
  setupDebugMessenger();//leave here

  window_->init();

  pickPhysicalDevice();//leave here

  createLogicalDevice();// leave here

  createSwapChain();// leave here?

  createImageViews();// move to texture

  createRenderPass();// to material?

  createCommandPool();// leave here

  createDepthTexture();

  createFramebuffers();//create in framebufer

  //createDescriptorPool();

  loadObjects();

  createCommandBuffers();// move to framebuffer

  createSyncObjects();// do not know.

}
/////////////////////////////////////////////////////////////////////////
void RenderManager::createInstance()
{
  VkApplicationInfo info;

  info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  info.pApplicationName = "Debug";
  info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  info.pEngineName = "Phantom";
  info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  info.apiVersion = VK_API_VERSION_1_0;


  VkInstanceCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &info;

  //TODO Improve this
  auto extensions = getRequiredExtensions();
  createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  createInfo.ppEnabledExtensionNames = extensions.data();

  if (enableValidationLayers) {
    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
  }
  else {
    createInfo.enabledLayerCount = 0;
  }

  if (vkCreateInstance(&createInfo, nullptr, &data_->instance) != VK_SUCCESS) {
    throw std::runtime_error("failed to create instance!");
  }

}
/////////////////////////////////////////////////////////////////////////
std::vector<const char*> RenderManager::getRequiredExtensions() {
  uint32_t glfwExtensionCount = 0;
  const char** glfwExtensions;
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

  if (enableValidationLayers) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  return extensions;
}
/////////////////////////////////////////////////////////////////////////
void RenderManager::setupDebugMessenger()
{
  if (!enableValidationLayers) return;

  VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = debugCallback;

  if (CreateDebugUtilsMessengerEXT(data_->instance, &createInfo, nullptr, &data_->debugMessenger) != VK_SUCCESS) {
    throw std::runtime_error("failed to set up debug messenger!");
  }
}
/////////////////////////////////////////////////////////////////////////
void RenderManager::createSurface()
{
  //REMOVE
}
/////////////////////////////////////////////////////////////////////////
void RenderManager::cleanup()
{
  {
    for (auto framebuffer : data_->swapChainFramebuffers) {
      vkDestroyFramebuffer(data_->device, framebuffer, nullptr);
    }

    vkFreeCommandBuffers(data_->device, data_->commandPool, static_cast<uint32_t>(data_->commandBuffers.size()), data_->commandBuffers.data());

    vkDestroyPipeline(data_->device, data_->graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(data_->device, data_->pipelineLayout, nullptr);
    vkDestroyRenderPass(data_->device, data_->renderPass, nullptr);

    for (auto imageView : data_->swapChainImageViews) {
      vkDestroyImageView(data_->device, imageView, nullptr);
    }

    vkDestroySwapchainKHR(data_->device, data_->swapChain, nullptr);
  }
  


  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkDestroySemaphore(data_->device, data_->renderFinishedSemaphores[i], nullptr);
    vkDestroySemaphore(data_->device, data_->imageAvailableSemaphores[i], nullptr);
    vkDestroyFence(data_->device, data_->inFlightFences[i], nullptr);
  }

  vkDestroyCommandPool(data_->device, data_->commandPool, nullptr);
  vkDestroyDescriptorPool(data_->device, data_->descriptorPool, nullptr);

  if (enableValidationLayers) {
    DestroyDebugUtilsMessengerEXT(data_->instance, data_->debugMessenger, nullptr);
  }
  vkDestroyDevice(data_->device, nullptr);

  vkDestroySurfaceKHR(data_->instance, phantom::graphics::surface(), nullptr);
  vkDestroyInstance(data_->instance, nullptr);

  delete window_;

}
/////////////////////////////////////////////////////////////////////////
void RenderManager::pickPhysicalDevice()
{
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(data_->instance, &deviceCount, nullptr);

  if (deviceCount == 0) {
    throw std::runtime_error("failed to find GPUs with Vulkan support!");
  }

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(data_->instance, &deviceCount, devices.data());

  for (const auto& device : devices) {
    if (checkDevice(device, phantom::graphics::surface())) {
      data_->physicalDevice = device;
      break;
    }
  }

  if (data_->physicalDevice == VK_NULL_HANDLE) {
    throw std::runtime_error("failed to find a suitable GPU!");
  }
}
/////////////////////////////////////////////////////////////////////////
void RenderManager::createLogicalDevice()
{
  QueueFamilyIndices indices = findQueueFamilies(data_->physicalDevice, phantom::graphics::surface());

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

  float queuePriority = 1.0f;
  for (uint32_t queueFamily : uniqueQueueFamilies) {
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back(queueCreateInfo);
  }

  VkPhysicalDeviceFeatures deviceFeatures = {};

  VkDeviceCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

  createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
  createInfo.pQueueCreateInfos = queueCreateInfos.data();

  createInfo.pEnabledFeatures = &deviceFeatures;

  createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
  createInfo.ppEnabledExtensionNames = deviceExtensions.data();

  if (enableValidationLayers) {
    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
  }
  else {
    createInfo.enabledLayerCount = 0;
  }

  if (vkCreateDevice(data_->physicalDevice, &createInfo, nullptr, &data_->device) != VK_SUCCESS) {
    throw std::runtime_error("failed to create logical device!");
  }

  vkGetDeviceQueue(data_->device, indices.graphicsFamily.value(), 0, &data_->graphicsQueue);
  vkGetDeviceQueue(data_->device, indices.presentFamily.value(), 0, &data_->presentQueue);
}
/////////////////////////////////////////////////////////////////////////
void RenderManager::createSwapChain()
{
  SwapChainSupportDetails swapChainSupport = checkSwapChain(data_->physicalDevice, phantom::graphics::surface());

  VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
  VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
  VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, window_->get ());

  uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
  if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
    imageCount = swapChainSupport.capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = phantom::graphics::surface();
  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  QueueFamilyIndices indices = findQueueFamilies(data_->physicalDevice, phantom::graphics::surface());
  uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

  if (indices.graphicsFamily != indices.presentFamily) {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  }
  else {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }

  createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;

  if (vkCreateSwapchainKHR(data_->device, &createInfo, nullptr, &data_->swapChain) != VK_SUCCESS) {
    throw std::runtime_error("failed to create swap chain!");
  }

  vkGetSwapchainImagesKHR(data_->device, data_->swapChain, &imageCount, nullptr);
  data_->swapChainImages.resize(imageCount);
  vkGetSwapchainImagesKHR(data_->device, data_->swapChain, &imageCount, data_->swapChainImages.data());

  data_->swapChainImageFormat = surfaceFormat.format;
  data_->swapChainExtent = extent;
}
/////////////////////////////////////////////////////////////////////////
void RenderManager::createImageViews()
{
  data_->swapChainImageViews.resize(data_->swapChainImages.size());

  for (size_t i = 0; i < data_->swapChainImages.size(); i++) {
    VkImageViewCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = data_->swapChainImages[i];
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = data_->swapChainImageFormat;
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(data_->device, &createInfo, nullptr, &data_->swapChainImageViews[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create image views!");
    }
  }
}
/////////////////////////////////////////////////////////////////////////
void RenderManager::createRenderPass()
{
  VkAttachmentDescription colorAttachment = {};
  colorAttachment.format = data_->swapChainImageFormat;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentDescription depthAttachment = {};
  depthAttachment.format = VK_FORMAT_D32_SFLOAT;
  depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference colorAttachmentRef = {};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depthAttachmentRef = {};
  depthAttachmentRef.attachment = 1;
  depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass = {};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;
  subpass.pDepthStencilAttachment = &depthAttachmentRef;

  VkSubpassDependency dependency = {};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };

  VkRenderPassCreateInfo renderPassInfo = {};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
  renderPassInfo.pAttachments = attachments.data();
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependency;

  if (vkCreateRenderPass(data_->device, &renderPassInfo, nullptr, &data_->renderPass) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }


}
/////////////////////////////////////////////////////////////////////////
void RenderManager::createGraphicsPipeline()
{
  auto vertShaderCode = readFile("hello_triangle_vert.spv");
  auto fragShaderCode = readFile("hello_triangle_frag.spv");

  Shader vertShaderModule = createShader(vertShaderCode, data_->device);
  Shader fragShaderModule = createShader(fragShaderCode, data_->device);

  VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
  vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertShaderStageInfo.module = vertShaderModule;
  vertShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
  fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShaderStageInfo.module = fragShaderModule;
  fragShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

  VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
  vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount = 0;
  vertexInputInfo.vertexAttributeDescriptionCount = 0;

  VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
  inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  VkViewport viewport = {};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)data_->swapChainExtent.width;
  viewport.height = (float)data_->swapChainExtent.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor = {};
  scissor.offset = { 0, 0 };
  scissor.extent = data_->swapChainExtent;

  VkPipelineViewportStateCreateInfo viewportState = {};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.pViewports = &viewport;
  viewportState.scissorCount = 1;
  viewportState.pScissors = &scissor;

  VkPipelineRasterizationStateCreateInfo rasterizer = {};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;

  VkPipelineMultisampleStateCreateInfo multisampling = {};
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
  colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = VK_FALSE;

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
  pipelineLayoutInfo.setLayoutCount = 0;
  pipelineLayoutInfo.pushConstantRangeCount = 0;

  if (vkCreatePipelineLayout(data_->device, &pipelineLayoutInfo, nullptr, &data_->pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }

  VkGraphicsPipelineCreateInfo pipelineInfo = {};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = 2;
  pipelineInfo.pStages = shaderStages;
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.layout = data_->pipelineLayout;
  pipelineInfo.renderPass = data_->renderPass;
  pipelineInfo.subpass = 0;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

  if (vkCreateGraphicsPipelines(data_->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &data_->graphicsPipeline) != VK_SUCCESS) {
    throw std::runtime_error("failed to create graphics pipeline!");
  }

  vkDestroyShaderModule(data_->device, fragShaderModule, nullptr);
  vkDestroyShaderModule(data_->device, vertShaderModule, nullptr);
}
/////////////////////////////////////////////////////////////////////////
void RenderManager::createFramebuffers()
{
  data_->swapChainFramebuffers.resize(data_->swapChainImageViews.size());

  for (size_t i = 0; i < data_->swapChainImageViews.size(); i++) {
    std::array<VkImageView, 2> attachments = {
        data_->swapChainImageViews[i],
        data_->depthImageView
    };

    VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = data_->renderPass;
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = data_->swapChainExtent.width;
    framebufferInfo.height = data_->swapChainExtent.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(data_->device, &framebufferInfo, nullptr, &data_->swapChainFramebuffers[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create framebuffer!");
    }
  }
}
/////////////////////////////////////////////////////////////////////////
void RenderManager::createCommandPool()
{
  QueueFamilyIndices queueFamilyIndices = findQueueFamilies(data_->physicalDevice, phantom::graphics::surface());

  VkCommandPoolCreateInfo poolInfo = {};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

  if (vkCreateCommandPool(data_->device, &poolInfo, nullptr, &data_->commandPool) != VK_SUCCESS) {
    throw std::runtime_error("failed to create command pool!");
  }
}
/////////////////////////////////////////////////////////////////////////
void RenderManager::createCommandBuffers()
{

  data_->commandBuffers.resize(data_->swapChainFramebuffers.size());

  VkCommandBufferAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = data_->commandPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = (uint32_t)data_->commandBuffers.size();

  if (vkAllocateCommandBuffers(data_->device, &allocInfo, data_->commandBuffers.data()) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers!");
  }

  for (size_t i = 0; i < data_->commandBuffers.size(); i++) {
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    if (vkBeginCommandBuffer(data_->commandBuffers[i], &beginInfo) != VK_SUCCESS) {
      throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = data_->renderPass;
    renderPassInfo.framebuffer = data_->swapChainFramebuffers[i];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = data_->swapChainExtent;

    std::array<VkClearValue, 2> clearValues = {};
    clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
    clearValues[1].depthStencil = { 1.0f, 0 };

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(data_->commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    for (auto draw : objectsToDraw_)
    {
      draw.material.prepare(data_->commandBuffers[i]);
      draw.mesh.prepare(data_->commandBuffers[i]);
    }

    vkCmdEndRenderPass(data_->commandBuffers[i]);

    if (vkEndCommandBuffer(data_->commandBuffers[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to record command buffer!");
    }
  }
}
/////////////////////////////////////////////////////////////////////////
void RenderManager::createSyncObjects()
{
  data_->imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
  data_->renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
  data_->inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

  VkSemaphoreCreateInfo semaphoreInfo = {};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo = {};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    if (vkCreateSemaphore(data_->device, &semaphoreInfo, nullptr, &data_->imageAvailableSemaphores[i]) != VK_SUCCESS ||
      vkCreateSemaphore(data_->device, &semaphoreInfo, nullptr, &data_->renderFinishedSemaphores[i]) != VK_SUCCESS ||
      vkCreateFence(data_->device, &fenceInfo, nullptr, &data_->inFlightFences[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create synchronization objects for a frame!");
    }
  }
}
/////////////////////////////////////////////////////////////////////////
void RenderManager::createDescriptorPool()
{
  VkDescriptorPoolSize poolSize = {};
  poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSize.descriptorCount = static_cast<uint32_t>(data_->swapChainImages.size());

  VkDescriptorPoolCreateInfo poolInfo = {};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = 1;
  poolInfo.pPoolSizes = &poolSize;
  poolInfo.maxSets = static_cast<uint32_t>(data_->swapChainImages.size());

  if (vkCreateDescriptorPool(data_->device, &poolInfo, nullptr, &data_->descriptorPool) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor pool!");
  }
}
/////////////////////////////////////////////////////////////////////////
void RenderManager::draw()
{
  for (auto& lightData : lights_[LightType::k_Dir])
  {
    auto mat = LightMaterials_[LightType::k_Dir];
    lightData.camera_pos = current_camera_->transform().world_translation();

    mat->updateUniformBlock("light", &lightData, sizeof(LightData));
    drawFrame();
  }
}
/////////////////////////////////////////////////////////////////////////
void RenderManager::drawFrame()
{
  vkWaitForFences(data_->device, 1, &data_->inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

  uint32_t imageIndex;
  VkResult result = vkAcquireNextImageKHR(data_->device, data_->swapChain, std::numeric_limits<uint64_t>::max(), data_->imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    //recreateSwapChain();
    return;
  }
  else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("failed to acquire swap chain image!");
  }

  updateUniformBlocks();

  VkSubmitInfo submitInfo = {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = { data_->imageAvailableSemaphores[currentFrame] };
  VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &data_->commandBuffers[imageIndex];

  VkSemaphore signalSemaphores[] = { data_->renderFinishedSemaphores[currentFrame] };
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  vkResetFences(data_->device, 1, &data_->inFlightFences[currentFrame]);

  if (vkQueueSubmit(data_->graphicsQueue, 1, &submitInfo, data_->inFlightFences[currentFrame]) != VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer!");
  }

  VkPresentInfoKHR presentInfo = {};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapChains[] = { data_->swapChain };
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;

  presentInfo.pImageIndices = &imageIndex;

  result = vkQueuePresentKHR(data_->presentQueue, &presentInfo);

  /*if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
    framebufferResized = false;
    //recreateSwapChain();
  }
  else if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to present swap chain image!");
  }*/

  currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void RenderManager::mainLoop()
{
  
  

  //vkDeviceWaitIdle(data_->device);
  
}
/////////////////////////////////////////////////////
void RenderManager::set_DepthValues(float minDepth, float maxDepth /*= 1.0f*/)
{

}
/////////////////////////////////////////////////////
size_t RenderManager::numberImages()
{
  return data_->swapChainImages.size();
}
/////////////////////////////////////////////////////
void RenderManager::renderMeshWithMaterial(Mesh& mesh, graphics::Material& material)
{
  DrawInfo info{mesh, material};

  objectsToDraw_.push_back(info);
}
/////////////////////////////////////////////////////
void RenderManager::loadObjects()
{

  for (auto& object : objectsToDraw_)
  {
    object.mesh.load();
    object.material.load();
  }

}
/////////////////////////////////////////////////////
void RenderManager::updateUniformBlocks()
{
  for (auto& object : objectsToDraw_)
  {
    object.mesh.updateUniforms(object.material);
  }

}
/////////////////////////////////////////////////////
void RenderManager::addLight(LightData data, 
                             LightType type,
                             std::shared_ptr< phantom::graphics::Material> material)
{
  lights_[type].push_back(data);
  LightMaterials_[type] = material;
}
/////////////////////////////////////////////////////
void RenderManager::createDepthTexture()
{
  createImage(data_->device,
              data_->physicalDevice,
              data_->swapChainExtent.width, 
              data_->swapChainExtent.height, 
              VK_FORMAT_D32_SFLOAT, 
              VK_IMAGE_TILING_OPTIMAL, 
              VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 
              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
              data_->depthImage, 
              data_->depthImageMemory);

  data_->depthImageView = createImageView(data_->device, data_->depthImage, VK_FORMAT_D32_SFLOAT, VK_IMAGE_ASPECT_DEPTH_BIT);

  VkCommandBuffer buffer = beginSingleTimeCommand(data_->device, data_->commandPool);

   transitionImageLayout(buffer, data_->depthImage,
                         VK_FORMAT_D32_SFLOAT, VK_IMAGE_LAYOUT_UNDEFINED, 
                         VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
  
  endSingleTimeCommand(data_->device, data_->commandPool, data_->graphicsQueue, buffer);
}


}
