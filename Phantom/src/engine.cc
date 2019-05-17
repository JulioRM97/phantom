#include "engine.h"

#include "GLFW/glfw3.h"

#include "render_manager.h"
#include "graphics/window.h"
#include "node.h"
#include "components/camera_component.h"
#include "components/camera_movement_component.h"
#include "scene.h"


Phantom::Phantom()
{
  render_manager_ = &phantom::RenderManager::instance();

}

Phantom::~Phantom()
{
}

void Phantom::start()
{

  scheduler_.startConsumers(3);
  render_manager_->init();
  frame_timer_.start();
  addMissingRequiredComponents();
}

void Phantom::run()
{
  while (!glfwWindowShouldClose(render_manager_->window_->get())) 
  {

    render_manager_->window_->processEvents();

    // Sync and wait for previous frame tasks to finish

    phantom::Time::deltaTime = frame_timer_.elapsed(true);

    scheduler_.schedule(std::bind(&Phantom::runFrame, this));

    scheduler_.flush();


    render_manager_->draw();
  }

  render_manager_->mainLoop();
}

void Phantom::destroy()
{
  render_manager_->cleanup();

}

const phantom::Scene* Phantom::current_scene() const
{
  if (current_scene_id_ >= 0 && (size_t)current_scene_id_ < scenes_.size())
  {
    return scenes_[current_scene_id_].get();
  }

  return nullptr;
}

void Phantom::addMissingRequiredComponents()
{
  if (addedMissingRequiredComponents)
  {
    return;
  }
  addedMissingRequiredComponents = true;

  if (scenes_.empty())
  {
    scenes_.emplace_back(new phantom::Scene());
    current_scene_id_ = 0;
  }

  for (auto& scene : scenes_)
  {
    auto camera = render_manager_->current_camera_;

    if (!camera)
    {
      auto& node = scene->addChild();
      node.transform()->set_local_translation(glm::vec3());

      camera = node.createComponent<phantom::CameraComponent>();
      camera->set_perspective(2.0f / 3.0f, 1.5f, 0.1f, 100.0f);

      auto* mov = node.createComponent<phantom::CameraControlMovementComponent>();
      mov->registerDefaultInput(input_manager_);

      render_manager_->current_camera_ = camera;
    }

  }
}

void Phantom::runFrame()
{
  // Run pre update in sequence
  {
    scheduler_.schedule(std::bind(&Phantom::runScenePreUpdate,this)).sync ();
  }


  // Run fixed updates in parallel
  {
    const auto dt = phantom::Time::fixedDeltaTime;
    phantom::Time::elapsedTime += phantom::Time::deltaTime;
    phantom::Time::remainingSimulationTime += phantom::Time::deltaTime;
    const auto count = (uint32_t)(phantom::Time::remainingSimulationTime / dt);
    phantom::Time::remainingSimulationTime -= count * dt;

    scheduler_.schedule(std::bind(&Phantom::runSceneFixedUpdate, this, count));
  }

  // Tell the scheduler to wait for all the fixed updates to finish
  scheduler_.sync();

  input_manager_.update();

  // Run updates in parallel
  {
    scheduler_.schedule(std::bind(&Phantom::runSceneUpdate, this), true);
  }

  // Tell the scheduler to wait for all the updates to finish
  scheduler_.sync();

  // Run post update in sequence
  {
    scheduler_.schedule(std::bind(&Phantom::runScenePostUpdate, this)).sync();
  }
}

void Phantom::runScenePreUpdate()
{
  for (const auto& game_system : current_scene()->game_systems_)
  {
    game_system.second->preUpdate();
  }
}

void Phantom::runSceneFixedUpdate(uint32_t count)
{
  for (uint32_t i = 0; i < count; i++)
  {
    for (const auto& pair : current_scene ()->game_systems_)
    {
      auto game_system = pair.second;
      scheduler_.schedule(std::bind(&phantom::Updatable::fixedUpdate, game_system.get()));
    }

    // Sync every cycle of simulation
    scheduler_.sync();
  }
}

void Phantom::runSceneUpdate()
{
  for (const auto& pair : current_scene()->game_systems_)
  {
    auto game_system = pair.second;
    scheduler_.schedule(std::bind(&phantom::Updatable::update, game_system.get()));
  }
}

void Phantom::runScenePostUpdate()
{
  for (const auto& game_system : current_scene ()->game_systems_)
  {
    game_system.second->postUpdate();
  }
}
