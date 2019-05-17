#include "components/camera_component.h"

#include <glm/ext.hpp>

#include "scene.h"
#include "node.h"
#include "render_manager.h"



namespace phantom
{

////////////////////////////////////////////////////////////////
struct CameraComponent::Data
{
  glm::mat4 transform{ 1 };
  glm::mat4 view{ 1 };
};

////////////////////////////////////////////////////////////////
CameraComponent::CameraComponent(Node& parent)
  : Component(parent),
  data_(new Data())
{
}

////////////////////////////////////////////////////////////////
CameraComponent::~CameraComponent() = default;

////////////////////////////////////////////////////////////////
void CameraComponent::set_perspective(float yfov,
  float aspect_ratio,
  float znear,
  float zfar)
{
  data_->transform = glm::perspective(yfov, aspect_ratio, znear, zfar);
}

////////////////////////////////////////////////////////////////
void CameraComponent::set_orthographic(float xmag,
  float ymag,
  float znear,
  float zfar)
{
  const float dz = znear - zfar;

  data_->transform = glm::mat4{ {1 / xmag, 0, 0, 0},
                                {0, 1 / ymag, 0, 0},
                                {0, 0, 2 / dz, (znear + zfar) / dz},
                                {0, 0, 0, 1} };
}

////////////////////////////////////////////////////////////////
void CameraComponent::update()
{
  data_->view = glm::inverse(owner_node_.transform()->world_transform());

  //auto& render_manager = GameManager::Instance().render_manager_;
  //render_manager.draw(*owner_node_.scene_, *this, data_->render_mode);
}

////////////////////////////////////////////////////////////////
void CameraComponent::set_screen_size(glm::ivec2 screen_size)
{
  // TODO update view matrix
}

////////////////////////////////////////////////////////////////
const TransformComponent& CameraComponent::transform() const
{
  return *owner_node_.transform();
}

////////////////////////////////////////////////////////////////
glm::mat4 CameraComponent::projection() const
{
  return data_->transform;
}

////////////////////////////////////////////////////////////////
glm::mat4 CameraComponent::view() const
{
  return data_->view;
}

////////////////////////////////////////////////////////////////
rect2i CameraComponent::viewport() const
{
  return rect2i();
}

} // end namespace phantom