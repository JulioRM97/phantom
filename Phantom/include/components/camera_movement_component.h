#pragma once

#pragma once

#include "component.h"
#include "input/input_manager.h"
#include <string>

namespace phantom
{

  class CameraControlMovementComponent : public Component
  {
  public:
    explicit CameraControlMovementComponent(Node& parent);

    ~CameraControlMovementComponent() override;

    void update() override;

    void registerDefaultInput(InputManager& inputManager);

    float cam_speed_ = 5.0f;

    float rotate_speed_ = 10.0f;

  private:

    float yaw_ = 0.0f;

    float pitch_ = 0.0f;

    InputManager* inputManager_;
  };

} // end namespace phantom


