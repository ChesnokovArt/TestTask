#include "input.h"

GLFWwindow* Input::m_window = nullptr;

bool Input::IsKeyPressed(const KeyCode key)
{
  auto state = glfwGetKey(m_window, static_cast<int32_t>(key));
  return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Input::IsMouseButtonPressed(const MouseCode button)
{
  auto state = glfwGetMouseButton(m_window, static_cast<int32_t>(button));
  return state == GLFW_PRESS;
}

glm::vec2 Input::GetMousePosition()
{
  double xpos, ypos;
  glfwGetCursorPos(m_window, &xpos, &ypos);

  return { (float)xpos, (float)ypos };
}

float Input::GetMouseX()
{
  return GetMousePosition().x;
}

float Input::GetMouseY()
{
  return GetMousePosition().y;
}