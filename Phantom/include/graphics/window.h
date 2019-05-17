#pragma once

#include "types.h"

#include <string>
#include <memory>

struct GLFWwindow;

namespace phantom
{
namespace graphics
{

class Window
{
 public:
   explicit Window(std::string name, int width, int height);

   ~Window();

   void init();

   void cleanup();

   bool processEvents();

   void set_ClippingSpace(ClippingSpace& scissors);

   bool keyPressed(Key key) const;

   bool mouseButtonPressed(uint8_t button) const;

   bool keyReleased(Key key) const;

   bool mouseButtonReleased(uint8_t button) const;

   float mouseAxisValue(int axis) const;

   void captureMouse(bool capture = true) const;

   ViewportSettings viewport();

   GLFWwindow* get();

   std::unique_ptr<WindowSettings> settings;

   struct Data;
   std::unique_ptr<Data> data_;

 private:

  static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

  void recreateSwapChain();

};

}//end namespace graphics
}//end namespace phantom
