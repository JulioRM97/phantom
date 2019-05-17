#pragma once

#include <list>

#include "types.h"

namespace phantom
{
  

struct ActionKeyInput
{
  explicit ActionKeyInput(Key key) : key(key) {};

  Key key;
};

struct ActionMouseInput
{
  explicit ActionMouseInput(uint8_t button) : button(button) {};

  uint8_t button;
};

struct Action
{
  std::list<ActionKeyInput> key_inputs;
  std::list<ActionMouseInput> mouse_inputs;

  bool previous = false;
  bool down = false;
  bool up = false;
  bool pressed = false;
};

} // namespace phantom

