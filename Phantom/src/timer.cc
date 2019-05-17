#include "timer.h"

namespace phantom
{

  float Time::deltaTime = 0.0f;
  float Time::fixedDeltaTime = 40.0f / 1000.0f;
  float Time::elapsedTime = 0.0f;
  float Time::remainingSimulationTime = 0.0f;

////////////////////////////////////////////////////////////////
void Timer::start()
{
  QueryPerformanceCounter(&start_time_);
}

////////////////////////////////////////////////////////////////
float Timer::elapsed(bool restart)
{
  LARGE_INTEGER frequency;
  QueryPerformanceFrequency(&frequency);

  LARGE_INTEGER end_time;
  QueryPerformanceCounter(&end_time);

  LARGE_INTEGER original_start = start_time_;

  if (restart)
  {
    start_time_ = end_time;
  }

  return static_cast<float>((end_time.QuadPart - original_start.QuadPart) /
    (double)frequency.QuadPart);
}

} // end namespace phantom

