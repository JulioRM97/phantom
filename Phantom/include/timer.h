#pragma once


#include <windows.h>

typedef LARGE_INTEGER START_TYPE;


namespace phantom
{

struct Time
{
  static float deltaTime;
  static float fixedDeltaTime;
  static float remainingSimulationTime;
  static float elapsedTime;
};

////////////////////////////////////////////////////////////////
/// \summary High precision timer
class Timer
{
public:
  /// \summary Start the timer
  void start();

  /// \returns time elapsed since the start of the timer in seconds
  float elapsed(bool restart = false);

  /// Restarts the timer.
  void restart() { elapsed(true); };

private:
  START_TYPE start_time_;
};

}
