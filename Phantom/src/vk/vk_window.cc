#include "graphics/window.h"

#include "vk/vk_Internal.h"


namespace phantom
{
namespace graphics
{


  //////////////////////////////////////////////////////
Window::Window(std::string name, int width, int height)
  : settings(new WindowSettings()),
    data_(new Data())
{
  name.copy(settings->name, name.size());

  settings->size = glm::ivec2(width, height);
  settings->scissor.extent = glm::ivec2(width, height);

  glfwInit();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  data_->window = glfwCreateWindow(settings->size.x, settings->size.y, settings->name, nullptr, nullptr);
  glfwSetWindowUserPointer(data_->window, this);
  glfwSetFramebufferSizeCallback(data_->window, Window::framebufferResizeCallback);

  glfwGetInputMode(data_->window, GLFW_STICKY_KEYS);

}

//////////////////////////////////////////////////////
Window::~Window()
{
  cleanup();
}

//////////////////////////////////////////////////////
void Window::init()
{
  

  if (glfwCreateWindowSurface(phantom::RenderManager::instance ().data_->instance, data_->window, nullptr, &data_->surface) != VK_SUCCESS) {
    throw std::runtime_error("failed to create window surface!");
  }


}
////////////////////////////////////////////////////////////////
bool Window::keyPressed(Key key) const
{
  return glfwGetKey(data_->window, key) == GLFW_PRESS;
}

////////////////////////////////////////////////////////////////
bool Window::mouseButtonPressed(uint8_t button) const
{
  return glfwGetMouseButton(data_->window, button) == GLFW_PRESS;
}

////////////////////////////////////////////////////////////////
bool Window::keyReleased(Key key) const
{
  return glfwGetKey(data_->window, key) == GLFW_RELEASE;
}

////////////////////////////////////////////////////////////////
bool Window::mouseButtonReleased(uint8_t button) const
{
  return glfwGetMouseButton(data_->window, button) == GLFW_RELEASE;
}

////////////////////////////////////////////////////////////////
float Window::mouseAxisValue(int axis) const
{
  return (float)data_->mouse_axis[axis];
}

////////////////////////////////////////////////////////////////
void Window::captureMouse(bool capture) const
{
  auto new_mode = capture ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;
  if (glfwGetInputMode(data_->window, GLFW_CURSOR) != new_mode)
  {
    glfwSetInputMode(data_->window, GLFW_CURSOR, new_mode);
  }
}
//////////////////////////////////////////////////////
GLFWwindow* Window::get()
{
  return data_->window;
}
//////////////////////////////////////////////////////
void Window::cleanup()
{
  glfwDestroyWindow(get());

  glfwTerminate();
}
//////////////////////////////////////////////////////
void Window::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
  //TBI
}
//////////////////////////////////////////////////////
void Window::recreateSwapChain()
{
  //TBI
}
//////////////////////////////////////////////////////
bool phantom::graphics::Window::processEvents()
{
  glfwPollEvents();

  {
    glm::dvec2 new_mouse_pos{};
    glfwGetCursorPos(data_->window, &new_mouse_pos.x, &new_mouse_pos.y);
    data_->mouse_axis = new_mouse_pos - data_->mouse_pos;
    data_->mouse_pos = new_mouse_pos;
  }

  // Global keys
  if (glfwGetKey(data_->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
  {
    return false;
  }

  return true;
}  

//////////////////////////////////////////////////////
void Window::set_ClippingSpace(ClippingSpace& scissors)
{
  settings->scissor = scissors;
}
//////////////////////////////////////////////////////
phantom::ViewportSettings Window::viewport()
{
  //TBI
  ViewportSettings viewportSettings;

  viewportSettings.size = settings->size;

  return viewportSettings;
}
//////////////////////////////////////////////////////

}
}