#pragma once

#include "component.h"

#include <memory>
#include <utility>
#include <vector>

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

#include "components/transform_component.h"

namespace phantom
{

class CameraComponent : public Component
{
public:
  explicit CameraComponent(Node& parent);

  ~CameraComponent() override;

  void set_perspective(float yfov,
    float aspect_ratio,
    float znear,
    float zfar);

  void set_orthographic(float xmag,
    float ymag,
    float znear,
    float zfar);

  void update() override;

  void set_screen_size(glm::ivec2 screen_size);


  const TransformComponent& transform() const;

  glm::mat4 projection() const;

  glm::mat4 view() const ;

  rect2i viewport() const;

private:
  struct Data;

  std::unique_ptr<Data> data_;
};

} // end namespace phantom