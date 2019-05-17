#include "mesh.h"

#include "vk_Internal.h"
#include "components/camera_component.h"

namespace phantom
{

//////////////////////////////////////////////////
void Mesh::Data::load()
{
  ///Create buffers
  auto device = RenderManager::instance().data_->device;
  auto physicalDevice = RenderManager::instance().data_->physicalDevice;

  vertexAttributeBuffers.reserve(meshInfos.size());
  buffers.reserve(meshInfos.size());

  for (auto buffer : meshInfos)
  {
    Vertex vertexInfo;

    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = buffer.size;
    bufferInfo.usage = bufferType(buffer.usage);
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &vertexInfo.buffer) != VK_SUCCESS) {
      throw std::runtime_error("failed to create vertex buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, vertexInfo.buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &vertexInfo.memory) != VK_SUCCESS) {
      throw std::runtime_error("failed to allocate vertex buffer memory!");
    }

    //TODO Add offsets to this function
    vkBindBufferMemory(device, vertexInfo.buffer, vertexInfo.memory, 0);

    void* data;

    //TODO Add offsets to this function
    vkMapMemory(device, vertexInfo.memory, 0, bufferInfo.size, 0, &data);

    //TODO Add offsets to this function
    memcpy(data, buffer.data, (size_t)bufferInfo.size);

    vkUnmapMemory(device, vertexInfo.memory);

    switch (buffer.usage)
    {
      case BufferType::k_Vertex:
      {
        buffers.push_back(vertexInfo.buffer);
      }
      break;

      case BufferType::k_Index:
      {
        indexBuffer = vertexInfo.buffer;
        hasIndexBuffer = true;
      }
      break;

      default:
        break;

    }
  }
  
}
//////////////////////////////////////////////////
void Mesh::Data::unload()
{

}
//////////////////////////////////////////////////
Mesh::Mesh()
  : data_(new Data())
{

}
//////////////////////////////////////////////////
Mesh::~Mesh()
{

}
//////////////////////////////////////////////////
void Mesh::prepare(VkCommandBuffer& buffers)
{
  //TODO
  std::vector<VkDeviceSize> offsets(data_->buffers.size(), 0);

  vkCmdBindVertexBuffers(buffers, 0, data_->buffers.size(), data_->buffers.data(), offsets.data());

  if (data_->hasIndexBuffer)
  {
    vkCmdBindIndexBuffer(buffers, data_->indexBuffer, 0, VK_INDEX_TYPE_UINT16);

    vkCmdDrawIndexed(buffers, data_->indexCount, data_->instanceCount, 0, 0, 0);
  }
  else
  {
    vkCmdDraw(buffers, data_->vertextCount, data_->instanceCount, 0, 0);
  }
}
//////////////////////////////////////////////////
int Mesh::get_bindingPos(int buffer)
{
  if (data_->infos.find(buffer) != data_->infos.end())
  {
    return data_->infos[buffer].binding_pos;
  }

  return data_->infos.size () - 1;
}
//////////////////////////////////////////////////
void Mesh::init()
{

}
//////////////////////////////////////////////////
void Mesh::load()
{
  data_->load();
}
//////////////////////////////////////////////////
void Mesh::unload()
{
  data_->unload();
}
//////////////////////////////////////////////////
void Mesh::addMeshData(MeshInfo& info)
{
  if(info.binding_pos == -1)
    info.binding_pos = data_->meshInfos.size();

  data_->infos[info.pos] = info;
  data_->meshInfos.push_back(info);
}

//////////////////////////////////////////////////
void Mesh::set_VertextCount(ui32 count)
{
  data_->vertextCount = count;
}
//////////////////////////////////////////////////
void Mesh::set_IndexCount(ui32 count)
{
  data_->indexCount = count;
}
//////////////////////////////////////////////////
void Mesh::updateUniforms(phantom::graphics::Material& material)
{

  MeshUniform mvp;
  mvp.view = RenderManager::instance().current_camera_->view();
  mvp.model = glm::scale(glm::mat4(1.0f), glm::vec3(0.02, 0.02, 0.02));

  material.updateUniformBlock("mvp", &mvp, sizeof(MeshUniform));
}

}