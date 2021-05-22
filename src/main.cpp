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

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main() {
  std::cout << "HelloWorld!" << std::endl;
  glfwSetErrorCallback(error_callback);
  if (!glfwInit())
  {
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

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
    glm::mat4 vp;

    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float)height;
    cam.SetViewportSize(width, height);

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    vp = cam.GetViewProjection();

    cam.OnUpdate(1 / 60.0);

    Renderer::SetVP((const GLfloat*)&vp);
    Renderer::DrawBox({ 0.0f, 0.0f, 0.0f }, { 10.0f, 0.5f, 10.0f });
    Renderer::DrawBox({ 0.0f, 1.25f, 0.0f }, { 2.0f, 2.0f, 2.0f });
    Renderer::DrawBox({ 2.0f, 1.65f, 0.0f }, { 2.0f, 2.0f, 2.0f }, { (float)glfwGetTime(), 0.0f, 0.0f});
    Renderer::DrawBox({ -2.0f, 1.65f, 0.0f }, { 2.0f, 2.0f, 2.0f }, { -(float)glfwGetTime(), 0.0f, 0.0f });

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