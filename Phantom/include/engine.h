#pragma once

#include "render_manager.h"
#include "asset_manager.h"
#include "threaded_scheduler.h"
#include "input/input_manager.h"
#include "scene.h"
#include "timer.h"

class Phantom
{
public:
  Phantom();
  ~Phantom();

  void start();
  void run();
  void destroy();



  phantom::RenderManager* render_manager_;

  phantom::AssetManager asset_manager_;

  phantom::ThreadedScheduler scheduler_;

  phantom::InputManager input_manager_;

  const phantom::Scene* current_scene() const;

 private:
   void addMissingRequiredComponents();

  void runFrame();

  /// Run all systems in sequence
  void runScenePreUpdate();

  /// Run all systems and all updatables in parallel but waits each execution
  /// for all to finish
  void runSceneFixedUpdate(uint32_t count);

  /// Run all systems in parallel
  void runSceneUpdate();

  /// Run all systems in sequence
  void runScenePostUpdate();

  std::deque<std::shared_ptr<phantom::Scene>> scenes_;

  int current_scene_id_ = 0;

  phantom::Timer frame_timer_;

  bool addedMissingRequiredComponents = false;
};


