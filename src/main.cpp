#include <iostream>
#include <cstdio>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "input.h"
#include "camera.h"
#include "renderer.h"

void error_callback(int error, const char* description);

float focal_dist = 8.0f;
float focal_range = 10.0f;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  if (key == GLFW_KEY_UP && action == GLFW_REPEAT) {
    focal_dist += 0.2f;
    Renderer::SetFocalDistance(focal_dist);
  } else if (key == GLFW_KEY_DOWN && action == GLFW_REPEAT) {
    focal_dist -= 0.2f;
    Renderer::SetFocalDistance(focal_dist);
  }

  if (key == GLFW_KEY_LEFT && action == GLFW_REPEAT) {
    focal_range -= 0.2f;
    Renderer::SetFocalRange(focal_range);
  }
  else if (key == GLFW_KEY_RIGHT && action == GLFW_REPEAT) {
    focal_range += 0.2f;
    Renderer::SetFocalRange(focal_range);
  }
}

int main() {
  std::cout << "HelloWorld!" << std::endl;
  glfwSetErrorCallback(error_callback);
  if (!glfwInit())
  {
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

  GLFWwindow* window = glfwCreateWindow(1280, 720, "TestTask", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    return -2;
  }

  Input::Init(window);

  glfwSetKeyCallback(window, key_callback);
  glfwMakeContextCurrent(window);
  gladLoadGL();
  glfwSwapInterval(1);

  glClearColor(0.8f, 0.75f, 0.85f, 1.0f);

  Renderer::Init();
  Camera cam;

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  while (!glfwWindowShouldClose(window))
  {
    float ratio;
    int width, height;
    glm::mat4 v, p;

    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float)height;
    cam.SetViewportSize(width, height);

    glViewport(0, 0, width, height);

    cam.OnUpdate(1 / 60.0);
    v = cam.GetViewMatrix();
    p = cam.GetProjection();


    Renderer::StartDOF();
    Renderer::SetView((const GLfloat*)&v);
    Renderer::SetProjection((const GLfloat*)&p);
    Renderer::DrawBox({ 0.0f, 0.0f, 0.0f }, { 10.0f, 0.5f, 10.0f });
    Renderer::DrawBox({ 0.0f, 1.25f, 0.0f }, { 2.0f, 2.0f, 2.0f });
    Renderer::DrawBox({ 5.0f, 1.65f, 0.0f }, { 2.0f, 2.0f, 2.0f }, { (float)glfwGetTime() / 10.0f, 0.0f, 0.0f});
    Renderer::DrawBox({ -5.0f, 1.65f, 0.0f }, { 2.0f, 2.0f, 2.0f }, { -(float)glfwGetTime() / 10.0f, 0.0f, 0.0f });
    Renderer::EndDOF();

    glfwSwapBuffers(window);

    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}

void error_callback(int error, const char* description)
{
  fprintf(stderr, "Error: %s\n", description);
}